/* Copyright (c) the JSVM authors
 * https://github.com/ntrrgc/jsvm
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

//
// Created by ntrrgc on 1/23/17.
//

#ifndef JSVM_INVOKESAFE_H
#define JSVM_INVOKESAFE_H

#include <stdafx.h>
#include <jsvm/JSVM.h>
#include <jsvm/JSValue.h>
#include <type_traits>

namespace jsvm {

    typedef std::function<duk_ret_t(duk_context *)> duk_safe_call_std_function;

    duk_ret_t
    _duk_raw_function_call(duk_context *ctx, void *udata);

    template<typename T>
    T
    JSVMPriv_invokeSafe(JSVMCallContext& jcc, std::function<T(duk_context *, JSVMPriv *, JNIEnv *)> callback) {
        JSVMPriv* priv = jcc.priv();

        // T has to be an assignable, concrete type (usually a pointer to a Java object)
        T ret;
        // Exceptions may be anything, so we have to use pointers to ensure virtual dispatch.
        std::unique_ptr<JavaException> capturedJavaException;

        duk_safe_call_std_function wrappedCallback = [&ret, &callback, &capturedJavaException, &jcc](
                duk_context *receivedCtx) {
            try {
                ret = callback(receivedCtx, jcc.priv(), jcc.jniEnv());
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
                JNIEnv *env = jcc.jniEnv();
                jsvm_assert(env->ExceptionCheck() == JNI_FALSE);
                capturedJavaException->propagateToJava(env);

                duk_pop(priv->ctx); // returned undefined
                return static_cast<T>(NULL);
            } else if (dukExecRet == DUK_EXEC_ERROR) {
                // A JavaScript error has been thrown, wrap into a JSError class
                // and propagate it to Java.
                JNIEnv *env = jcc.jniEnv();
                jsvm_assert(env->ExceptionCheck() == JNI_FALSE);

                JSValue errorValue = JSValue_createFromStack(jcc, -1);
                JSError(env, errorValue).propagateToJava(env);

                duk_pop(priv->ctx); // returned error instance
                return static_cast<T>(NULL);
            } else {
                // Successful execution. Return the return value of the provided callback.

                duk_pop(priv->ctx); // returned undefined
                return ret;
            }
        } catch (JavaException& error) {
            // An error occurred while trying to return, try to throw it.
            JNIEnv *env = jcc.jniEnv();
            if (!env->ExceptionCheck()) {
                error.propagateToJava(env);
            }
            return static_cast<T>(0);
        }
    }

    template<>
    void inline
    JSVMPriv_invokeSafe(JSVMCallContext& jcc, std::function<void(duk_context *, JSVMPriv *, JNIEnv *)> callback) {
        auto wrappedCallback = [&callback](duk_context * ctx, JSVMPriv * priv, JNIEnv * env) -> void* {
            callback(ctx, priv, env);
            return NULL;
        };
        JSVMPriv_invokeSafe<void*>(jcc, wrappedCallback);
    }

}

#endif //JSVM_INVOKESAFE_H
