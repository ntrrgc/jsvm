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

static duk_ret_t
int_function_call(duk_context *ctx, void *udata) {
    std::function<duk_ret_t()> * function = (std::function<duk_ret_t()> *) udata;
    return (*function)();
}

may_throw
duk_safe_call_lambda(duk_context* ctx, std::function<duk_ret_t()> lambda) {
    int ret = duk_safe_call(ctx, int_function_call, &lambda, 0, 1);
    if (ret == 0) {
        return OK;
    } else {
        return THREW_EXCEPTION;
    }
}

JNIEXPORT jobject JNICALL
Java_me_ntrrgc_jsvm_JSVM_evaluateScriptNative(JNIEnv *env, jobject instance, jstring code_) {
    JSVM jsVM = (JSVM) instance;
    JSVMPriv* priv = JSVM_getPriv(env, jsVM);

    const char *code = env->GetStringUTFChars(code_, 0);

    duk_context *ctx = priv->ctx;
    jobject ret;

    if (THREW_EXCEPTION == setjmp(priv->allocateExceptionHandler())) {
        ret = NULL;
        goto release;
    }

    {
        if (THREW_EXCEPTION == duk_safe_call_lambda(ctx, [ctx, code]() {
            duk_eval_string(ctx, code);
            return 1;
        })) {
            priv->propagateErrorToJava(env, -1);
            ret = NULL;
        } else {
            Result<JSValue> valueResult = JSValue_createFromStack(env, jsVM, -1);
            if (THREW_EXCEPTION == valueResult.status()) {
                ret = NULL;
            } else {
                ret = valueResult.get();
            }
            duk_pop(ctx);
        }
    }

release:
    priv->tearDownExceptionHandler();
    env->ReleaseStringUTFChars(code_, code);
    return ret;
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
    Result<JSValue> jsErrorValueResult = JSValue_createFromStack(env, jsVM, 1);
    if (THREW_EXCEPTION == jsErrorValueResult.status()) {
        return;
    }
    JSValue jsErrorValue = jsErrorValueResult.get();

    env->Throw((jthrowable) env->NewObject(
            JSError_Class, JSError_ctor, (jobject) jsErrorValue));
}