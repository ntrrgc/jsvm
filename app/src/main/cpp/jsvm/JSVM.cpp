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
    priv->setUpEnv(env, jsVM);
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
duk_safe_call_lambda(JSVMPriv *priv, JNIEnv *env, duk_context* ctx, std::function<T(duk_context*)> callback) {
    T ret;
    duk_safe_call_std_function wrappedCallback = [&ret, &callback](duk_context *ctx) {
        ret = std::move(callback(ctx));
        return 0;
    };

    int jsErrorThrew = duk_safe_call(ctx, duk_raw_function_call, &wrappedCallback, 0, 1);
    if (jsErrorThrew == 0) {
        duk_pop(ctx);
        return ret;
    } else {
        assert(env->ExceptionCheck() == JNI_FALSE);
        priv->propagateErrorToJava(env, -1);
        duk_pop(ctx);
        return NULL;
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
        env->ThrowNew(JSRuntimeException_Class, "Failed to create JS heap");
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

    duk_context *ctx = priv->ctx;

    return duk_safe_call_lambda<JSValue>(priv, env, ctx, [env, code, jsVM](duk_context* ctx) -> JSValue {
        String_pushJString(env, code, ctx);
        duk_eval(ctx);
        return JSValue_createFromStackOrThrow(env, jsVM, -1);
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

static void
JSVM_onUnhandledJSError(void *priv_, const char *msg) {
    JSVMPriv* priv = (JSVMPriv *) priv_;
    JSVM jsVM = priv->jsVM;
    JNIEnv* env = priv->env;

    Result<JSValue> jsErrorValueResult = JSValue_createFromStack(env, jsVM, 1);
    if (THREW_EXCEPTION == jsErrorValueResult.status()) {
        return;
    }
    JSValue jsErrorValue = jsErrorValueResult.get();

    env->Throw((jthrowable) env->NewObject(
            JSError_Class, JSError_ctor, (jobject) jsErrorValue));

    // Return control to setUpExceptionHandler() and make it
    // return THREW_EXCEPTION
    priv->unwindIntoExceptionHandler();
}

JSVMPriv::JSVMPriv(JNIEnv *initialJNIEnv, JSVM initialJSVM)
        : env(initialJNIEnv), jsVM(initialJSVM)
{
    ctx = duk_create_heap(
            NULL, NULL, NULL, this, JSVM_onUnhandledJSError
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

void JSVMPriv::propagateErrorToJava(JNIEnv *env, int errorStackPos) {
    Result<JSValue> jsErrorValueResult = JSValue_createFromStack(env, jsVM, errorStackPos);
    if (THREW_EXCEPTION == jsErrorValueResult.status()) {
        return;
    }
    JSValue jsErrorValue = jsErrorValueResult.get();

    env->Throw((jthrowable) env->NewObject(
            JSError_Class, JSError_ctor, (jobject) jsErrorValue));
}