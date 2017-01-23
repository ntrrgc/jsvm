//
// Created by ntrrgc on 1/15/17.
//

#include "JSObject.h"
#include "JSValue.h"
#include "invokeSafe.h"

using namespace jsvm;

JSObject
jsvm::JSObject_createFromStack(JNIEnv *env, JSVM jsVM, int stackPosition) {
    JSVMPriv* priv = JSVM_getPriv(env, jsVM);

    ObjectBook::handle_t handle = priv->objectBook.storeStackValue(stackPosition);

    JSObject jsObject = (JSObject) env->NewObject(JSObject_Class, JSObject_ctor);
    env->SetObjectField(jsObject, JSObject_jsVM, jsVM);
    env->SetIntField(jsObject, JSObject_handle, handle);

    return jsObject;
}

void
jsvm::JSObject_push(JNIEnv* env, JSObject jsObject) {
    JSVM jsVM = (JSVM) env->GetObjectField(jsObject, JSObject_jsVM);
    JSVMPriv* priv = JSVM_getPriv(env, jsVM);

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
    JSVMPriv* priv = JSVM_getPriv(env, jsVM);

    return JSVMPriv_invokeSafe<jstring>(priv, [priv, env, jsVM, handle] (duk_context *ctx) {

        // Retrieve the object
        priv->objectBook.pushObjectWithHandle((ObjectBook::handle_t) handle);

        // Read as string
        jstring ret = env->NewStringUTF(duk_to_string(ctx, -1));

        // Restore the stack
        duk_pop(ctx);

        return ret;

    });
}

JNIEXPORT void JNICALL
Java_me_ntrrgc_jsvm_JSObject_finalizeNative(JNIEnv *env, jobject instance, jobject jsVM_,
                                            jint handle) {

    JSVM jsVM = (JSVM) jsVM_;
    JSVMPriv* priv = JSVM_getPriv(env, jsVM);

    priv->objectBook.removeObjectWithHandle((ObjectBook::handle_t) handle);

}


JNIEXPORT jobject JNICALL
Java_me_ntrrgc_jsvm_JSObject_getByKeyNative(JNIEnv *env, jobject instance, jobject jsVM_, jint handle,
                                            jstring key) {
    JSVM jsVM = (JSVM) jsVM_;
    JSVMPriv* priv = JSVM_getPriv(env, jsVM);

    return JSVMPriv_invokeSafe<JSValue>(priv, [priv, env, jsVM, key, handle] (duk_context *ctx) {

        // Retrieve the object
        priv->objectBook.pushObjectWithHandle((ObjectBook::handle_t) handle);

        // Fetch property
        String_pushJString(env, key, ctx);
        duk_get_prop(ctx, -2);

        // Wrap in JSValue
        JSValue ret = JSValue_createFromStack(env, jsVM, -1);

        // Restore stack
        duk_pop_2(ctx);

        return ret;

    });
}


JNIEXPORT jobject JNICALL
Java_me_ntrrgc_jsvm_JSObject_getByIndexNative(JNIEnv *env, jobject instance, jobject jsVM_,
                                              jint handle, jint index) {

    JSVM jsVM = (JSVM) jsVM_;
    JSVMPriv* priv = JSVM_getPriv(env, jsVM);

    return JSVMPriv_invokeSafe<JSValue>(priv, [priv, env, jsVM, index, handle] (duk_context *ctx) {

        // Retrieve the object
        priv->objectBook.pushObjectWithHandle((ObjectBook::handle_t) handle);

        // Fetch property
        duk_get_prop_index(ctx, -1, (duk_uarridx_t) index);

        // Wrap in JSValue
        JSValue ret = JSValue_createFromStack(env, jsVM, -1);

        // Restore stack
        duk_pop_2(ctx);

        return ret;

    });
}

JNIEXPORT void JNICALL
Java_me_ntrrgc_jsvm_JSObject_setByKeyNative(JNIEnv *env, jobject instance, jobject jsVM_,
                                            jint handle, jstring key, jobject value) {
    JSVM jsVM = (JSVM) jsVM_;
    JSVMPriv* priv = JSVM_getPriv(env, jsVM);

    return JSVMPriv_invokeSafeVoid(priv, [priv, env, jsVM, key, handle, value] (duk_context *ctx) {

        // Retrieve the object
        priv->objectBook.pushObjectWithHandle((ObjectBook::handle_t) handle);

        // Set property
        String_pushJString(env, key, ctx);
        JSValue_push(env, (JSValue) value, ctx);
        duk_put_prop(ctx, -3);

        // Restore stack
        duk_pop(ctx); // objectBook

    });
}

JNIEXPORT void JNICALL
Java_me_ntrrgc_jsvm_JSObject_setByIndexNative(JNIEnv *env, jobject instance, jobject jsVM_,
                                              jint handle, jint index, jobject value) {

    JSVM jsVM = (JSVM) jsVM_;
    JSVMPriv* priv = JSVM_getPriv(env, jsVM);

    return JSVMPriv_invokeSafeVoid(priv, [priv, env, jsVM, index, handle, value] (duk_context *ctx) {

        // Retrieve the object
        priv->objectBook.pushObjectWithHandle((ObjectBook::handle_t) handle);

        // Set property
        JSValue_push(env, (JSValue) value, ctx);
        duk_put_prop_index(ctx, -2, (duk_uarridx_t) index);

        // Restore stack
        duk_pop(ctx); // objectBook

    });
}

JNIEXPORT jboolean JNICALL
Java_me_ntrrgc_jsvm_JSObject_containsByKeyNative(JNIEnv *env, jobject instance, jobject jsVM_,
                                                 jint handle, jstring key) {
    JSVM jsVM = (JSVM) jsVM_;
    JSVMPriv* priv = JSVM_getPriv(env, jsVM);

    return JSVMPriv_invokeSafe<jboolean>(priv, [priv, env, jsVM, key, handle] (duk_context *ctx) {

        // Retrieve the object
        priv->objectBook.pushObjectWithHandle((ObjectBook::handle_t) handle);

        // Search property
        String_pushJString(env, key, ctx);
        jboolean ret = (jboolean) duk_has_prop(ctx, -2);

        // Restore stack
        duk_pop(ctx); // objectBook

        return ret;

    });
}

JNIEXPORT jboolean JNICALL
Java_me_ntrrgc_jsvm_JSObject_containsByIndexNative(JNIEnv *env, jobject instance, jobject jsVM_,
                                                   jint handle, jint index) {

    JSVM jsVM = (JSVM) jsVM_;
    JSVMPriv* priv = JSVM_getPriv(env, jsVM);

    return JSVMPriv_invokeSafe<jboolean>(priv, [priv, env, jsVM, index, handle] (duk_context *ctx) {

        // Retrieve the object
        priv->objectBook.pushObjectWithHandle((ObjectBook::handle_t) handle);

        // Search property
        jboolean ret = (jboolean) duk_has_prop_index(ctx, -1, (duk_uarridx_t) index);

        // Restore stack
        duk_pop(ctx); // objectBook

        return ret;

    });
}

}