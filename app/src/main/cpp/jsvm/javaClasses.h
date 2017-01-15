//
// Created by ntrrgc on 1/15/17.
//

#pragma once

#include <jni.h>
#include <string>

namespace jsvm {

    extern jclass JSVM_Class;
    extern jfieldID JSVM_hDukContext;

    extern jclass JSRuntimeException_Class;

    extern jclass JSValue_Class;
    extern jfieldID JSValue_jsVM;

    void initClassesAndFields(JNIEnv *env);
    void tearDownClassesAndFields(JNIEnv *env);

}
