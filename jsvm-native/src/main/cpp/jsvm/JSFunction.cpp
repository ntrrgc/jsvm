/* Copyright (c) the JSVM authors
 * https://github.com/ntrrgc/jsvm
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

//
// Created by ntrrgc on 1/25/17.
//

#include "JSFunction.h"
#include <jsvm/invokeSafe.h>
#include <jsvm/JSValue.h>
using namespace jsvm;

extern "C" {

JNIEXPORT jobject JNICALL
Java_me_ntrrgc_jsvm_JSFunction_callNative(JNIEnv *env, jobject instance, jobject jsVM_, jint handle,
                                          jobject thisArg_, jobjectArray args) {

    JSVM jsVM = (JSVM) jsVM_;
    JSVMCallContext jcc(env, jsVM);
    JSValue thisArg = (JSValue) thisArg_;

    return JSVMPriv_invokeSafe<JSValue>(jcc, [&jcc, handle, thisArg, args](duk_context *ctx, JSVMPriv *priv, JNIEnv* env) {

        // Push the function
        priv->objectBook.pushObjectWithHandle((ObjectBook::handle_t) handle);

        // Push thisArg
        JSValue_push(env, thisArg, ctx);

        // Push arguments
        const jint numArgs = env->GetArrayLength(args);
        for (jint i = 0; i < numArgs; i++) {
            JSValue arg = (JSValue) env->GetObjectArrayElement(args, i);
            JSValue_push(env, arg, ctx);
        }

        // Invoke the function
        duk_call_method(ctx, numArgs);

        // Retrieve the return value
        JSValue ret = JSValue_createFromStack(jcc, -1);

        // Restore stack
        duk_pop(ctx); // return value

        return ret;

    });

}

JNIEXPORT jobject JNICALL
Java_me_ntrrgc_jsvm_JSFunction_callNewNative(JNIEnv *env, jobject instance, jobject jsVM_, jint handle,
                                             jobjectArray args) {

    JSVM jsVM = (JSVM) jsVM_;
    JSVMCallContext jcc(env, jsVM);
    JSVMPriv* priv = jcc.priv();

    return JSVMPriv_invokeSafe<JSValue>(jcc, [&jcc, handle, args](duk_context *ctx, JSVMPriv *priv, JNIEnv* env) {

        // Push the function
        priv->objectBook.pushObjectWithHandle((ObjectBook::handle_t) handle);

        // Push arguments
        const jint numArgs = env->GetArrayLength(args);
        for (jint i = 0; i < numArgs; i++) {
            JSValue arg = (JSValue) env->GetObjectArrayElement(args, i);
            JSValue_push(env, arg, ctx);
        }

        // Invoke the function with new
        duk_new(ctx, numArgs);

        // Retrieve the return value
        JSValue ret = JSValue_createFromStack(jcc, -1);

        // Restore stack
        duk_pop(ctx); // return value

        return ret;

    });

}

};