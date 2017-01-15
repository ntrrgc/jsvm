//
// Created by ntrrgc on 1/15/17.
//

#include <jni.h>
#include "JSVM.h"
#include "JSValue.h"

using namespace jsvm;

duk_context * ::jsvm::JSVM_getDukContext(JNIEnv *env, JSVM jsVM) {
    return (duk_context *) env->GetLongField(jsVM, JSVM_hDukContext);
}

extern "C" {

JNIEXPORT void JNICALL
Java_me_ntrrgc_jsvm_JSVM_nativeInit(JNIEnv *env, jobject instance) {
    duk_context *ctx = duk_create_heap(
            NULL, NULL, NULL, NULL, NULL
    );
    if (ctx == NULL) {
        env->ThrowNew(JSRuntimeException_Class, "Failed to create JS heap");
        return;
    }

    duk_push_global_object(ctx);
    duk_push_string(ctx, "global");
    duk_push_global_object(ctx);
    duk_put_prop(ctx, -3);

    env->SetLongField(instance, JSVM_hDukContext, (jlong) ctx);

}

JNIEXPORT void JNICALL
Java_me_ntrrgc_jsvm_JSVM_initialize(JNIEnv *env, jclass type) {
    initClassesAndFields(env);
}

JNIEXPORT void JNICALL
Java_me_ntrrgc_jsvm_JSVM_destroy(JNIEnv *env, jclass type) {
    tearDownClassesAndFields(env);
}

JNIEXPORT jobject JNICALL
Java_me_ntrrgc_jsvm_JSVM_evaluateScript(JNIEnv *env, jobject instance, jstring code_) {
    JSVM jsVM = (JSVM) instance;
    const char *code = env->GetStringUTFChars(code_, 0);

    duk_context *ctx = JSVM_getDukContext(env, jsVM);

    duk_eval_string(ctx, code);
    jobject returnValue = JSValue_createFromStackTop(env, jsVM);
    duk_pop(ctx);

    env->ReleaseStringUTFChars(code_, code);
    return returnValue;
}

}