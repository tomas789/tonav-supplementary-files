#ifndef __DROP_HPP__
#define __DROP_HPP__

#include "../RecursivePipeline.hpp"

namespace TrackMe {

/**
 * @brief Drops the first N events.
 */
template<typename SensorEvent>
class Drop : public PipelineElement<SensorEvent, SensorEvent>
{
  public:
    /**
     * @brief Sets the number of samples (N) to be dropped.
     */
    Drop(std::size_t samples) :
      samples_{samples}
    {
    }

    /**
     * @brief Receives an event and drops it if it is in the first N events.
     * @param event Received event.
     */
    void receive(const SensorEvent &event)
    {
      if (samples_)
        --samples_;
      else
        this->send(event);
    }

  private:
    std::size_t samples_;
};

} // namespace TrackMe
#endif
