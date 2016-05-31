#ifndef __PREMULTIPLYBY_HPP__
#define __PREMULTIPLYBY_HPP__

#include "../RecursivePipeline.hpp"
#include "../ArrayMatrix.hpp"

namespace TrackMe {

/**
 * @brief Pre-multiplies the received event by an external event.
 */
template <typename SensorEvent, typename ExternalEvent = SensorEvent>
class PreMultiplyBy
: public PipelineElement<SensorEvent, SensorEvent>
{
  public:
    /**
     * @brief Sets the external event.
     */
    PreMultiplyBy(const ExternalEvent *external) :
      external_{external}
    {
    }

    void receive(SensorEvent event)
    {
      using ArrayMatrix::operator*;
      event.value = external_->value * event.value;

      this->send(event);
    }

  private:
    const ExternalEvent *external_{};
};

} // namespace TrackMe
#endif
