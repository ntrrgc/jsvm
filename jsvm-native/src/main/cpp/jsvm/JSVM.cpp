/* Copyright (c) the JSVM authors
 * https://github.com/ntrrgc/jsvm
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

//
// Created by ntrrgc on 1/15/17.
//

#include <jsvm/JSVM.h>
#include "JSValue.h"
#include "invokeSafe.h"
#include "JSObject.h"
#include "JSFunction.h"
#include <functional>

using namespace jsvm;

thread_local std::stack<JSVMCallContext*> JSVMCallContext::s_thisThreadStack;

/** Get the JSVMPriv from a JSVM object.
 * In situations where a JS code will be run this function should not be
 * used directly, opting instead for JSVMCallContext so that the JSVM
 * pointers can be retrieved if JS needs to call Java code. */
JSVMPriv *
jsvm::JSVM_peekPriv(JNIEnv *env, JSVM jsVM) {
    return (JSVMPriv *) env->GetLongField(jsVM, JSVM_hPriv);
}

JSVMCallContext::JSVMCallContext(JNIEnv *jniEnv, JSVM jsVM)
    : m_jniEnv(jniEnv)
    , m_jsVM(jsVM)
{
    m_priv = JSVM_peekPriv(jniEnv, jsVM);
    s_thisThreadStack.push(this);
}

JSVMCallContext::~JSVMCallContext() {
    jsvm_assert(s_thisThreadStack.top() == this);
    s_thisThreadStack.pop();
}

JSVMCallContext&
JSVMCallContext::current() {
    jsvm_assert(!s_thisThreadStack.empty());
    return *s_thisThreadStack.top();
}

ArrayList JSVMCallContext::jsObjectsByHandle() {
    if (!m_jsObjectsByHandle)
        m_jsObjectsByHandle = (ArrayList) m_jniEnv->GetObjectField(m_jsVM, JSVM_jsObjectsByHandle);
    jsvm_assert(m_jsObjectsByHandle);
    return m_jsObjectsByHandle;
}

// Duktape will call this function if a fatal error within the VM is encountered.
// For this to work Duktape must have been configured with -DDUK_USE_FATAL_HANDLER=JSVMDuktapeFatalErrorHandler
// If Duktape is configured without said macro, it will use abort() instead.
// The version of Duktape embedded in JSVM has this macro already set.
void JSVMDuktapeFatalErrorHandler(void *, const char * msg)
{
    JSVMCallContext& jcc = JSVMCallContext::current();
    jcc.jniEnv()->ThrowNew(JSVMInternalError_Class, msg);
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
    JSVMPriv *priv = new JSVMPriv();

    if (priv->ctx != NULL) {
        env->SetLongField(instance, JSVM_hPriv, (jlong) priv);
    } else {
        delete priv;
        env->ThrowNew(JSVMInternalError_Class, "Failed to create JS heap");
    }
}

JNIEXPORT void JNICALL
Java_me_ntrrgc_jsvm_JSVM_finalizeNative(JNIEnv *env, jobject instance) {
    JSVM jsVM = (JSVM) instance;

    JSVMPriv* priv = JSVM_peekPriv(env, jsVM);
    delete priv;
}

JNIEXPORT jobject JNICALL
Java_me_ntrrgc_jsvm_JSVM_evaluateNative(JNIEnv *env, jobject instance, jstring code) {
    JSVM jsVM = (JSVM) instance;
    JSVMCallContext jcc(env, jsVM);
    JSVMPriv* priv = jcc.priv();

    return JSVMPriv_invokeSafe<JSValue>(jcc, [&jcc, code](duk_context *ctx, JSVMPriv *priv, JNIEnv* env) {
        String_pushJString(env, code, ctx);
        duk_eval(ctx);
        return JSValue_createFromStack(jcc, -1);
    });
}


JNIEXPORT int JNICALL
Java_me_ntrrgc_jsvm_JSVM_getStackFrameSizeNative(JNIEnv *env, jobject instance) {

    JSVM jsVM = (JSVM) instance;
    JSVMPriv* priv = JSVM_peekPriv(env, jsVM);
    duk_context* ctx = priv->ctx;

    return (int) duk_get_top(ctx);

}

