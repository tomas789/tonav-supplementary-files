#ifndef EVENTTYPE_HPP__
#define EVENTTYPE_HPP__

#include <cstdint>

namespace TrackMe {

/**
 * @brief Represents different types of events.
 *        1-14 are sensor types, 16-128 are system events.
 */
enum class EventType : std::int8_t
{
  UNKNOWN              = 0x0,

  SENSOR               = 0x0F, // below are sensor types
                    // = 00001111
  ACCELEROMETER        = 0x01,
  GYROSCOPE            = 0x02,
  MAGNETIC_FIELD       = 0x03,
  ORIENTATION          = 0x04,
  VELOCITY             = 0x05,
  DISPLACEMENT         = 0x06,
  LINEAR_ACCELERATION  = 0x07,

  SYSTEM               = 0x70, // below are system events
                    // = 01110000
  START                = 0x10,
  STOP                 = 0x20
};

constexpr bool isSystemEvent(EventType type)
{
  return (std::int8_t)type & (std::int8_t)EventType::SYSTEM;
}

} // namespace TrackMe
#endif
