/* Copyright (c) the JSVM authors
 * https://github.com/ntrrgc/jsvm
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

//
// Created by ntrrgc on 1/15/17.
//

#include "JSObject.h"
#include "JSValue.h"
#include "invokeSafe.h"

using namespace jsvm;

void
jsvm::JSObject_push(JNIEnv* env, JSObject jsObject) {
    JSVM jsVM = (JSVM) env->GetObjectField(jsObject, JSObject_jsVM);
    JSVMCallContext jcc(env, jsVM);
    JSVMPriv* priv = jcc.priv();

    jint handle = env->GetIntField(jsObject, JSObject_handle);
    
    priv->objectBook.pushObjectWithHandle((ObjectBook::handle_t) handle);
}

extern "C" {

JNIEXPORT jdouble JNICALL
Java_me_ntrrgc_jsvm_JSVM_returnADouble(JNIEnv *env, jclass type) {

    return 1.1;

}

JNIEXPORT jstring JNICALL
Java_me_ntrrgc_jsvm_JSObject_toStringNative(JNIEnv *env, jobject instance, jobject jsVM_,
                                            jint handle) {

    JSVM jsVM = (JSVM) jsVM_;
    JSVMCallContext jcc(env, jsVM);
    JSVMPriv* priv = jcc.priv();

    return JSVMPriv_invokeSafe<jstring>(jcc, [handle] (duk_context *ctx, JSVMPriv *priv, JNIEnv* env) {

        // Retrieve the object
        priv->objectBook.pushObjectWithHandle((ObjectBook::handle_t) handle);

        // Read as string
        jstring ret = env->NewStringUTF(duk_to_string(ctx, -1));

        // Restore the stack
        duk_pop(ctx);

        return ret;

    });
}

JNIEXPORT jstring JNICALL
Java_me_ntrrgc_jsvm_JSObject_getClassNameNative(JNIEnv *env, jobject instance, jobject jsVM_,
                                                jint handle) {

    JSVM jsVM = (JSVM) jsVM_;
    JSVMCallContext jcc(env, jsVM);
    JSVMPriv* priv = jcc.priv();

    return JSVMPriv_invokeSafe<jstring>(jcc, [handle] (duk_context *ctx, JSVMPriv *priv, JNIEnv* env) {

        jstring ret = NULL;

        // Retrieve the object
        priv->objectBook.pushObjectWithHandle((ObjectBook::handle_t) handle);

        duk_get_prop_string(ctx, -1, "constructor");
        if (duk_is_object(ctx, -1)) {
            duk_get_prop_string(ctx, -1, "name");
            if (duk_is_string(ctx, -1)) {
                ret = env->NewStringUTF(duk_get_string(ctx, -1));
            }
            duk_pop(ctx);
        }

        // Restore the stack
        duk_pop_2(ctx);

        return ret;

    });
}

JNIEXPORT void JNICALL
Java_me_ntrrgc_jsvm_JSObject_finalizeNative(JNIEnv *env, jobject instance, jobject jsVM_,
                                            jint handle) {

    JSVM jsVM = (JSVM) jsVM_;
    JSVMCallContext jcc(env, jsVM);
    JSVMPriv* priv = jcc.priv();

    priv->objectBook.finalizeJSObjectWithHandle(jcc, (ObjectBook::handle_t) handle);

}


JNIEXPORT jobject JNICALL
Java_me_ntrrgc_jsvm_JSObject_getByKeyNative(JNIEnv *env, jobject instance, jobject jsVM_, jint handle,
                                            jstring key) {
    JSVM jsVM = (JSVM) jsVM_;
    JSVMCallContext jcc(env, jsVM);
    JSVMPriv* priv = jcc.priv();

    return JSVMPriv_invokeSafe<JSValue>(jcc, [&jcc, key, handle] (duk_context *ctx, JSVMPriv *priv, JNIEnv* env) {

        // Retrieve the object
        priv->objectBook.pushObjectWithHandle((ObjectBook::handle_t) handle);

        // Fetch property
        String_pushJString(env, key, ctx);
        duk_get_prop(ctx, -2);

        // Wrap in JSValue
        JSValue ret = JSValue_createFromStack(jcc, -1);

        // Restore stack
        duk_pop_2(ctx);

        return ret;

    });
}


JNIEXPORT jobject JNICALL
Java_me_ntrrgc_jsvm_JSObject_getByIndexNative(JNIEnv *env, jobject instance, jobject jsVM_,
                                              jint handle, jint index) {

    JSVM jsVM = (JSVM) jsVM_;
    JSVMCallContext jcc(env, jsVM);
    JSVMPriv* priv = jcc.priv();

    return JSVMPriv_invokeSafe<JSValue>(jcc, [&jcc, index, handle] (duk_context *ctx, JSVMPriv *priv, JNIEnv* env) {

        // Retrieve the object
        priv->objectBook.pushObjectWithHandle((ObjectBook::handle_t) handle);

        // Fetch property
        duk_get_prop_index(ctx, -1, (duk_uarridx_t) index);

        // Wrap in JSValue
        JSValue ret = JSValue_createFromStack(jcc, -1);

        // Restore stack
        duk_pop_2(ctx);

        return ret;

    });
}

JNIEXPORT void JNICALL
Java_me_ntrrgc_jsvm_JSObject_setByKeyNative(JNIEnv *env, jobject instance, jobject jsVM_,
                                            jint handle, jstring key, jobject value) {
    JSVM jsVM = (JSVM) jsVM_;
    JSVMCallContext jcc(env, jsVM);

    return JSVMPriv_invokeSafeVoid(jcc, [handle, key, value](duk_context *ctx, JSVMPriv *priv, JNIEnv* env) {

        // Retrieve the object
        priv->objectBook.pushObjectWithHandle((ObjectBook::handle_t) handle);

        // Set property
        String_pushJString(env, key, ctx);
        JSValue_push(env, (JSValue) value, ctx);
        duk_put_prop(ctx, -3);

        // Restore stack
        duk_pop(ctx); // object

    });
}

JNIEXPORT void JNICALL
Java_me_ntrrgc_jsvm_JSObject_setByIndexNative(JNIEnv *env, jobject instance, jobject jsVM_,
                                              jint handle, jint index, jobject value) {

    JSVM jsVM = (JSVM) jsVM_;
    JSVMCallContext jcc(env, jsVM);
    JSVMPriv* priv = jcc.priv();

    return JSVMPriv_invokeSafeVoid(jcc, [index, handle, value] (duk_context *ctx, JSVMPriv *priv, JNIEnv* env) {

        // Retrieve the object
        priv->objectBook.pushObjectWithHandle((ObjectBook::handle_t) handle);

        // Set property
        JSValue_push(env, (JSValue) value, ctx);
        duk_put_prop_index(ctx, -2, (duk_uarridx_t) index);

        // Restore stack
        duk_pop(ctx); // object

    });
}

JNIEXPORT jboolean JNICALL
Java_me_ntrrgc_jsvm_JSObject_containsByKeyNative(JNIEnv *env, jobject instance, jobject jsVM_,
                                                 jint handle, jstring key) {
    JSVM jsVM = (JSVM) jsVM_;
    JSVMCallContext jcc(env, jsVM);
    JSVMPriv* priv = jcc.priv();

    return JSVMPriv_invokeSafe<jboolean>(jcc, [key, handle] (duk_context *ctx, JSVMPriv *priv, JNIEnv* env) {

        // Retrieve the object
        priv->objectBook.pushObjectWithHandle((ObjectBook::handle_t) handle);

        // Search property
        String_pushJString(env, key, ctx);
        jboolean ret = (jboolean) duk_has_prop(ctx, -2);

        // Restore stack
        duk_pop(ctx); // object

        return ret;

    });
}

JNIEXPORT jboolean JNICALL
Java_me_ntrrgc_jsvm_JSObject_containsByIndexNative(JNIEnv *env, jobject instance, jobject jsVM_,
                                                   jint handle, jint index) {

    JSVM jsVM = (JSVM) jsVM_;
    JSVMCallContext jcc(env, jsVM);
    JSVMPriv* priv = jcc.priv();

    return JSVMPriv_invokeSafe<jboolean>(jcc, [index, handle] (duk_context *ctx, JSVMPriv *priv, JNIEnv* env) {

        // Retrieve the object
        priv->objectBook.pushObjectWithHandle((ObjectBook::handle_t) handle);

        // Search property
        jboolean ret = (jboolean) duk_has_prop_index(ctx, -1, (duk_uarridx_t) index);

        // Restore stack
        duk_pop(ctx); // object

        return ret;

    });
}

}