#ifndef TRACKME_HPP_
#define TRACKME_HPP_

#include "filters/AddValueTo.hpp"
#include "filters/Callback.hpp"
#include "filters/ChangeType.hpp"
#include "filters/DeltaIntegrate.hpp"
#include "filters/Drop.hpp"
#include "filters/EpsilonZero.hpp"
#include "filters/PostMultiplyThe.hpp"
#include "filters/PreMultiplyBy.hpp"
#include "filters/RemoveValueOf.hpp"
#include "filters/FunctionFilter.hpp"
#include "filters/ToRotationMatrix.hpp"
#include "filters/Statistics.hpp"

#include "EventType.hpp"
#include "OrientationEvent.hpp"
#include "AxesEvent.hpp"

#include "Configuration.hpp"
#include "RecursivePipeline.hpp"
#include "BlockingDeque.hpp"
#include "ArrayMatrix.hpp"

#include <mutex>
#include <array>
#include <thread>
#include <functional>
#include <limits>
#include <cmath>

namespace TrackMe {

/**
 * @brief The main processing class. It is the public interface
 *        for the entire process. 
 *
 *        New events are queued for processing by the receiveEvent().
 *
 *        All the interesting stuff happens in the run() function.
 *        After the START event is received, the run() is started in
 *        a separate thread. After the STOP event is received, the
 *        thread is joined. The threads are synchronized by a
 *        blocking queue.
 *
 *        Communication with the class can be performed by setting
 *        callbacks or by manually asking the class for the current
 *        position and orientation of the device.
 */
template <typename RawEventT>
class Tracker
{
  public:
    typedef typename RawEventT::ValueT ValueT;
    typedef typename RawEventT::TimestampT TimestampT;
    typedef typename RawEventT::Ratio Ratio;
    typedef OrientationEvent<ValueT, TimestampT, Ratio>
      OrientationEventT;

    // Destructor has to stop the thread
    ~Tracker();

    void receiveEvent(const RawEventT &event);

    // Gyroscope specific
    void setRawGyroscopeCallback(
      std::function<void (const RawEventT &)>);
    void setFilteredGyroscopeCallback(
      std::function<void (const RawEventT &)>);
    void setOrientationCallback(
      std::function<void (const OrientationEventT &)>);

    // Accelerometer specific
    void setRawAccelerometerCallback(
      std::function<void (const RawEventT &)>);
    void setFilteredAccelerometerCallback(
      std::function<void (const RawEventT &)>);
    void setVelocityCallback(
      std::function<void (const RawEventT &)>);
    void setDisplacementCallback(
      std::function<void (const RawEventT &)>);

    // Magnetic field specific
    void setRawMagneticFieldCallback(
      std::function<void (const RawEventT &)>);
    void setFilteredMagneticFieldCallback(
      std::function<void (const RawEventT &)>);

    OrientationEventT orientation() const;
    RawEventT velocity() const;
    RawEventT displacement() const;

    SensorStats<typename RawEventT::ValueT> accelerometerStats() const;
    SensorStats<typename RawEventT::ValueT> gyroscopeStats() const;
    SensorStats<typename RawEventT::ValueT> magneticFieldStats() const;

    void start();
    void stop();
    bool stopped() const;
    bool calibrated() const;

    Configuration<std::string> & config();

  private:
    std::thread thread_;
    BlockingDeque<RawEventT> events_;
    mutable std::mutex mutex_;

    std::function<void(RawEventT)> raw_gyroscope_callback_;
    std::function<void(RawEventT)> filtered_gyroscope_callback_;
    std::function<void(OrientationEventT)> orientation_callback_;

    std::function<void(RawEventT)> raw_accelerometer_callback_;
    std::function<void(RawEventT)> filtered_accelerometer_callback_;
    std::function<void(RawEventT)> velocity_callback_;
    std::function<void(RawEventT)> displacement_callback_;

    std::function<void(RawEventT)> raw_linear_accelerometer_callback_;
    std::function<void(RawEventT)> filtered_linear_accelerometer_callback_;
    std::function<void(RawEventT)> linear_velocity_callback_;
    std::function<void(RawEventT)> linear_displacement_callback_;

    std::function<void(RawEventT)> raw_magnetic_field_callback_;
    std::function<void(RawEventT)> filtered_magnetic_field_callback_;

    void setOrientation(const OrientationEventT &orientation);
    void setVelocity(const RawEventT &velocity);
    void setDisplacement(const RawEventT &displacement);

    OrientationEventT orientation_{};
    RawEventT velocity_{};
    RawEventT displacement_{};

