#ifndef RECURSIVEPIPELINE_HPP__
#define RECURSIVEPIPELINE_HPP__

#include <functional>
#include <tuple>
#include <type_traits>

/**
 * @brief An optional base class for used to represent a pipeline element.
 *        It implements the basic functionality and the only thing for a
 *        user to do is to implement a `receive()` function.
 * @tparam Input Input type (type of the `receive()` argument).
 * @tparam Output Output type (type of the `send()` argument).
 */
template <typename Input, typename Output>
class PipelineElement
{
  public:
    /**
     * @brief Type that is received by the `receive()`.
     */
    typedef Input  input_type;
    /**
     * @brief Type that is sent by the `send()`.
     */
    typedef Output output_type;

    /**
     * @brief Processing function - has to be overridden.
     * @param lhs Input packet.
     *
     * Process a packet and when you want to pass the packet
     * to a next pipeline element, call `this->send(packet)`.
     */
    void receive(const Input &lhs);

    /**
     * @brief Send a packet to the next pipeline element.
     * @param rhs Output packet.
     */
    void send(const Output &rhs)
    {
      if (destination_)
          destination_(rhs);
    }

    /**
     * @brief Sets the receiver of the `send()` function.
     * @param destination Destination function.
     */
    void set_destination(std::function<void(const Output &)> destination)
    {
      destination_ = destination;
    }

  private:
    std::function<void(Output)> destination_;
};

/**
 * @brief Partial specialization of a pipeline tail (the last pipeline
 *        element). The only difference is, that there is no `send()` function.
 */
template <typename Input>
class PipelineElement<Input, void>
{
  public:
    /**
     * @brief Type that is received by the `receive()`.
     */
    typedef Input input_type;
    /**
     * @brief Output type for a tail element is void.
     */
    typedef void  output_type;

    /**
     * @brief Processing function - has to be overridden.
     * @param lhs Input packet.
     *
     * This is the last pipeline element and the `send()` function
     * is not available.
     */
    void receive(const Input &lhs);
};

/**
 * CONNECT FUNCTORS
 */

/**
 * @brief Recursion bottom for a variadic template function.
 *        When there is only one pipeline element.
 */
template <typename LastElem>
void connect_pipeline(const LastElem &)
{
}

/**
 * @brief Recursively connect pipeline elements together.
 */
template <typename FirstElem, typename SecondElem, typename... Rest> 
void connect_pipeline(FirstElem &first_elem,
                      SecondElem &second_elem,
                      Rest&... rest)
{
  // Get type of the second element
  using input_type =
    typename std::remove_reference<SecondElem>::type::input_type;

  // Connect two consecutive elements
  auto dest = [&](const input_type &packet)
              { second_elem.receive(packet); };
  first_elem.set_destination(dest);

  // Recursion
  connect_pipeline(second_elem, rest...);
}

/*
 * CONNECT FUNCTORS IN A TUPLE
 */

/**
 * @brief Recursively connect pipeline elements in a tuple together.
 */
template <typename Tuple, std::size_t pos>
struct PipelineConnector
{
  static void connect_previous(Tuple &elements)
  {
    // Get the input type of the current element
    using input_type =
      typename std::remove_reference<
                 typename std::tuple_element<pos, Tuple>::type
               >::type::input_type;

    // Set the previous element destination to the current element receiver
    auto dest = [&](const input_type &packet)
                { std::get<pos>(elements).receive(packet); };
    std::get<pos - 1>(elements).set_destination(dest);

    // Recursion
    PipelineConnector<Tuple, pos - 1>::connect_previous(elements);
  }
};

/**
 * @brief Recursion bottom for the variadic template class.
 *        When there is only one pipeline element.
 */
template <typename Tuple>
struct PipelineConnector<Tuple, 0>
{
  static void connect_previous(const Tuple &)
  {
  }
};

/**
 * @brief Recursively connect the pipeline elements in a tuple together.
 */
template <typename... T>
void connect_pipeline(std::tuple<T...> &elements)
{
  using Tuple = typename std::remove_reference<decltype(elements)>::type;
  PipelineConnector<Tuple, std::tuple_size<Tuple>::value - 1>
    ::connect_previous(elements);
}

#endif
