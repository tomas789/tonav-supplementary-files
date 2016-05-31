#ifndef __CHANGETYPE_HPP__
#define __CHANGETYPE_HPP__

#include "../RecursivePipeline.hpp"
#include "../EventType.hpp"

namespace TrackMe {

/**
 * @brief A filter that changes the type of a sensor event.
 */
template<typename SensorEvent>
class ChangeType : public PipelineElement<SensorEvent, SensorEvent>
{
  public:
    /**
     * Sets the new type for all passing events.
     */
    ChangeType(EventType type) :
      type_{type}
    {
    }

    /**
     * @brief Receives an event, changes the type and sends it further.
     * @param event Received event.
     */
    void receive(SensorEvent event)
    {
      event.type = type_;
      this->send(event);
    }

  private:
    EventType type_;
};

} // namespace TrackMe
#endif
