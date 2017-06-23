INNER_SAVED_LOCAL_PATH := $(LOCAL_PATH)

LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := gum

LOCAL_C_INCLUDES  := \
	${RIGGING_SRC_PATH} \
	${GUM_SRC_PATH}/include \
	${GUM_SRC_PATH}/include/gum \
	${SM_SRC_PATH} \
	${CU_SRC_PATH} \
	${FS_SRC_PATH} \
	${SIMP_SRC_PATH}/include \
	${BIMP_SRC_PATH}/include \
	${TIMP_SRC_PATH}/include \
	${LOGGER_SRC_PATH} \
	${DTEX2_SRC_PATH}/include \
	${SHADERLAB_SRC_PATH}/include \
	${UNIRENDER_SRC_PATH}/include \
	${SPRITE2_SRC_PATH}/include \
	${JSON_SRC_PATH}/include \
	${GTXT_SRC_PATH} \
	${GIMG_SRC_PATH} \
	${POLYMESH_SRC_PATH}/include \
	${PS_SRC_PATH} \
	${MTRAIL_SRC_PATH} \
	${BOOST_PATH}/include \
	${TASKS_SRC_PATH} \
	${GLP_SRC_PATH} \


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

include $(BUILD_STATIC_LIBRARY)	

LOCAL_PATH := $(INNER_SAVED_LOCAL_PATH)