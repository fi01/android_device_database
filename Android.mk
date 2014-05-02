MY_LOCAL_PATH := $(call my-dir)

LOCAL_PATH := $(MY_LOCAL_PATH)

include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
  device_database.c

LOCAL_MODULE := libdevice_database
LOCAL_MODULE_TAGS := optional
LOCAL_C_INCLUDES += $(LOCAL_PATH)/libsqlite

LOCAL_STATIC_LIBRARIES += libsqlite

include $(BUILD_STATIC_LIBRARY)

include $(MY_LOCAL_PATH)/libsqlite/Android.mk
