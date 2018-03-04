/* Copyright (c) the JSVM authors
 * https://github.com/ntrrgc/jsvm
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

//
// Created by ntrrgc on 1/23/17.
//
#include <jsvm/invokeSafe.h>

using namespace jsvm;

duk_ret_t
jsvm::_duk_raw_function_call(duk_context *ctx, void *udata) {
    duk_safe_call_std_function *function = (duk_safe_call_std_function *) udata;
    return (*function)(ctx);
}

void ::jsvm::JSVMPriv_invokeSafeVoid(JSVMPriv *priv, std::function<void(duk_context *)> callback) {
    duk_context *ctx = priv->ctx;

    // Exceptions may be anything, so we have to use pointers to ensure virtual dispatch.
    std::unique_ptr<JavaException> capturedJavaException;

    duk_safe_call_std_function wrappedCallback = [&callback, &capturedJavaException](
            duk_context *receivedCtx) {
        try {
            callback(receivedCtx);
        } catch (JavaException &javaException) {
            capturedJavaException = javaException.moveClone();
        }
        return 0;
    };

    int dukExecRet = duk_safe_call(priv->ctx, _duk_raw_function_call, &wrappedCallback, 0, 1);

    try {
        if (capturedJavaException) {
            // A wrapped Java exception has been throw in C++, but not propagated yet to Java!
            // Propagate it now:
            JNIEnv *env = priv->env;
            jsvm_assert(env->ExceptionCheck() == JNI_FALSE);
            capturedJavaException->propagateToJava(env);

            duk_pop(priv->ctx); // returned undefined
        } else if (dukExecRet == DUK_EXEC_ERROR) {
            // A JavaScript error has been thrown, wrap into a JSError class
            // and propagate it to Java.
            JNIEnv *env = priv->env;
            jsvm_assert(env->ExceptionCheck() == JNI_FALSE);

            JSValue errorValue = JSValue_createFromStack(env, priv->jsVM, -1);
            JSError(env, errorValue).propagateToJava(env);

            duk_pop(priv->ctx); // returned error instance
        } else {
            // Successful execution. Return the return value of the provided callback.

            duk_pop(priv->ctx); // returned undefined
        }
    } catch (JavaException& error) {
        // An error occurred while trying to return, try to throw it.
        JNIEnv *env = priv->env;
        if (!env->ExceptionCheck()) {
            error.propagateToJava(env);
        }
    }
}