JNIEXPORT int JNICALL
Java_me_ntrrgc_jsvm_JSVM_getStackSizeNative(JNIEnv *env, jobject instance) {

    JSVM jsVM = (JSVM) instance;
    JSVMPriv* priv = JSVM_peekPriv(env, jsVM);
    duk_context* ctx = priv->ctx;

    duk_push_int(ctx, 1);
    duk_pop(ctx);

    duk_idx_t topIndex = duk_get_top_index(ctx);
    return topIndex < 0 ? 0 : topIndex + 1;

}

JNIEXPORT jobject JNICALL
Java_me_ntrrgc_jsvm_JSVM_getGlobalScopeNative(JNIEnv *env, jobject instance) {

    JSVM jsVM = (JSVM) instance;
    JSVMCallContext jcc(env, jsVM);

    return JSVMPriv_invokeSafe<JSObject>(jcc, [&jcc](duk_context *ctx, JSVMPriv *priv, JNIEnv* env) {

        // Push global object
        duk_push_global_object(ctx);

        JSObject ret = priv->objectBook.exposeObject(jcc, -1);

        // Restore stack
        duk_pop(ctx); // object

        return ret;

    });
}

JNIEXPORT jobject JNICALL
Java_me_ntrrgc_jsvm_JSVM_newObjectNative(JNIEnv *env, jobject instance) {

    JSVM jsVM = (JSVM) instance;
    JSVMCallContext jcc(env, jsVM);

    return JSVMPriv_invokeSafe<JSObject>(jcc, [&jcc](duk_context *ctx, JSVMPriv *priv, JNIEnv* env) {

        // Push empty object
        duk_push_object(ctx);

        JSObject ret = priv->objectBook.exposeObject(jcc, -1);

        // Restore stack
        duk_pop(ctx); // object

        return ret;

    });
}

JNIEXPORT jobject JNICALL
Java_me_ntrrgc_jsvm_JSVM_newObjectNativeWithProto(JNIEnv *env, jobject instance, jobject proto_) {

    JSVM jsVM = (JSVM) instance;
    JSVMCallContext jcc(env, jsVM);
    JSObject proto = (JSObject) proto_;

    return JSVMPriv_invokeSafe<JSObject>(jcc, [&jcc, proto](duk_context *ctx, JSVMPriv *priv, JNIEnv* env) {

        if (proto == NULL) {
            // Push bare object (inherits from null)
            duk_push_bare_object(ctx);
        } else {
            // Push new object and set prototype
            duk_push_object(ctx);

            JSObject_push(env, proto);
            duk_set_prototype(ctx, -2);
        }

        JSObject ret = priv->objectBook.exposeObject(jcc, -1);

        // Restore stack
        duk_pop(ctx); // object

        return ret;

    });

}

JNIEXPORT jobject JNICALL
Java_me_ntrrgc_jsvm_JSVM_newFunctionNative(JNIEnv *env, jobject instance, jint callableHandle) {

    JSVM jsVM = (JSVM) instance;
    JSVMCallContext jcc(env, jsVM);

    return JSVMPriv_invokeSafe<JSObject>(jcc, [&jcc, callableHandle](duk_context *ctx, JSVMPriv *priv, JNIEnv* env) {

        // Create the trampoline JS function
        duk_push_global_stash(ctx);
        duk_get_prop_index(ctx, -1, GLOBAL_STASH_INDEX_TRAMPOLINE_FACTORY);
        duk_push_int(ctx, callableHandle);
        duk_call(ctx, 1);

        // Generate a finalizer for the trampoline JS function
        duk_get_prop_index(ctx, -2, GLOBAL_STASH_INDEX_FINALIZE_FACTORY);
        duk_push_int(ctx, callableHandle);
        duk_call(ctx, 1);

        // Attach the finalizer to the function
        duk_set_finalizer(ctx, -2);

        JSFunction retFunction = (JSFunction) priv->objectBook.exposeObject(jcc, -1);

        // Restore stack
        duk_pop_2(ctx); // retFunction, global stash

        return retFunction;

    });

}

}

/**
 * Called as a Duktape function, invokes a JSCallable and passes the result to Duktape through
 * the stack.
 */
