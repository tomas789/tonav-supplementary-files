#ifndef __DELTAINTEGRATIONFILTER_HPP__
#define __DELTAINTEGRATIONFILTER_HPP__

#include "../RecursivePipeline.hpp"
#include "../EventType.hpp"
#include "../AxesEvent.hpp"

#include "../ArrayMatrix.hpp"

namespace TrackMe {

/**
 * @brief Numerical integration (linear approximation) that calculates
 *        only how much the new event would add to the integral.
 *        It is a normal integration in combination with addition.
 */
template <typename SensorEvent>
class DeltaIntegrate
: public PipelineElement<SensorEvent, SensorEvent>
{
  public:
    /**
     * @brief Receives an event and returns area
     *        under a line connecting previous event and this event.
     * @param event Received event.
     */
    void receive(SensorEvent event)
    {
      // The first event will be the initial one
      if (last_event_.type == EventType::UNKNOWN) {
        last_event_ = event;
        return;
      }

      // Integration routine
      // Calculate the time difference in seconds
      auto timestamp_dif = event.timestamp - last_event_.timestamp;
      auto time_dif = timestampToSeconds<SensorEvent>(timestamp_dif);
      // Calculate delta - timestamp is taken from the current event
      using ArrayMatrix::operator+;
      using ArrayMatrix::operator*;
      auto delta_value = (event.value + last_event_.value) * (time_dif / 2.);

      last_event_ = event;
      event.value = delta_value;
      this->send(event);
    }

  private:
    SensorEvent last_event_{};
};

} // namespace TrackMe
#endif
