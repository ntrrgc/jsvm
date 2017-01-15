//
// Created by ntrrgc on 1/15/17.
//

#pragma once

#include <jni.h>
#include <string>

namespace jsvm {

    extern jclass       Boolean_Class;
    extern jmethodID    Boolean_ctor;

    extern jclass       Double_Class;
    extern jmethodID    Double_ctor;

    extern jclass       JSVM_Class;
    extern jfieldID     JSVM_hPriv;

    extern jclass       JSRuntimeException_Class;

    extern jclass       JSValue_Class;
    extern jfieldID     JSValue_jsVM;
    extern jfieldID     JSValue_type;
    extern jfieldID     JSValue_value;

    extern jclass       JSObject_Class;
    extern jmethodID    JSObject_ctor;
    extern jfieldID     JSObject_jsVM;
    extern jfieldID     JSObject_handle;

    void initClassesAndFields(JNIEnv *env);
    void tearDownClassesAndFields(JNIEnv *env);

}
