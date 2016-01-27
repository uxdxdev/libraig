LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := socket_shared

LOCAL_MODULE_FILENAME := libsocket

LOCAL_SRC_FILES :=	$(LOCAL_PATH)/external/libsocket/src/socket.c

LOCAL_EXPORT_C_INCLUDES :=	$(LOCAL_PATH)/external/libsocket/include

LOCAL_C_INCLUDES :=	$(LOCAL_PATH)/external/libsocket/include					 

include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)

LOCAL_MODULE := raig_shared

LOCAL_MODULE_FILENAME := libraig

LOCAL_SRC_FILES :=	$(LOCAL_PATH)/src/raig/raig_client.cc \
					$(LOCAL_PATH)/src/base/vector3.cc \
					$(LOCAL_PATH)/src/net/net_manager.cc

LOCAL_EXPORT_C_INCLUDES :=	$(LOCAL_PATH)/include \
							$(LOCAL_PATH)/src 

LOCAL_C_INCLUDES :=	$(LOCAL_PATH)/include \
					$(LOCAL_PATH)/src 
					
LOCAL_SHARED_LIBRARIES := socket_shared

include $(BUILD_SHARED_LIBRARY)