    RawEventT gravity_{};
    RawEventT magnetic_field_init_{};

    SensorStats<ValueT> accelerometer_stats_{};
    SensorStats<ValueT> gyroscope_stats_{};
    SensorStats<ValueT> magnetic_field_stats_{};

    bool calibrated_{};

    Configuration<std::string> conf_;

    void run();
};

/**
 * @brief Stops the thread using the stop() method and destructs the object.
 */
template <typename RawEventT>
Tracker<RawEventT>::~Tracker()
{
  stop();
}

/**
 * @brief Puts the STOP event in the queue and joins the processing thread.
 *
 *        After the STOP event is received, all future events are ignored.
 *        Calling this function has the same effect as 
 *        receiving the STOP event.
 */
template <typename RawEventT>
void Tracker<RawEventT>::stop()
{
  if (!stopped()) {
    RawEventT stop_event;
    stop_event.type = EventType::STOP;

    events_.push_back(stop_event);

    thread_.join();
  }
}

/**
 * @brief Starts/restarts the processor.
 *
 * Stops, reinitializes and starts again the processing thread. This has
 * the same effect as receiving the START event.
 */
template <typename RawEventT>
void Tracker<RawEventT>::start()
{
  stop();

  /* cleanup */

  orientation_ = OrientationEventT{};
  velocity_ = RawEventT{};
  displacement_ = RawEventT{};

  calibrated_ = false;

  thread_ = std::thread([&](){ this->run(); });
}

/**
 * @brief Returns if the processor is receiving events or not.
 *
 * It can take some time for the processor to stop after
 * putting the STOP event on the queue.
 */
template <typename RawEventT>
bool Tracker<RawEventT>::stopped() const
{
  return !thread_.joinable();
}

/**
 * @brief Returns if the processor is calibrated.
 */
template <typename RawEventT>
bool Tracker<RawEventT>::calibrated() const
{
  std::lock_guard<std::mutex> lock{mutex_};
  return calibrated_;
}

/**
 * @brief Puts an event on the queue or distributes system
 *        events (START, STOP...)
 * @param event Received event.
 */
template <typename RawEventT>
void Tracker<RawEventT>::receiveEvent(const RawEventT &event)
{
	// Handle system events
  switch (event.type) {
    case EventType::STOP:
      stop();
      return;
    case EventType::START:
      start();
      return;
    default:;
  }
  // Store sensor event.
  if (!stopped())
    events_.push_back(event);
}

/**
 * @brief The main processing function. Processes events from the queue
 *        and updates member variables representing the state of the device.
 */
template <typename RawEventT>
void Tracker<RawEventT>::run()
{
  // This method is rather big. All filters and variables are local
  // because their type is deducted by "auto". It would be difficult
  // to create an std::tuple<LotOfFilters> as a member variable.

  /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
  /* %%% CONFIGURATION DEFAULT VALUES %% */
  /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
  const auto STILLNESS_ACC_EVENT_NUMBER =
    conf_.ul("STILLNESS_ACC_EVENT_NUMBER", 16);
  const auto STILLNESS_GYR_EVENT_NUMBER =
    conf_.ul("STILLNESS_GYR_EVENT_NUMBER", 32);
  const auto STILLNESS_MAG_EVENT_NUMBER =
    conf_.ul("STILLNESS_MAG_EVENT_NUMBER", 16);
  const auto STILLNESS_ACC_VARIANCE_THRESHOLD =
    conf_.d("STILLNESS_ACC_VARIANCE_THRESHOLD", 0.01);
  const auto STILLNESS_GYR_SQUARES_THRESHOLD =
    conf_.d("STILLNESS_GYR_SQUARES_THRESHOLD", 0.1);
  const auto STILLNESS_MAG_VARIANCE_THRESHOLD =
    conf_.d("STILLNESS_MAG_VARIANCE_THRESHOLD", 0.15);
  const auto STILLNESS_GRAVITY_EPSILON =
    conf_.d("STILLNESS_GRAVITY_EPSILON", 1.5);
  const auto STILLNESS_MAG_FIELD_EPSILON =
    conf_.d("STILLNESS_MAG_FIELD_EPSILON", 30.0);
  const auto STILLNESS_SPEED_QUOCIENT =
    conf_.d("STILLNESS_SPEED_QUOCIENT", 0.0);
  const auto STILLNESS_UPDATE_GRAVITY =
    conf_.ul("STILLNESS_UPDATE_GRAVITY", 1);
  const auto ACC_DROP_INIT =
    conf_.ul("ACC_DROP_INIT", 32);
  const auto GYR_DROP_INIT =
    conf_.ul("GYR_DROP_INIT", 32);
  const auto MAG_DROP_INIT =
    conf_.ul("MAG_DROP_INIT", 32);
  const auto ACC_EPSILON_ZERO =
    conf_.d("ACC_EPSILON_ZERO", 0.07);
  const auto VEL_EPSILON_ZERO =
    conf_.d("VEL_EPSILON_ZERO", 0.3);
  const auto GYR_EPSILON_ZERO =
    conf_.d("GYR_EPSILON_ZERO", 0.02);

  /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
  /* %%%%%%% STILLNESS DETECTION %%%%%%% */
  /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

  // Calibration routine
  FunctionFilter<RawEventT> calibrate { [&](const RawEventT &) {
    if (calibrated_)
      return true;
    // If all the sensors report stillness
    if (accelerometer_stats_.size == STILLNESS_ACC_EVENT_NUMBER &&
        accelerometer_stats_.variance < STILLNESS_ACC_VARIANCE_THRESHOLD &&
        gyroscope_stats_.size == STILLNESS_GYR_EVENT_NUMBER &&
        gyroscope_stats_.squares < STILLNESS_GYR_SQUARES_THRESHOLD &&
        magnetic_field_stats_.size == STILLNESS_MAG_EVENT_NUMBER &&
        magnetic_field_stats_.variance < STILLNESS_MAG_VARIANCE_THRESHOLD) {
      // Store initial forces and mark as calibrated
      gravity_.value = accelerometer_stats_.mean;
      magnetic_field_init_.value = magnetic_field_stats_.mean;
      calibrated_ = true;
    }
    return false;
  }};

  // Fix drift routine
  Callback<RawEventT> fix_drift { [&]() {
    using namespace ArrayMatrix;
    using std::abs;

    // If the accelerometer and gyroscope are still
    if (gyroscope_stats_.squares < STILLNESS_GYR_SQUARES_THRESHOLD &&
        accelerometer_stats_.variance < STILLNESS_ACC_VARIANCE_THRESHOLD &&
        abs(norm(gravity_.value) - norm(accelerometer_stats_.mean)) <
        STILLNESS_GRAVITY_EPSILON) {

      // According to the orientation matrix, we know that gravity should
      // point towards direction A, but it points towards direction B.
      // We fix the orientation using a rotation matrix from A to B.
      // drift_fix * orientation * mean == gravity
      auto drift_fix =
          rotation_matrix(orientation_.value * accelerometer_stats_.mean,
                          gravity_.value);
      orientation_.value = drift_fix * orientation_.value;
      // As accelerometer is inaccurate and gravity magnitude
      // changes significantly during rotation, resize the gravity
      // to the current magnitude of acceleration
      if (STILLNESS_UPDATE_GRAVITY) {
        gravity_.value =
            gravity_.value *
            (accelerometer_stats_.mean_magnitude / norm(gravity_.value));
      }
      velocity_.value = velocity_.value * STILLNESS_SPEED_QUOCIENT;

      // If the magnetic field is also still
      if (magnetic_field_stats_.variance < STILLNESS_MAG_VARIANCE_THRESHOLD &&
          abs(norm(magnetic_field_stats_.mean)-norm(magnetic_field_init_.value))
          < STILLNESS_MAG_FIELD_EPSILON) {
        auto gravity_to_Z =
            rotation_matrix(gravity_.value, Vector<ValueT,3>{{0,0,1}});
        // Function to project a vector to a plane perpendicular to the gravity
        auto project = [&](Vector<ValueT, 3> v) {
          v = gravity_to_Z * v;
          v[2] = 0;
          return v * gravity_to_Z;
        };
        auto projected_mean =
            project(orientation_.value * magnetic_field_stats_.mean);
        auto projected_init =
            project(magnetic_field_init_.value);
        auto drift_fix =
            rotation_matrix(projected_mean, projected_init);
        orientation_.value = drift_fix * orientation_.value;
      }
    }
  }};

  /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
  /* %%%%%% ACCELEROMETER FILTERS %%%%%% */
  /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
  auto accelerometer_filters = std::make_tuple(
    Callback<RawEventT>{raw_accelerometer_callback_},
    Drop<RawEventT>{ACC_DROP_INIT},
    Statistics<RawEventT>{&accelerometer_stats_, STILLNESS_ACC_EVENT_NUMBER},
    calibrate,
    fix_drift,
    PreMultiplyBy<RawEventT, OrientationEventT>{&orientation_},
    RemoveValueOf<RawEventT>{&gravity_},
    EpsilonZero<RawEventT>{ACC_EPSILON_ZERO},
    Callback<RawEventT>{filtered_accelerometer_callback_},
    ChangeType<RawEventT>{EventType::VELOCITY},
    DeltaIntegrate<RawEventT>{},
    AddValueTo<RawEventT>{&velocity_},
    EpsilonZero<RawEventT>{VEL_EPSILON_ZERO},
    Callback<RawEventT>{velocity_callback_},
    ChangeType<RawEventT>{EventType::DISPLACEMENT},
    DeltaIntegrate<RawEventT>{},
    AddValueTo<RawEventT>{&displacement_},
    Callback<RawEventT>{displacement_callback_}
  );
  connect_pipeline(accelerometer_filters);

  /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
  /* %%%%%%%% GYROSCOPE FILTERS %%%%%%%% */
  /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
  auto gyroscope_filters = std::make_tuple(
    Callback<RawEventT>{raw_gyroscope_callback_},
    Drop<RawEventT>{GYR_DROP_INIT},
    Statistics<RawEventT>{&gyroscope_stats_, STILLNESS_GYR_EVENT_NUMBER},
    calibrate,
    fix_drift,
    EpsilonZero<RawEventT>{GYR_EPSILON_ZERO},
    Callback<RawEventT>{filtered_gyroscope_callback_},
    ChangeType<RawEventT>{EventType::ORIENTATION},
    DeltaIntegrate<RawEventT>{},
    ToRotationMatrix<RawEventT,OrientationEventT>{},
    PostMultiplyThe<OrientationEventT>{&orientation_},
    Callback<OrientationEventT>{orientation_callback_}
  );
  connect_pipeline(gyroscope_filters);

  /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
  /* %%%%%% MAGNETIC FIELD FILTERS %%%%% */
  /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
  auto magnetic_field_filters = std::make_tuple(
    Callback<RawEventT>{raw_magnetic_field_callback_},
    Drop<RawEventT>{MAG_DROP_INIT},
    Statistics<RawEventT>{&magnetic_field_stats_, STILLNESS_MAG_EVENT_NUMBER},
    calibrate,
    fix_drift,
    PreMultiplyBy<RawEventT, OrientationEventT>{&orientation_},
    RemoveValueOf<RawEventT>{&magnetic_field_init_},
    Callback<RawEventT>{filtered_magnetic_field_callback_}
  );
  connect_pipeline(magnetic_field_filters);

  /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
  /* %%% LINEAR ACCELERATION FILTERS %%% */
  /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
  // Linear acceleration and accelerometer + gyroscope
  // are mutually exclusive.
  auto linear_acceleration_filters = std::make_tuple(
    ChangeType<RawEventT>{EventType::ACCELEROMETER},
    Callback<RawEventT>{raw_linear_accelerometer_callback_},
    Drop<RawEventT>{ACC_DROP_INIT},
    Callback<RawEventT>{filtered_linear_accelerometer_callback_},
    ChangeType<RawEventT>{EventType::VELOCITY},
    DeltaIntegrate<RawEventT>{},
    AddValueTo<RawEventT>{&velocity_},
    Callback<RawEventT>{linear_velocity_callback_},
    ChangeType<RawEventT>{EventType::DISPLACEMENT},
    DeltaIntegrate<RawEventT>{},
    AddValueTo<RawEventT>{&displacement_},
    Callback<RawEventT>{linear_displacement_callback_}
  );
  connect_pipeline(linear_acceleration_filters);

  /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
  /* %%%%%%%%%%%% MAIN LOOP %%%%%%%%%%%% */
  /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
  for (auto event = events_.pop_get_front();
       event.type != EventType::STOP;
       event = events_.pop_get_front()) {
    std::lock_guard<std::mutex> lock(mutex_);
    switch (event.type) {
    case EventType::ACCELEROMETER:
      std::get<0>(accelerometer_filters).receive(event);
      break;
    case EventType::GYROSCOPE:
      std::get<0>(gyroscope_filters).receive(event);
      break;
    case EventType::MAGNETIC_FIELD:
      std::get<0>(magnetic_field_filters).receive(event);
      break;
    case EventType::LINEAR_ACCELERATION:
      std::get<0>(linear_acceleration_filters).receive(event);
      break;
    default:;
    }
  }
}

// %%% GYROSCOPE SPECIFIC %%%

/**
 * @brief Returns the current orientation.
 */
template <typename RawEventT>
typename Tracker<RawEventT>::OrientationEventT
  Tracker<RawEventT>::orientation() const
{
  std::lock_guard<std::mutex> lock{mutex_};
  return orientation_;
}

/**
 * @brief Sets a callback that will be called with each new
 *        raw gyroscope event.
 */
template <typename RawEventT>
void Tracker<RawEventT>::setRawGyroscopeCallback(
  std::function<void (const RawEventT &)> callback)
{
  raw_gyroscope_callback_ = callback;
}

/**
 * @brief Sets a callback that will be called with each new
 *        filtered gyroscope event.
 */
template <typename RawEventT>
void Tracker<RawEventT>::setFilteredGyroscopeCallback(
  std::function<void (const RawEventT &)> callback)
{
  filtered_gyroscope_callback_ = callback;
}

/**
 * @brief Sets a callback that will be called with each new
 *        orientation.
 */
template <typename RawEventT>
void Tracker<RawEventT>::setOrientationCallback(
  std::function<void (const OrientationEventT &)> callback)
{
  orientation_callback_ = callback;
}

/**
 * @brief Returns the current gyroscope statistics.
 */
template <typename RawEventT>
SensorStats<typename RawEventT::ValueT>
  Tracker<RawEventT>::gyroscopeStats() const
{
  std::lock_guard<std::mutex> lock{mutex_};
  return gyroscope_stats_;
}

// %%% ACCELEROMETER SPECIFIC %%%

/**
 * @brief Returns the current displacement.
 */
template <typename RawEventT>
RawEventT Tracker<RawEventT>::displacement() const
{
  std::lock_guard<std::mutex> lock{mutex_};
  return displacement_;
}

/**
 * @brief Returns the current velocity.
 */
template <typename RawEventT>
RawEventT Tracker<RawEventT>::velocity() const
{
  std::lock_guard<std::mutex> lock{mutex_};
  return velocity_;
}

/**
 * @brief Returns the current accelerometer statistics.
 */
template <typename RawEventT>
SensorStats<typename RawEventT::ValueT>
  Tracker<RawEventT>::accelerometerStats() const
{
  std::lock_guard<std::mutex> lock{mutex_};
  return accelerometer_stats_;
}

/**
 * @brief Sets a callback that will be called with each new
 *        raw accelerometer event.
 */
template <typename RawEventT>
void Tracker<RawEventT>::setRawAccelerometerCallback(
  std::function<void (const RawEventT &)> callback)
{
  raw_accelerometer_callback_ = callback;
  raw_linear_accelerometer_callback_ = callback;
}

/**
 * @brief Sets a callback that will be called with each new
 *        filtered (gravity is removed) accelerometer event.
 */
template <typename RawEventT>
void Tracker<RawEventT>::setFilteredAccelerometerCallback(
  std::function<void (const RawEventT &)> callback)
{
  filtered_accelerometer_callback_ = callback;
  filtered_linear_accelerometer_callback_ = callback;
}

/**
 * @brief Sets a callback that will be called with each new
 *        velocity.
 */
template <typename RawEventT>
void Tracker<RawEventT>::setVelocityCallback(
  std::function<void (const RawEventT &)> callback)
{
  velocity_callback_ = callback;
  linear_velocity_callback_ = callback;
}

/**
 * @brief Sets a callback that will be called with each new
 *        displacement.
 */
template <typename RawEventT>
void Tracker<RawEventT>::setDisplacementCallback(
  std::function<void (const RawEventT &)> callback)
{
  displacement_callback_ = callback;
  linear_displacement_callback_ = callback;
}

// %%% MAGNETIC FIELD SPECIFIC %%%

/**
 * @brief Sets a callback that will be called with each new
 *        raw magnetic field event.
 */
template <typename RawEventT>
void Tracker<RawEventT>::setRawMagneticFieldCallback(
  std::function<void (const RawEventT &)> callback)
{
  raw_magnetic_field_callback_ = callback;
}

/**
 * @brief Sets a callback that will be called with each new
 *        filtered magnetic field event.
 */
template <typename RawEventT>
void Tracker<RawEventT>::setFilteredMagneticFieldCallback(
  std::function<void (const RawEventT &)> callback)
{
  filtered_magnetic_field_callback_ = callback;
}

/**
 * @brief Returns the current magnetic field statistics.
 */
template <typename RawEventT>
SensorStats<typename RawEventT::ValueT>
  Tracker<RawEventT>::magneticFieldStats() const
{
  std::lock_guard<std::mutex> lock{mutex_};
  return magnetic_field_stats_;
}

/**
 * @brief Returns the reference to the configuration.
 */
template <typename RawEventT>
Configuration<std::string> &
  Tracker<RawEventT>::config()
{
  return conf_;
}

} // namespace TrackMe
#endif
