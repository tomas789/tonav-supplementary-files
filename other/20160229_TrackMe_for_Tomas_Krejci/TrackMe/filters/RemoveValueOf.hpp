#ifndef __REMOVEVALUEOF_HPP__
#define __REMOVEVALUEOF_HPP__

#include "../RecursivePipeline.hpp"
#include "../ArrayMatrix.hpp"

namespace TrackMe {

/**
 * @brief Removes a value of an external event from the value of the received
 *        event and sends the result further.
 */
template <typename SensorEvent, typename ExternalEvent = SensorEvent>
class RemoveValueOf
: public PipelineElement<SensorEvent, SensorEvent>
{
  public:
    /**
     * @brief Sets the external event.
     */
    RemoveValueOf(const ExternalEvent *external) :
      external_{external}
    {
    }

    void receive(SensorEvent event)
    {
      using ArrayMatrix::operator-;
      event.value = event.value - external_->value;

      this->send(event);
    }

  private:
    const ExternalEvent *external_{};
};

} // namespace TrackMe
#endif
