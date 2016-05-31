LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := sensors-jni
LOCAL_SRC_FILES := sensors-jni.cpp
LOCAL_LDLIBS    += -llog -landroid 

include $(BUILD_SHARED_LIBRARY)
