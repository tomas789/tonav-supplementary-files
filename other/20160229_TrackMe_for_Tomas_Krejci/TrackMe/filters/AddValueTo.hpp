#ifndef __ADDVALUETO_HPP__
#define __ADDVALUETO_HPP__

#include "../RecursivePipeline.hpp"
#include "../ArrayMatrix.hpp"

namespace TrackMe {

/**
 * @brief Adds the value of the received event to the value of an external event
 *        and sends the result further.
 */
template <typename SensorEvent, typename ExternalEvent = SensorEvent>
class AddValueTo
: public PipelineElement<SensorEvent, SensorEvent>
{
  public:
    /**
     * @brief Sets the external event.
     */
    AddValueTo(ExternalEvent *external) :
      external_{external}
    {
    }

    void receive(const SensorEvent &event)
    {
      external_->type = event.type;
      external_->timestamp = event.timestamp;

      using ArrayMatrix::operator+;
      external_->value = external_->value + event.value;

      this->send(*external_);
    }

  private:
    ExternalEvent *external_{};
};

} // namespace TrackMe
#endif
