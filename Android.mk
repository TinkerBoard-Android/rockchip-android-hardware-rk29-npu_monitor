LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_C_INCLUDES += system/core/include/cutils
LOCAL_SHARED_LIBRARIES := liblog libcutils libutils

LOCAL_CPPFLAGS += -D_FILE_OFFSET_BITS=64 -D_LARGE_FILE -fexceptions -Wno-non-virtual-dtor
LOCAL_CFLAGS += -Wno-error 
LOCAL_LDLIBS = -Wl,-Bstatic -Wl,-Bdynamic -Wl,-Bstatic
LOCAL_SRC_FILES := main.cpp
LOCAL_INIT_RC := npu_monitor.rc

LOCAL_MULTILIB := 64
LOCAL_MODULE_TARGET_ARCH := arm64
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE := npu_monitor

include $(BUILD_EXECUTABLE)