static duk_ret_t JSVM_dukMakeNativeCall(duk_context *ctx) {
    JSVMCallContext& jcc = JSVMCallContext::current();
    JSVMPriv* priv = jcc.priv();
    JSVM jsVM = jcc.jsVM();
    JNIEnv* env = jcc.jniEnv();

    jint callableHandle = duk_require_int(ctx, 0);

    JSValue thisArg = JSValue_createFromStack(jcc, 1);

    duk_require_object_coercible(ctx, 2); // args
    unsigned int countArgs = (unsigned int) duk_get_length(ctx, 2);
    jobjectArray argsArray = env->NewObjectArray(countArgs, JSValue_Class, NULL);

    for (unsigned int i = 0; i < countArgs; ++i) {
        duk_get_prop_index(ctx, 2, i);
        JSValue jsValue = JSValue_createFromStack(jcc, -1);
        env->SetObjectArrayElement(argsArray, i, jsValue);
        env->DeleteLocalRef(jsValue);
        duk_pop(ctx);
    }

    // Call the Java callable
    JSValue jsCallableRet = (JSValue)
            env->CallObjectMethod(jsVM, JSVM_callNative, callableHandle, thisArg, argsArray);

    if (!env->ExceptionCheck()) {
        // Return the JSValue created in JSCallable to Duktape
        JSValue_push(env, jsCallableRet, ctx);

        env->DeleteLocalRef(jsCallableRet);
        env->DeleteLocalRef(argsArray);
        env->DeleteLocalRef(thisArg);

        return 1; // there is a return value at the top of the stack
    } else {
        // Java exception thrown. Must be a JSError.
        jthrowable jsError = env->ExceptionOccurred();

        // Handle the exception by throwing the attached JSObject.

        // Push the error value
        env->ExceptionClear();
        jsvm_assert(env->IsInstanceOf(jsError, JSError_Class));
        JSValue errorValue = (JSValue) env->GetObjectField(jsError, JSError_errorValue);
        JSValue_push(env, errorValue, ctx);

        env->DeleteLocalRef(errorValue);
        env->DeleteLocalRef(argsArray);
        env->DeleteLocalRef(thisArg);

        // Throw in Duktape
        duk_throw(ctx);
    }
}

/**
 * Called as a Duktape function to finalize JSCallable.
 */
static duk_ret_t JSVM_dukFinalizeNativeCallable(duk_context *ctx) {
    JSVMCallContext& jcc = JSVMCallContext::current();
    JSVM jsVM = jcc.jsVM();
    JNIEnv* env = jcc.jniEnv();

    jint callableHandle = duk_require_int(ctx, 0);

    env->CallVoidMethod(jsVM, JSVM_finalizeCallable, callableHandle);

    return 0; // return undefined
}

JSVMPriv::JSVMPriv()
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

    // Create the trampoline factory function used to create functions bound to Java callables.

    // It will be stored in the global stash, so that it's hidden from the user
    duk_push_global_stash(ctx);

    // The trampoline factory function needs to have the nativeCall function in order to work.
    // We can't use variables since they would be manipulable by the user, so we have to get a
    // little creative with closures... Trampoline factory factory!
    // (This one is used only once, it dies in the stack soon after)
    duk_eval_string(ctx,
            "(function _jsvmTrampolineFactoryFactory(nativeCall) {\n"
                "return function _jsvmTrampolineFactory(callableHandle) {\n"
                    "return function _jsvmTrampoline() {\n"
                        "return nativeCall(callableHandle, this, arguments);\n"
                    "}\n"
                "};\n"
            "})\n");
    // Pass the nativeCall function
    duk_push_c_function(ctx, JSVM_dukMakeNativeCall, 3);
    duk_call(ctx, 1);
    // _jsvmTrampolineFactory is now at the top of the stack, save it in the stash
    duk_put_prop_index(ctx, -2, GLOBAL_STASH_INDEX_TRAMPOLINE_FACTORY);

    // Initialize the callable finalizer factory function
    duk_eval_string(ctx,
            "(function _jsvmFinalizeCallableFactoryFactory(finalizeFn) {\n"
                "return function _jsvmFinalizeCallableFactory(callableHandle) {\n"
                    "return function _jsvmFinalizeCallable() {\n"
                        "finalizeFn(callableHandle);\n"
                    "};\n"
                "};\n"
            "})");
    duk_push_c_function(ctx, JSVM_dukFinalizeNativeCallable, 1);
    duk_call(ctx, 1);
    duk_put_prop_index(ctx, -2, GLOBAL_STASH_INDEX_FINALIZE_FACTORY);

    duk_pop(ctx); // pop the stash

    this->objectBook.lateInit(ctx);
}

JSVMPriv::~JSVMPriv() {
    duk_destroy_heap(ctx);
    ctx = NULL;
}
