#ifndef ANDROIDEVENT_HPP_
#define ANDROIDEVENT_HPP_

#include "AxesEvent.hpp"

#include <vector>
#include <limits>
#include <cstdint>
#include <cassert>
#include <ratio>
#include <iostream>

/**
 * @brief Identifies the endianess of the system.
 */
static bool system_big_endian()
{
    union {
        std::uint32_t n;
        char c[4];
    } chint {0x01020304};

    return chint.c[0] == 1;
}

/**
 * @brief Swap the byte order.
 */
template <typename T>
static T swap_endians(T value)
{
  auto bytes = reinterpret_cast<char *>(&value);
  for (std::size_t i = 0; i < sizeof(value) / 2; ++i)
      std::swap(bytes[i], bytes[sizeof(value) - i - 1]);
  return value;
}

/**
 * @brief Transforms a variable from
 *        the big endian to the local endianess.
 */
template <typename T>
static T big_to_local(const T &value)
{
  if (system_big_endian())
    return value;
  return swap_endians(value);
}

/**
 * @brief Transforms a variable from
 *        the local endianess to the big endian.
 */
template <typename T>
static T local_to_big(const T &value)
{
  return big_to_local(value);
}

namespace TrackMe {

/**
 * @brief Android SensorEvent is a specialization of the AxesEvent supporting
 *        reading and writing from and to a byte array.
 */
class AndroidEvent : public AxesEvent<double, std::int64_t, std::nano>
{
  public:
    static AndroidEvent fromByteArray(const std::vector<char> &);
    std::vector<char> toByteArray() const;

    /**
     * @brief The size of the byte array.
     */
    static const int byteArraySize = 25;
    /**
     * @brief The value of the magic number.
     */
    static const std::int32_t magicNumber = 0x56289740;
};

/**
 * @brief Stores an event to a big endian byte array.
 */
inline std::vector<char> AndroidEvent::toByteArray() const
{
  static_assert(std::numeric_limits<float>::is_iec559,
                "Only iec559 floats supported");
  static_assert(sizeof(float) == 4,
                "Only single precision floats supported");

  std::vector<char> ba(byteArraySize);
  auto bytes = ba.data();

  *reinterpret_cast<std::int32_t *>(bytes) =
      local_to_big(static_cast<std::int32_t>(magicNumber));
  bytes += 4;

  *reinterpret_cast<EventType *>(bytes) = type;
  bytes += 1;

  *reinterpret_cast<std::int64_t *>(bytes) = local_to_big(timestamp);
  bytes += 8;

  for (auto v : value) {
     *reinterpret_cast<float *>(bytes) = local_to_big(static_cast<float>(v));
     bytes += 4;
  }

  return ba;
}

/**
 * @brief Reads an event from a big endian byte array.
 */
inline AndroidEvent AndroidEvent::fromByteArray(const std::vector<char> &ba)
{
  static_assert(std::numeric_limits<float>::is_iec559,
                "Only iec559 floats supported");
  static_assert(sizeof(float) == 4,
                "Only single precision floats supported");

  AndroidEvent event;

  auto bytes = ba.data();
  // Read magic number
  std::int32_t magic = big_to_local(
                        *reinterpret_cast<const std::int32_t *>(bytes));
  bytes += 4;

  // Check magic number
  if(magic != magicNumber) {
    std::cerr << "WARNING: Android event magic number does not match.\n";
    return AndroidEvent{}; // unknown android event
  }

  // Read type
  event.type = *reinterpret_cast<const EventType *>(bytes);
  bytes += 1;

  // Read timestamp
  event.timestamp = big_to_local(
                    *reinterpret_cast<const std::int64_t *>(bytes));
  bytes += 8;

  // Read values
  for (auto &v : event.value) {
     v = big_to_local(*reinterpret_cast<const float *>(bytes));
     bytes += 4;
  }

  return event;
}

} // namespace TrackMe
#endif
