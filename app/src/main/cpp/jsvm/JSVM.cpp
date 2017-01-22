//
// Created by ntrrgc on 1/15/17.
//

#include <jsvm/JSVM.h>
#include "JSValue.h"
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

typedef std::function<duk_ret_t(duk_context*)> duk_safe_call_std_function;

static duk_ret_t
duk_raw_function_call(duk_context *ctx, void *udata) {
    duk_safe_call_std_function * function = (duk_safe_call_std_function *) udata;
    return (*function)(ctx);
}

template<typename T>
T
JSVMPriv_invokeSafe(JSVMPriv *priv, std::function<T(duk_context*)> callback) {
    // T has to be an assignable, concrete type (usually a pointer to a Java object)
    T ret;
    // Exceptions may be anything, so we have to use pointers to ensure virtual dispatch.
    std::unique_ptr<JavaException> capturedJavaException;

    duk_safe_call_std_function wrappedCallback = [&ret, &callback, &capturedJavaException](duk_context *ctx) {
        try {
            ret = std::move(callback(ctx));
        } catch (JavaException& javaException) {
            capturedJavaException = javaException.moveClone();
        }
        return 0;
    };

    int dukExecRet = duk_safe_call(priv->ctx, duk_raw_function_call, &wrappedCallback, 0, 1);

    if (capturedJavaException) {
        // A wrapped Java exception has been throw in C++, but not propagated yet to Java!
        // Propagate it now:
        JNIEnv* env = priv->env;
        jsvm_assert(env->ExceptionCheck() == JNI_FALSE);
        capturedJavaException->propagateToJava(env);

        duk_pop(priv->ctx);
        return NULL;
    } else if (dukExecRet == DUK_EXEC_ERROR) {
        // A JavaScript error has been thrown, wrap into a JSError class
        // and propagate it to Java.
        JNIEnv* env = priv->env;
        jsvm_assert(env->ExceptionCheck() == JNI_FALSE);
        JSError(env, JSValue_createFromStack(env, priv->jsVM, -1)).propagateToJava(env);

        duk_pop(priv->ctx);
        return NULL;
    } else {
        // Successful execution. Return the return value of the provided callback.

        duk_pop(priv->ctx);
        return ret;
    }
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

    return (int) duk_get_top_index(ctx);

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
    duk_push_string(ctx, "global");
    duk_push_global_object(ctx);
    duk_put_prop(ctx, -3);

    this->objectBook.lateInit(ctx);
}

JSVMPriv::~JSVMPriv() {
    duk_destroy_heap(ctx);
    ctx = NULL;
}