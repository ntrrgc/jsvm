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

    void JSVMPriv_invokeSafeVoid(JSVMPriv *priv, std::function<void(duk_context *)> callback);

    template<typename T>
    T
    JSVMPriv_invokeSafe(JSVMPriv *priv, std::function<T(duk_context *)> callback) {
        duk_context *ctx = priv->ctx;

        // T has to be an assignable, concrete type (usually a pointer to a Java object)
        T ret;
        // Exceptions may be anything, so we have to use pointers to ensure virtual dispatch.
        std::unique_ptr<JavaException> capturedJavaException;

        duk_safe_call_std_function wrappedCallback = [&ret, &callback, &capturedJavaException](
                duk_context *receivedCtx) {
            try {
                ret = callback(receivedCtx);
            } catch (JavaException &javaException) {
                capturedJavaException = javaException.moveClone();
            }
            return 0;
        };

        int dukExecRet = duk_safe_call(priv->ctx, _duk_raw_function_call, &wrappedCallback, 0, 1);

        if (capturedJavaException) {
            // A wrapped Java exception has been throw in C++, but not propagated yet to Java!
            // Propagate it now:
            JNIEnv *env = priv->env;
            jsvm_assert(env->ExceptionCheck() == JNI_FALSE);
            capturedJavaException->propagateToJava(env);

            duk_pop(priv->ctx); // returned undefined
            return static_cast<T>(NULL);
        } else if (dukExecRet == DUK_EXEC_ERROR) {
            // A JavaScript error has been thrown, wrap into a JSError class
            // and propagate it to Java.
            JNIEnv *env = priv->env;
            jsvm_assert(env->ExceptionCheck() == JNI_FALSE);

            JSValue errorValue = JSValue_createFromStack(env, priv->jsVM, -1);
            JSError(env, errorValue).propagateToJava(env);

            duk_pop(priv->ctx); // returned error instance
            return static_cast<T>(NULL);
        } else {
            // Successful execution. Return the return value of the provided callback.

            duk_pop(priv->ctx); // returned undefined
            return ret;
        }
    }

}

#endif //JSVM_INVOKESAFE_H
