//
// Created by ntrrgc on 1/15/17.
//

#include <jsvm/JSVM.h>
#include "JSValue.h"
#include "invokeSafe.h"
#include "JSObject.h"
#include <functional>

using namespace jsvm;

/**
 * Get a JSVMPriv object with its JNIEnv and JSVM pointers
 * already set so that Duktape functions can be called safely.
 */
JSVMPriv *
jsvm::JSVM_getPriv(JNIEnv *env, JSVM jsVM) {
    JSVMPriv* priv = (JSVMPriv *) env->GetLongField(jsVM, JSVM_hPriv);
    priv->load(env, jsVM);
    return priv;
}

extern "C" {

extern int weakRefCount;
JNIEXPORT jint JNICALL
Java_me_ntrrgc_jsvm_JSVM_getWeakRefCount(JNIEnv *env, jobject instance) {

    return weakRefCount;

}

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
    JSVM jsVM = (JSVM) instance;
    JSVMPriv *priv = new JSVMPriv(env, jsVM);

    if (priv->ctx != NULL) {
        env->SetLongField(instance, JSVM_hPriv, (jlong) priv);
    } else {
        env->ThrowNew(JSVMInternalError_Class, "Failed to create JS heap");
    }
}

JNIEXPORT void JNICALL
Java_me_ntrrgc_jsvm_JSVM_finalizeNative(JNIEnv *env, jobject instance) {
    JSVM jsVM = (JSVM) instance;

    JSVMPriv* priv = JSVM_getPriv(env, jsVM);
    delete priv;
}

JNIEXPORT jobject JNICALL
Java_me_ntrrgc_jsvm_JSVM_evaluateScriptNative(JNIEnv *env, jobject instance, jstring code) {
    JSVM jsVM = (JSVM) instance;
    JSVMPriv* priv = JSVM_getPriv(env, jsVM);

    return JSVMPriv_invokeSafe<JSValue>(priv, [priv, env, jsVM, code](duk_context *ctx) {
        String_pushJString(env, code, ctx);
        duk_eval(ctx);
        return JSValue_createFromStack(env, jsVM, -1);
    });
}


JNIEXPORT int JNICALL
Java_me_ntrrgc_jsvm_JSVM_getStackFrameSizeNative(JNIEnv *env, jobject instance) {

    JSVM jsVM = (JSVM) instance;
    JSVMPriv* priv = JSVM_getPriv(env, jsVM);
    duk_context* ctx = priv->ctx;

    return (int) duk_get_top(ctx);

}

JNIEXPORT int JNICALL
Java_me_ntrrgc_jsvm_JSVM_getStackSizeNative(JNIEnv *env, jobject instance) {

    JSVM jsVM = (JSVM) instance;
    JSVMPriv* priv = JSVM_getPriv(env, jsVM);
    duk_context* ctx = priv->ctx;

    duk_idx_t topIndex = duk_get_top_index(ctx);
    return topIndex != DUK_INVALID_INDEX ? (int) topIndex - 1 : 0;

}

JNIEXPORT jobject JNICALL
Java_me_ntrrgc_jsvm_JSVM_getGlobalScopeNative(JNIEnv *env, jobject instance) {

    JSVM jsVM = (JSVM) instance;
    JSVMPriv* priv = JSVM_getPriv(env, jsVM);

    return JSVMPriv_invokeSafe<JSObject>(priv, [priv, env, jsVM] (duk_context *ctx) {

        // Push global object
        duk_push_global_object(ctx);

        JSObject ret = priv->objectBook.exposeObject(env, -1);

        // Restore stack
        duk_pop(ctx); // object

        return ret;

    });
}

JNIEXPORT jobject JNICALL
Java_me_ntrrgc_jsvm_JSVM_newObjectNative(JNIEnv *env, jobject instance) {

    JSVM jsVM = (JSVM) instance;
    JSVMPriv* priv = JSVM_getPriv(env, jsVM);

    return JSVMPriv_invokeSafe<JSObject>(priv, [priv, env, jsVM] (duk_context *ctx) {

        // Push empty object
        duk_push_object(ctx);

        JSObject ret = priv->objectBook.exposeObject(env, -1);

        // Restore stack
        duk_pop(ctx); // object

        return ret;

    });
}

JNIEXPORT jobject JNICALL
Java_me_ntrrgc_jsvm_JSVM_newObjectNativeWithProto(JNIEnv *env, jobject instance, jobject proto_) {

    JSVM jsVM = (JSVM) instance;
    JSVMPriv* priv = JSVM_getPriv(env, jsVM);
    JSObject proto = (JSObject) proto_;

    return JSVMPriv_invokeSafe<JSObject>(priv, [priv, env, jsVM, proto] (duk_context *ctx) {

        if (proto == NULL) {
            // Push bare object (inherits from null)
            duk_push_bare_object(ctx);
        } else {
            // Push new object and set prototype
            duk_push_object(ctx);

            JSObject_push(env, proto);
            duk_set_prototype(ctx, -2);
        }

        JSObject ret = priv->objectBook.exposeObject(env, -1);

        // Restore stack
        duk_pop(ctx); // object

        return ret;

    });

}

}

JSVMPriv::JSVMPriv(JNIEnv *initialJNIEnv, JSVM initialJSVM)
        : env(initialJNIEnv), jsVM(initialJSVM)
{
    ctx = duk_create_heap(
            NULL, NULL, NULL, NULL, NULL
    );
    if (ctx == NULL) {
        return;
    }

    // Make global object accessible with "global".
    duk_push_global_object(ctx);
    duk_push_global_object(ctx);
    duk_put_prop_string(ctx, -2, "global");
    duk_pop(ctx);

    this->objectBook.lateInit(ctx, this);
}

JSVMPriv::~JSVMPriv() {
    duk_destroy_heap(ctx);
    ctx = NULL;
}