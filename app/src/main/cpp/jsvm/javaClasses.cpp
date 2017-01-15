//
// Created by ntrrgc on 1/15/17.
//

#include "javaClasses.h"

namespace jsvm {
    jclass JSVM_Class = NULL;
    jfieldID JSVM_hDukContext = NULL;

    jclass JSRuntimeException_Class = NULL;

    jclass JSValue_Class = NULL;
    jfieldID JSValue_jsVM = NULL;
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
    JSVM_Class = findClass(env, "me/ntrrgc/jsvm/JSVM");
    JSVM_hDukContext = env->GetFieldID(JSVM_Class, "hDukContext", "J");

    JSRuntimeException_Class = findClass(env, "me/ntrrgc/jsvm/JSRuntimeException");

    JSValue_Class = findClass(env, "me/ntrrgc/jsvm/JSValue");
    JSValue_jsVM = env->GetFieldID(JSValue_Class, "jsVM", "L" "me/ntrrgc/jsvm/JSVM;");
}


void ::jsvm::tearDownClassesAndFields(JNIEnv *env) {
    removeClass(env, &JSVM_Class);
    removeClass(env, &JSValue_Class);
    removeClass(env, &JSRuntimeException_Class);
}

