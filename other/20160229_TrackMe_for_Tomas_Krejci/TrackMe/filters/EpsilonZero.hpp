#ifndef __EPSILONZERO_HPP__
#define __EPSILONZERO_HPP__

#include "../RecursivePipeline.hpp"
#include "../ArrayMatrix.hpp"

namespace TrackMe {

/**
 * @brief If the event magnitude is less than an epsilon, the event is zeroed.
 */
template <typename SensorEvent>
class EpsilonZero
: public PipelineElement<SensorEvent, SensorEvent>
{
  public:
    /**
     * @brief Sets the epsilon value.
     */
    EpsilonZero(const typename SensorEvent::ValueT epsilon) :
      epsilon_{epsilon}
    {
    }

    /**
     * @brief Receives an axis event and if its magnitude is less than an
     *        epsilon, assigns it the value from the default constructed
     *        SensorEvent.
     * @param event Received event.
     */
    void receive(SensorEvent event)
    {
      auto mag = ArrayMatrix::norm(event.value);

      if (mag < epsilon_)
        event.value = SensorEvent{}.value;

      this->send(event);
    }

  private:
    typename SensorEvent::ValueT epsilon_{};
};

} // namespace TrackMe
#endif
