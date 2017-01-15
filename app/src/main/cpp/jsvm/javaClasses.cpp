//
// Created by ntrrgc on 1/15/17.
//

#include "javaClasses.h"

namespace jsvm {
    jclass      Boolean_Class = NULL;
    jmethodID   Boolean_ctor = NULL;

    jclass      Double_Class = NULL;
    jmethodID   Double_ctor = NULL;

    jclass      JSVM_Class = NULL;
    jfieldID    JSVM_hPriv = NULL;

    jclass      JSRuntimeException_Class = NULL;

    jclass      JSValue_Class = NULL;
    jfieldID    JSValue_type = NULL;
    jfieldID    JSValue_value = NULL;

    jclass      JSObject_Class = NULL;
    jmethodID   JSObject_ctor = NULL;
    jfieldID    JSObject_jsVM = NULL;
    jfieldID    JSObject_handle = NULL;
}

using namespace jsvm;

static jclass
findClass(JNIEnv *env, const char *name) {
    jclass tmpClassRef;

    tmpClassRef = env->FindClass(name);
    jclass ret = (jclass) env->NewGlobalRef(tmpClassRef);
    env->DeleteLocalRef(tmpClassRef);

    return ret;
}

static void
removeClass(JNIEnv *env, jclass *classRef) {
    if (*classRef != NULL) {
        env->DeleteGlobalRef(*classRef);
        *classRef = NULL;
    }
}

void ::jsvm::initClassesAndFields(JNIEnv *env) {
    Boolean_Class = findClass(env, "java/lang/Boolean");
    Boolean_ctor = env->GetMethodID(Boolean_Class, "<init>", "(Z)V");

    Double_Class = findClass(env, "java/lang/Double");
    Double_ctor = env->GetMethodID(Double_Class, "<init>", "(D)V");

    JSVM_Class = findClass(env, "me/ntrrgc/jsvm/JSVM");
    JSVM_hPriv = env->GetFieldID(JSVM_Class, "hPriv", "J");

    JSRuntimeException_Class = findClass(env, "me/ntrrgc/jsvm/JSRuntimeException");

    JSValue_Class = findClass(env, "me/ntrrgc/jsvm/JSValue");
    JSValue_type = env->GetFieldID(JSValue_Class, "type", "I");
    JSValue_value = env->GetFieldID(JSValue_Class, "value", "L" "java/lang/Object;");

    JSObject_Class = findClass(env, "me/ntrrgc/jsvm/JSObject");
    JSObject_ctor = env->GetMethodID(JSObject_Class, "<init>", "()V");
    JSObject_jsVM = env->GetFieldID(JSObject_Class, "jsVM", "L" "me/ntrrgc/jsvm/JSVM;");
    JSObject_handle = env->GetFieldID(JSObject_Class, "handle", "I");
}


void ::jsvm::tearDownClassesAndFields(JNIEnv *env) {
    removeClass(env, &Boolean_Class);
    removeClass(env, &Double_Class);
    removeClass(env, &JSVM_Class);
    removeClass(env, &JSRuntimeException_Class);
    removeClass(env, &JSValue_Class);
    removeClass(env, &JSObject_Class);
}

