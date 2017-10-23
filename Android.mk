INNER_SAVED_LOCAL_PATH := $(LOCAL_PATH)

LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := gum

LOCAL_C_INCLUDES  := \
	${CLIB_PATH} \
	${RIGGING_SRC_PATH} \
	${GUM_SRC_PATH}/include \
	${GUM_SRC_PATH}/include/gum \
	${SM_SRC_PATH} \
	${FS_SRC_PATH} \
	${SIMP_SRC_PATH}/include \
	${BIMP_SRC_PATH}/include \
	${TIMP_SRC_PATH}/include \
	${LOGGER_SRC_PATH} \
	${DTEX2_SRC_PATH}/include \
	${SHADERLAB_SRC_PATH}/include \
	${UNIRENDER_SRC_PATH}/include \
	${UNIAUDIO_SRC_PATH}/include \
	${SPRITE2_SRC_PATH}/include \
	${JSON_SRC_PATH}/include \
	${COOKING_SRC_PATH}/include \
	${GTXT_SRC_PATH} \
	${GIMG_SRC_PATH} \
	${POLYMESH_SRC_PATH}/include \
	${PS_SRC_PATH} \
	${MTRAIL_SRC_PATH} \
	${BOOST_PATH}/include \
	${MULTITASK_SRC_PATH}/include \
	${GLP_SRC_PATH} \
	${DS_SRC_PATH} \
	${OPENAL_SRC_PATH} \
	${LUA_SRC_PATH} \
	${FLATTEN_SRC_PATH}/include \
	${MEMMGR_SRC_PATH}/include \

LOCAL_CPPFLAGS  := -DNO_BOOST -g

LOCAL_SRC_FILES := \
	$(subst $(LOCAL_PATH)/,,$(shell find $(LOCAL_PATH)/source/ -name "*.cpp" -print)) \

LOCAL_STATIC_LIBRARIES := \
	gimg \
	bimp \
	timp \
	sprite2 \
	shaderlab \
	dtex2 \
	multitask \
	cooking \

include $(BUILD_STATIC_LIBRARY)	

LOCAL_PATH := $(INNER_SAVED_LOCAL_PATH)