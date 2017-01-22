//
// Created by ntrrgc on 1/15/17.
//

#pragma once

#include <stdafx.h>

namespace jsvm {

    extern jclass       Boolean_Class;
    extern jmethodID    Boolean_ctor;
    extern jfieldID     Boolean_value;

    extern jclass       Double_Class;
    extern jmethodID    Double_ctor;
    extern jfieldID     Double_value;

    extern jclass       JSVM_Class;
    extern jfieldID     JSVM_hPriv;

    extern jclass       JSRuntimeException_Class;

    extern jclass       JSError_Class;
    extern jmethodID    JSError_ctor;

    extern jclass       IllegalArgumentException_Class;

    extern jclass       AttemptedToUseObjectFromOtherVM_Class;
    extern jmethodID    AttemptedToUseObjectFromOtherVM_ctor;

    extern jclass       JSValue_Class;
    extern jfieldID     JSValue_type;
    extern jfieldID     JSValue_value;

    extern jclass       JSObject_Class;
    extern jmethodID    JSObject_ctor;
    extern jfieldID     JSObject_jsVM;
    extern jfieldID     JSObject_handle;

    void initClassesAndFields(JNIEnv *env);
    void tearDownClassesAndFields(JNIEnv *env);

}
