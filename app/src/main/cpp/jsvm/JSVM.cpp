//
// Created by ntrrgc on 1/15/17.
//

#include <jsvm/JSVM.h>
#include "JSValue.h"

using namespace jsvm;

JSVMPriv *
jsvm::JSVM_getPriv(JNIEnv *env, JSVM jsVM) {
    return (JSVMPriv *) env->GetLongField(jsVM, JSVM_hPriv);
}

extern "C" {

JNIEXPORT void JNICALL
Java_me_ntrrgc_jsvm_JSVM_initializeLibrary(JNIEnv *env, jclass type) {
    initClassesAndFields(env);
}

JNIEXPORT void JNICALL
Java_me_ntrrgc_jsvm_JSVM_destroyLibrary(JNIEnv *env, jclass type) {
    tearDownClassesAndFields(env);
}

JNIEXPORT void JNICALL
Java_me_ntrrgc_jsvm_JSVM_nativeInit(JNIEnv *env, jobject instance) {
    JSVMPriv *priv = new JSVMPriv();

    if (priv->ctx != NULL) {
        env->SetLongField(instance, JSVM_hPriv, (jlong) priv);
    } else {
        env->ThrowNew(JSRuntimeException_Class, "Failed to create JS heap");
    }
}

JNIEXPORT void JNICALL
Java_me_ntrrgc_jsvm_JSVM_finalize(JNIEnv *env, jobject instance) {
    JSVM jsVM = (JSVM) instance;

    JSVMPriv* priv = JSVM_getPriv(env, jsVM);
    delete priv;
}

JNIEXPORT jobject JNICALL
Java_me_ntrrgc_jsvm_JSVM_evaluateScriptNative(JNIEnv *env, jobject instance, jstring code_) {
    JSVM jsVM = (JSVM) instance;
    const char *code = env->GetStringUTFChars(code_, 0);

    duk_context *ctx = JSVM_getPriv(env, jsVM)->ctx;

    duk_eval_string(ctx, code);
    jobject returnValue = JSValue_createFromStackTop(env, jsVM);
    duk_pop(ctx);

    env->ReleaseStringUTFChars(code_, code);
    return returnValue;
}

}

JSVMPriv::JSVMPriv() {
    ctx = duk_create_heap(
            NULL, NULL, NULL, NULL, NULL
    );
    if (ctx == NULL) {
        return;
    }

    // Make global object accessible with "global".
    duk_push_global_object(ctx);
    duk_push_string(ctx, "global");
    duk_push_global_object(ctx);
    duk_put_prop(ctx, -3);

    this->objectBook.lateInit(ctx);
}

JSVMPriv::~JSVMPriv() {
    duk_destroy_heap(ctx);
    ctx = NULL;
}