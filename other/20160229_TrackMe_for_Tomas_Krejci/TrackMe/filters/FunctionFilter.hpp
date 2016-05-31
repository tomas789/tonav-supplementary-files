#ifndef __FUNCTIONFILTER_HPP__
#define __FUNCTIONFILTER_HPP__

#include "../RecursivePipeline.hpp"

#include <functional>

namespace TrackMe {

/**
* @brief A filter that calls a function and according to it's boolean
*        return value it does or does not pass the received the event further.
*/
template<typename SensorEvent>
class FunctionFilter : public PipelineElement<SensorEvent, SensorEvent>
{
  typedef std::function<bool (const SensorEvent &)> FunctionT;

  public:
    /**
     * @brief Sets the external function.
     */
    FunctionFilter(FunctionT filter = FunctionT{}) :
      filter_{filter}
    {}

    /**
     * @brief Calls the function and if it returns true or is not set,
     *        sends the event further.
     */
    void receive(const SensorEvent &event)
    {
      if (!filter_ || filter_(event))
        this->send(event);
    }

  private:
    FunctionT filter_;
};

} // namespace TrackMe
#endif
