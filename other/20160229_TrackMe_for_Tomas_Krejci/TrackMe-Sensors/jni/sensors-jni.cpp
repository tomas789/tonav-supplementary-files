#include <jni.h>
#include <android/looper.h>
#include <android/sensor.h>
#include <android/log.h>

#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "sensors-jni", __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, "sensors-jni", __VA_ARGS__))
#define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR, "sensors-jni", __VA_ARGS__))

static ALooper *looper;
static const int LOOPER_ID = 1;
static ASensorManager *sensorManager;
static const ASensor *accelerometer;
static const ASensor *gyroscope;
static const ASensor *magnetic_field;
static ASensorEventQueue *sensorEventQueue;
static jmethodID JniSensorReaderPut;
static JNIEnv *env;
static jobject self;

int getEvents(int fd, int events, void *data)
{
  ASensorEvent event;
  while (ASensorEventQueue_getEvents(sensorEventQueue, &event, 1) > 0) {
    switch (event.type) {
    case ASENSOR_TYPE_ACCELEROMETER:
    case ASENSOR_TYPE_GYROSCOPE:
    case ASENSOR_TYPE_MAGNETIC_FIELD:
      env->CallVoidMethod(self, JniSensorReaderPut,
                          (jint)event.type,
                          (jlong)event.timestamp,
                          (jfloat)event.vector.v[0],
                          (jfloat)event.vector.v[1],
                          (jfloat)event.vector.v[2]);
      break;
    default:;
    }
  }
  return 1;
}

extern "C" {
void Java_trackmesensors_main_JniSensorReader_jniStart(
  JNIEnv *env_, jobject self_)
{
  int ret;

  env = env_;
  self = env->NewGlobalRef(self_);

  // Set looper
  looper = ALooper_forThread();
  if (looper == NULL)
    looper = ALooper_prepare(ALOOPER_PREPARE_ALLOW_NON_CALLBACKS);
  if (looper == NULL)
    LOGE("Looper cannot be assigned.");

  // Set callback
  jclass classHandle = env->FindClass("trackmesensors/main/JniSensorReader");
  JniSensorReaderPut = env->GetMethodID(classHandle, "put", "(IJFFF)V");

  // Set sensors
  sensorManager =
      ASensorManager_getInstance();
  accelerometer =
      ASensorManager_getDefaultSensor(sensorManager, ASENSOR_TYPE_ACCELEROMETER);
  gyroscope =
      ASensorManager_getDefaultSensor(sensorManager, ASENSOR_TYPE_GYROSCOPE);
  magnetic_field =
      ASensorManager_getDefaultSensor(sensorManager, ASENSOR_TYPE_MAGNETIC_FIELD);
  sensorEventQueue =
      ASensorManager_createEventQueue(sensorManager,
                                      looper, LOOPER_ID,
                                      getEvents, NULL);

  if ((ret =
      ASensorEventQueue_enableSensor(sensorEventQueue, accelerometer)) < 0)
    LOGE("Accelerometer cannot be enabled. Error %d", ret);

  if ((ret =
      ASensorEventQueue_enableSensor(sensorEventQueue, gyroscope)) < 0)
    LOGE("Gyroscope cannot be enabled. Error %d", ret);

  if ((ret =
      ASensorEventQueue_enableSensor(sensorEventQueue, magnetic_field)) < 0)
    LOGE("Magnetic field cannot be enabled. Error %d", ret);

  if ((ret =
      ASensorEventQueue_setEventRate(sensorEventQueue, accelerometer,
                                     ASensor_getMinDelay(accelerometer))) < 0)
    LOGE("Accelerometer event rate cannot be set. Error %d", ret);

  if ((ret =
      ASensorEventQueue_setEventRate(sensorEventQueue, gyroscope,
                                     ASensor_getMinDelay(gyroscope))) < 0)
    LOGE("Gyroscope event rate cannot be set. Error %d", ret);

  if ((ret =
      ASensorEventQueue_setEventRate(sensorEventQueue, magnetic_field,
                                     ASensor_getMinDelay(magnetic_field))) < 0)
    LOGE("Magnetic field event rate cannot be set. Error %d", ret);
}}

extern "C" {
void Java_trackmesensors_main_JniSensorReader_jniStop(
  JNIEnv *env_, jobject *self_)
{
  ASensorEventQueue_disableSensor(sensorEventQueue, accelerometer);
  ASensorEventQueue_disableSensor(sensorEventQueue, gyroscope);
  ASensorManager_destroyEventQueue(sensorManager, sensorEventQueue);
  env->DeleteGlobalRef(self);
}}
