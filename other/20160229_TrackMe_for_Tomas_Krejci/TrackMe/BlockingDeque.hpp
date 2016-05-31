#ifndef BLOCKINGDEQUE_HPP__
#define BLOCKINGDEQUE_HPP__

#include <deque>
#include <utility>
#include <mutex>
#include <condition_variable>
#include <exception>
#include <chrono>
#include <limits>

/**
 * @brief Exception type thrown on the timeout of the NBDeque.
 */
class BlockingDequeTimeout : public std::exception {};

/**
 * @brief Simple thread-safe blocking deque class.
 *
 * All operations are thread-safe, but iterators are not.
 * To use them in multiple threads, lock the object using the mutex().
 */
template <typename T, typename Allocator = std::allocator<T>>
class BlockingDeque
{
  public:
    typedef Allocator allocator_type;
    typedef typename Allocator::value_type value_type;
    typedef typename Allocator::reference reference;
    typedef typename Allocator::const_reference const_reference;
    typedef typename Allocator::difference_type difference_type;
    typedef typename Allocator::size_type size_type;

    BlockingDeque();
    BlockingDeque(const BlockingDeque &rhs);
    BlockingDeque(BlockingDeque &&rhs);
    BlockingDeque& operator=(const BlockingDeque &rhs);
    BlockingDeque& operator=(BlockingDeque &&rhs);

    bool empty() const;
    void clear();
    size_type size() const;
    size_type max_size() const;

    typedef std::deque<T,Allocator> deque_type;
    typedef typename deque_type::iterator iterator; 
    typedef typename deque_type::const_iterator const_iterator;

    iterator begin()
      {return unsafe_deque_.begin();}
    const_iterator begin() const
      {return unsafe_deque_.begin();}
    const_iterator cbegin() const
      {return unsafe_deque_.cbegin();}
    iterator end() 
      {return unsafe_deque_.end();}
    const_iterator end() const
      {return unsafe_deque_.end();}
    const_iterator cend() const
      {return unsafe_deque_.cend();}

    typedef typename deque_type::reverse_iterator reverse_iterator; 
    typedef typename deque_type::const_reverse_iterator const_reverse_iterator;
    reverse_iterator rbegin()
      {return unsafe_deque_.rbegin();}
    const_reverse_iterator rbegin() const
      {return unsafe_deque_.rbegin();}
    const_reverse_iterator crbegin() const
      {return unsafe_deque_.crbegin();}
    reverse_iterator rend() 
      {return unsafe_deque_.rend();}
    const_reverse_iterator rend() const
      {return unsafe_deque_.rend();}
    const_reverse_iterator crend() const
      {return unsafe_deque_.crend();}

    template <typename Rep, typename Period>
    void set_timeout(const std::chrono::duration<Rep,Period> &timeout);
    template <typename... ArgsTypes>
    void push_back(ArgsTypes &&... args);
    template <typename... ArgsTypes>
    void push_front(ArgsTypes &&... args);
    T pop_get_back();
    T get_back() const;
    T pop_get_front();
    T get_front() const;

    deque_type & deque();
    const deque_type & deque() const;
    std::mutex & mutex() const;
    void wait_for_push(std::unique_lock<std::mutex> &) const;

  private:
    std::chrono::milliseconds       timeout_;
    std::deque<T,Allocator>         unsafe_deque_;
    // mutable mutex because of const functions
    mutable std::mutex              deque_mutex_;
    mutable std::condition_variable inserted_cond_;
};

/**
 * @brief NBDeque constructor.
 */
template <typename T, typename Allocator>
BlockingDeque<T, Allocator>::BlockingDeque()
: timeout_{std::chrono::milliseconds::max()}
{
}

/**
 * @brief NBDeque copy constructor. Ignores mutex and condition variable.
 */
template <typename T, typename Allocator>
BlockingDeque<T, Allocator>::BlockingDeque(const BlockingDeque &rhs)
{
  *this = rhs;
}

/**
 * @brief NBDeque move constructor. Ignores mutex and condition variable.
 */
template <typename T, typename Allocator>
BlockingDeque<T, Allocator>::BlockingDeque(BlockingDeque &&rhs)
{
  *this = std::move(rhs);
}

/**
 * @brief NBDeque copy assignment. Ignores mutex and condition variable.
 */
template <typename T, typename Allocator>
BlockingDeque<T, Allocator> & BlockingDeque<T, Allocator>::operator=(const BlockingDeque &rhs)
{
  std::lock_guard<std::mutex> lock(rhs.mutex());
  timeout_      = rhs.timeout_;
  unsafe_deque_ = rhs.unsafe_deque_;
  return *this;
}

/**
 * @brief NBDeque move assignment. Ignores mutex and condition variable.
 */
template <typename T, typename Allocator>
BlockingDeque<T, Allocator> & BlockingDeque<T, Allocator>::operator=(BlockingDeque &&rhs)
{
  std::lock_guard<std::mutex> lock(rhs.mutex());
  timeout_      = std::move(rhs.timeout_);
  unsafe_deque_ = std::move(rhs.unsafe_deque_);
  return *this;
}

/**
 * @brief Sets a new timeout value.
 * @param timeout Timeout for waiting for a new element (pop_get,
 *                etc...). For infinity, use std::chrono::duration::max().
 *
 * Also wakes up all sleeping threads.
 */
