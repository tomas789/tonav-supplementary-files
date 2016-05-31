#ifndef __STATISTICS_HPP__
#define __STATISTICS_HPP__

#include "../RecursivePipeline.hpp"
#include "../ArrayMatrix.hpp"

#include <deque>

namespace TrackMe {

/**
 * @brief This structure keeps various statistics of a particular sensor.
 */
template <typename ValueT>
struct SensorStats {
  std::size_t size;
  ValueT rate;
  std::array<ValueT, 3> mean;
  ValueT mean_magnitude;
  ValueT variance;
  ValueT squares;
};

/**
 * @brief Updates various statistics about the events which pass through.
 *        Only works for an AxisEvent (i.e. event.value
 *        is an std::array of SensorEvent::ValueT).
 */
template <typename SensorEvent>
class Statistics
: public PipelineElement<SensorEvent, SensorEvent>
{
  public:
    typedef typename SensorEvent::ValueT ValueT;

    Statistics(SensorStats<ValueT> *external,
               std::size_t stats_size) :
      external_{external},
      stats_size_{stats_size}
    {
      *external_ = SensorStats<ValueT>{};
    }

    /**
     * @brief Updates the external statistics.
     * @param event Received event.
     */
    void receive(const SensorEvent &event)
    {
      using namespace ArrayMatrix;

      // Knuth - The Art of Computer Programming, volume 2
      // On-line variance
      if (external_->size >= stats_size_) {
        auto &remove = history_.front();
        external_->size--;
        auto delta = remove.value - external_->mean;
        external_->mean = external_->mean - delta /
                          static_cast<ValueT>(external_->size);
        M2_ = M2_ - norm(delta) *
              (norm(remove.value - external_->mean));
        external_->squares = external_->squares -
                             norm(remove.value) * norm(remove.value);
        history_.pop_front();
      }

      external_->size++;
      auto delta = event.value - external_->mean;
      external_->mean = external_->mean + delta /
                        static_cast<ValueT>(external_->size);
      external_->mean_magnitude = norm(external_->mean);
      M2_ = M2_ + norm(delta) *
            (norm(event.value - external_->mean));
      external_->squares = external_->squares +
                           norm(event.value) * norm(event.value);
      history_.push_back(event);

      if (external_->size > 1) {
        external_->variance = M2_ / (external_->size - 1);
        external_->rate = external_->size / (
            timestampToSeconds<SensorEvent>(history_.back().timestamp) -
            timestampToSeconds<SensorEvent>(history_.front().timestamp));
      }

      this->send(event);
    }

  private:
    SensorStats<ValueT> *external_{nullptr};
    std::size_t stats_size_{};
    ValueT M2_{};
    std::deque<SensorEvent> history_{};
};

} // namespace TrackMe
#endif
