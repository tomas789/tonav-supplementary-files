#ifndef __TOROTATIONMATRIX_HPP__
#define __TOROTATIONMATRIX_HPP__

#include "../RecursivePipeline.hpp"
#include "../ArrayMatrix.hpp"

namespace TrackMe {

/**
 * @brief Creates a rotation matrix out of three simultaneous rotations
 * around coordinate axes.
 */
template <typename AxesEvent, typename OrientationEvent>
class ToRotationMatrix
: public PipelineElement<AxesEvent, OrientationEvent>
{
  public:
    /**
     * @brief Receives an event with value <u,v,w> and creates a rotation
     *        matrix such that when it is post-multiplied by a vector, it
     *        rotates the vector by angles u, v and w around the X,Y and Z axis
     *        respectively.
     * @param event Received event.
     */
    void receive(const AxesEvent &event)
    {
      auto angle = ArrayMatrix::norm(event.value);

      OrientationEvent matrix_event{};
      matrix_event.type = event.type;
      matrix_event.timestamp = event.timestamp;
      matrix_event.value = ArrayMatrix::rotation_matrix(event.value, angle);

      this->send(matrix_event);
    }
};

} // namespace TrackMe
#endif