template <typename T, typename Allocator>
template <typename Rep, typename Period>
void BlockingDeque<T, Allocator>::set_timeout(
  const std::chrono::duration<Rep,Period> &timeout)
{
  std::lock_guard<std::mutex> lock(deque_mutex_);
  timeout_ = std::chrono::duration_cast<std::chrono::milliseconds>(timeout);
  inserted_cond_.notify_all();
}

/**
 * @brief Inserts an element to the back of the deque.
 */
template <typename T, typename Allocator>
template <typename... ArgsTypes>
void BlockingDeque<T, Allocator>::push_back(ArgsTypes &&... args)
{
  std::lock_guard<std::mutex> lock(deque_mutex_);
  unsafe_deque_.push_back(std::forward<ArgsTypes>(args)...);
  inserted_cond_.notify_all();
}

/**
 * @brief Inserts an element to the front of the deque.
 */
template <typename T, typename Allocator>
template <typename... ArgsTypes>
void BlockingDeque<T, Allocator>::push_front(ArgsTypes &&... args)
{
  std::lock_guard<std::mutex> lock(deque_mutex_);
  unsafe_deque_.push_front(std::forward<ArgsTypes>(args)...);
  inserted_cond_.notify_all();
}

/**
 * @brief Returns the back element and removes it from the
 *        deque. If the deque is empty, blocks the call until there
 *        is something inserted.
 */
template <typename T, typename Allocator>
T BlockingDeque<T, Allocator>::pop_get_back()
{
  std::unique_lock<std::mutex> lock(deque_mutex_);
  while (unsafe_deque_.empty())
    wait_for_push(lock);
  T back = unsafe_deque_.back();
  unsafe_deque_.pop_back();
  return back;
}

/**
 * @brief Returns the copy of the back element. If the deque
 *        is empty, blocks the call until there is
 *        something inserted.
 */
template <typename T, typename Allocator>
T BlockingDeque<T, Allocator>::get_back() const
{
  std::unique_lock<std::mutex> lock(deque_mutex_);
  while (unsafe_deque_.empty())
    wait_for_push(lock);
  return unsafe_deque_.back();
}

/**
 * @brief Returns a copy of the front element. If the deque
 *        is empty, blocks the call until there is
 *        something inserted.
 */
template <typename T, typename Allocator>
T BlockingDeque<T, Allocator>::get_front() const
{
  std::unique_lock<std::mutex> lock(deque_mutex_);
  while (unsafe_deque_.empty())
    wait_for_push(lock);
  return unsafe_deque_.front();
}

/**
 * @brief Returns the front element and removes it from the
 *        deque. If the deque is empty, blocks the call until there
 *        is something inserted.
 */
template <typename T, typename Allocator>
T BlockingDeque<T, Allocator>::pop_get_front()
{
  std::unique_lock<std::mutex> lock(deque_mutex_);
  while (unsafe_deque_.empty())
    wait_for_push(lock);
  T front = unsafe_deque_.front();
  unsafe_deque_.pop_front();
  return front;
}

/**
 * @brief Removes all elements from the deque.
 */
template <typename T, typename Allocator>
void BlockingDeque<T, Allocator>::clear()
{
  std::lock_guard<std::mutex> lock(deque_mutex_);
  unsafe_deque_.clear();
}

/**
 * @brief Returns true if the deque is empty.
 */
template <typename T, typename Allocator>
bool BlockingDeque<T, Allocator>::empty() const
{
  std::lock_guard<std::mutex> lock(deque_mutex_);
  return unsafe_deque_.empty();
}

/**
 * @brief Returns a number of elements in the deque.
 */
template <typename T, typename Allocator>
typename BlockingDeque<T, Allocator>::size_type BlockingDeque<T, Allocator>::size() const
{
  std::lock_guard<std::mutex> lock(deque_mutex_);
  return unsafe_deque_.size();
}

/**
 * @brief Returns a maximum number of elements in the deque.
 */
template <typename T, typename Allocator>
typename BlockingDeque<T, Allocator>::size_type BlockingDeque<T, Allocator>::max_size() const
{
  std::lock_guard<std::mutex> lock(deque_mutex_);
  return unsafe_deque_.max_size();
}

/**
 * @brief Returns the underlaying unsafe deque.
 */
template <typename T, typename Allocator>
typename BlockingDeque<T, Allocator>::deque_type &
  BlockingDeque<T, Allocator>::deque()
{
  return unsafe_deque_;
}

/**
 * @brief Returns the underlaying unsafe deque with const quantifier.
 */
template <typename T, typename Allocator>
const typename BlockingDeque<T, Allocator>::deque_type &
  BlockingDeque<T, Allocator>::deque() const
{
  return unsafe_deque_;
}

/**
 * @brief Returns the mutex of this object.
 */
template <typename T, typename Allocator>
std::mutex & BlockingDeque<T, Allocator>::mutex() const
{
  return deque_mutex_;
}

/**
 * @brief Waits for an insertion of a new element and obeys the timeout
 *       settings. Throws a NBDequeTimeout exception in case of timeout.
 *       Spurious wakeups can happen.
 * @param Lock to be locked in case of waking up.
 */
template <typename T, typename Allocator>
void BlockingDeque<T, Allocator>::wait_for_push(
    std::unique_lock<std::mutex> &lock) const
{
  if (timeout_ == std::chrono::milliseconds::max())
    inserted_cond_.wait(lock);
  else if (inserted_cond_.wait_for(lock, timeout_) == std::cv_status::timeout)
    throw BlockingDequeTimeout();
}
#endif
