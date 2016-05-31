#ifndef __POSTMULTIPLYTHE_HPP__
#define __POSTMULTIPLYTHE_HPP__

#include "../RecursivePipeline.hpp"
#include "../ArrayMatrix.hpp"

namespace TrackMe {

/**
 * @brief Post-multiplies an external event by the received event. Also
 *        copies type and timestamp.
 */
template <typename SensorEvent, typename ExternalEvent = SensorEvent>
class PostMultiplyThe
: public PipelineElement<SensorEvent, SensorEvent>
{
  public:
    /**
     * @brief Sets the external event.
     */
    PostMultiplyThe(ExternalEvent *external) :
      external_{external}
    {
    }

    void receive(SensorEvent event)
    {
      using ArrayMatrix::operator*;
      external_->type = event.type;
      external_->timestamp = event.timestamp;
      external_->value = external_->value * event.value;

      this->send(*external_);
    }

  private:
    ExternalEvent *external_{};
};

} // namespace TrackMe
#endif
