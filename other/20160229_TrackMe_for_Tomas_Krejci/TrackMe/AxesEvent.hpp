#ifndef AXESEVENT_HPP__
#define AXESEVENT_HPP__

#include "EventType.hpp"
#include "ArrayMatrix.hpp"

#include <sstream>
#include <string>
#include <ratio>
#include <cstdint>

namespace TrackMe {

/**
 * @brief Axes event represents a sensor event with three values relative
 *        to the three coordinate axes.
 *
 *        For instance, accelerometer, gyroscope, magnetometer.
 */
template <typename ValueT_ = double,
          typename TimestampT_ = std::int64_t,
          typename Ratio_ = std::nano>
class AxesEvent
{
public:
  /**
   * @brief Type of sensor values.
   */
  typedef ValueT_ ValueT;
  /**
   * @brief Type of timestamp values.
   */
  typedef TimestampT_ TimestampT;
  /**
   * @brief Ratio timestamp to seconds.
   */
  typedef Ratio_ Ratio;

  /**
   * @brief Event type. See enum EventType.
   */
  EventType type;
  /**
   * @brief Event time.
   */
  TimestampT timestamp;
  /**
   * @brief Reading values.
   */
  ArrayMatrix::Vector<ValueT,3> value;

  /*
   * MEMBER FUNCTIONS
   */

  std::string toString() const;

  /*
   * STATIC MEMBER FUNCTIONS
   */

  template <typename StringT>
  static AxesEvent<ValueT_, TimestampT_, Ratio_>
    fromString(const StringT &);

  template <typename AnotherEventType>
  static AxesEvent<ValueT_, TimestampT_, Ratio_>
    from(const AnotherEventType &);
};

/************************************************/
/***************  IMPLEMENTATION  ***************/
/************************************************/

/**
 * @brief Serializes an event into the std::string.
 */
template <typename ValueT_,
          typename TimestampT_,
          typename Ratio_>
std::string
  AxesEvent<ValueT_, TimestampT_, Ratio_>::toString() const
{
  std::stringstream ss;
  ss << static_cast<int>(this->type) << '\t'
     << this->timestamp << '\t';
  for (auto &v : this->value) {
    ss.width(20);
    ss << std::left << v;
  }
  return ss.str();
}

/*
 * STATIC MEMBER FUNCTIONS
 */

/**
 * @brief Creates an event from another object with members of the same names
 *        as used in this class, i.e., type, value, timestamp.
 *        Those members are statically casted to the types of this class.
 */
template <typename ValueT_,
          typename TimestampT_,
          typename Ratio_>
template <typename AnotherEventType>
AxesEvent<ValueT_, TimestampT_, Ratio_>
  AxesEvent<ValueT_, TimestampT_, Ratio_>::from(
    const AnotherEventType &rhs)
{
  AxesEvent<ValueT_, TimestampT_, Ratio_> event;
  event.type = static_cast<EventType>(rhs.type);
  event.timestamp = static_cast<TimestampT_>(rhs.timestamp);
  for (size_t i = 0; i < 3; ++i)
    event.value[i] = static_cast<ValueT_>(rhs.value[i]);
  return event;
}

/**
 * @brief Unserizalizes an event from a string.
 */
template <typename ValueT_,
          typename TimestampT_,
          typename Ratio_>
template <typename StringT>
AxesEvent<ValueT_, TimestampT_, Ratio_>
  AxesEvent<ValueT_, TimestampT_, Ratio_>::fromString(
    const StringT &string)
{
  AxesEvent<ValueT_, TimestampT_, Ratio_> event;
  std::stringstream ss{string};
  int int_type;
  ss >> int_type;
  event.type = static_cast<EventType>(int_type);
  ss >> event.timestamp;
  for (auto &v : event.value)
    ss >> v;
  return event;
}

/*
 * GLOBAL FUNCTIONS
 */

/**
 * @brief Comparator, that compares two sensor events according
 *        to their timestamps.
 */
template <typename SensorEvent>
bool compareTimestamps(
  const SensorEvent &lhs,
  const SensorEvent &rhs)
{
  return lhs.timestamp < rhs.timestamp;
}

/**
 * @brief Converts timestamp value to seconds according to the Ratio.
 */
template <typename SensorEvent>
typename SensorEvent::ValueT
  timestampToSeconds(const typename SensorEvent::TimestampT &timestamp)
{
  typedef typename SensorEvent::ValueT ValueT;
  typedef typename SensorEvent::Ratio Ratio;
  return static_cast<ValueT>(Ratio::num) /
         static_cast<ValueT>(Ratio::den) *
         static_cast<ValueT>(timestamp);
}

} // namespace TrackMe
#endif
