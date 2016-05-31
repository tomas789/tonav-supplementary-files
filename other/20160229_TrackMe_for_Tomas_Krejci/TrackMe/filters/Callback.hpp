#ifndef __CALLBACK_HPP__
#define __CALLBACK_HPP__

#include "../RecursivePipeline.hpp"

#include <functional>

namespace TrackMe {

/**
* @brief A filter that calls a function with every received event.
*/
template<typename SensorEvent>
class Callback : public PipelineElement<SensorEvent, SensorEvent>
{
  typedef std::function<void (const SensorEvent &)> CallbackT;
  typedef std::function<void ()> VoidCallbackT;

  public:
    /**
     * @brief Sets the callback function.
     * @param callback The callback function.
     */
    Callback(CallbackT callback = CallbackT{}) :
      filter_{callback}
    {}

    /**
     * @brief Sets a callback function with void return type.
     * @param callback The callback function.
     */
    Callback(VoidCallbackT callback) :
      filter_{[=](const SensorEvent &){callback();}}
    {}

    /**
     * @brief Calls the callback.
     * @param event Received event.
     */
    void receive(const SensorEvent &event)
    {
      if (filter_)
        filter_(event);
      this->send(event);
    }

  private:
    CallbackT filter_;
};

} // namespace TrackMe
#endif
