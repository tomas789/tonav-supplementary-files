#ifndef ORIENTATIONEVENT_HPP__
#define ORIENTATIONEVENT_HPP__

#include "EventType.hpp"
#include "ArrayMatrix.hpp"

#include <ratio>
#include <cstdint>

namespace TrackMe {

/**
 * @brief Orientation event represents an event with matrix value.
 */
template <typename ValueT_ = double,
          typename TimestampT_ = std::int64_t,
          typename Ratio_ = std::nano>
class OrientationEvent
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
   * @brief Reading value.
   */
  ArrayMatrix::Matrix<ValueT_,3,3> value;

  /**
   * @brief Default constructor initializes the value to an identity matrix.
   */
  OrientationEvent() :
    type{},
    timestamp{}
  {
    // Neutral orientation is an identity matrix
    this->value = ArrayMatrix::identity_matrix<ValueT, 3>();
  }
};

} // namespace TrackMe
#endif
