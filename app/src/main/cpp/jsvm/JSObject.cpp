//
// Created by ntrrgc on 1/15/17.
//

#include "JSObject.h"
#include "JSValue.h"

using namespace jsvm;

JSObject
jsvm::JSObject_createFromStackTop(JNIEnv* env, JSVM jsVM){
    JSVMPriv* priv = JSVM_getPriv(env, jsVM);

    ObjectBook::handle_t handle = priv->objectBook.storeStackTop();

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

JNIEXPORT jstring JNICALL
Java_me_ntrrgc_jsvm_JSObject_toStringNative(JNIEnv *env, jobject instance, jlong hPriv,
                                            jint handle) {

    JSVMPriv* priv = (JSVMPriv *) hPriv;
    duk_context* ctx = priv->ctx;

    // Retrieve the object
    priv->objectBook.pushObjectWithHandle((ObjectBook::handle_t) handle);

    // Read as string
    jstring ret = env->NewStringUTF(duk_to_string(ctx, -1));

    // Restore the stack
    duk_pop(ctx);

    return ret;
}

JNIEXPORT void JNICALL
Java_me_ntrrgc_jsvm_JSObject_finalizeNative(JNIEnv *env, jobject instance, jlong hPriv,
                                            jint handle) {

    JSVMPriv* priv = (JSVMPriv *) hPriv;

    priv->objectBook.removeObjectWithHandle((ObjectBook::handle_t) handle);

}


JNIEXPORT jobject JNICALL
Java_me_ntrrgc_jsvm_JSObject_getByKeyNative(JNIEnv *env, jobject instance, jobject jsVM_, jint handle,
                                            jstring key_) {
    const char *key = env->GetStringUTFChars(key_, 0);

    JSVM jsVM = (JSVM) jsVM_;
    JSVMPriv* priv = JSVM_getPriv(env, jsVM);
    duk_context* ctx = priv->ctx;

    // Retrieve the object
    priv->objectBook.pushObjectWithHandle((ObjectBook::handle_t) handle);

    // Fetch property
    duk_get_prop_string(ctx, -1, key);

    // Wrap in JSValue
    JSValue ret = JSValue_createFromStackTop(env, jsVM);

    // Restore stack
    duk_pop_2(ctx);

    env->ReleaseStringUTFChars(key_, key);
    return ret;
}


JNIEXPORT jobject JNICALL
Java_me_ntrrgc_jsvm_JSObject_getByIndexNative(JNIEnv *env, jobject instance, jobject jsVM_,
                                              jint handle, jint index) {

    JSVM jsVM = (JSVM) jsVM_;
    JSVMPriv* priv = JSVM_getPriv(env, jsVM);
    duk_context* ctx = priv->ctx;

    // Retrieve the object
    priv->objectBook.pushObjectWithHandle((ObjectBook::handle_t) handle);

    // Fetch property
    duk_get_prop_index(ctx, -1, (duk_uarridx_t) index);

    // Wrap in JSValue
    JSValue ret = JSValue_createFromStackTop(env, jsVM);

    // Restore stack
    duk_pop_2(ctx);

    return ret;

}

JNIEXPORT void JNICALL
Java_me_ntrrgc_jsvm_JSObject_setByKeyNative(JNIEnv *env, jobject instance, jobject jsVM_,
                                            jint handle, jstring key_, jobject value) {
    const char *key = env->GetStringUTFChars(key_, 0);

    JSVM jsVM = (JSVM) jsVM_;
    JSVMPriv* priv = JSVM_getPriv(env, jsVM);
    duk_context* ctx = priv->ctx;

    // Retrieve the object
    priv->objectBook.pushObjectWithHandle((ObjectBook::handle_t) handle);

    // Set property
    if (THREW_EXCEPTION == JSValue_push(env, (JSValue) value, ctx)) {
        goto release;
    }
    duk_put_prop_string(ctx, -2, key);

release:
    // Restore stack
    duk_pop(ctx); // objectBook

    env->ReleaseStringUTFChars(key_, key);
}

JNIEXPORT void JNICALL
Java_me_ntrrgc_jsvm_JSObject_setByIndexNative(JNIEnv *env, jobject instance, jobject jsVM_,
                                              jint handle, jint index, jobject value) {

    JSVM jsVM = (JSVM) jsVM_;
    JSVMPriv* priv = JSVM_getPriv(env, jsVM);
    duk_context* ctx = priv->ctx;

    // Retrieve the object
    priv->objectBook.pushObjectWithHandle((ObjectBook::handle_t) handle);

    // Set property
    if (THREW_EXCEPTION == JSValue_push(env, (JSValue) value, ctx)) {
        goto release;
    }
    duk_put_prop_index(ctx, -2, (duk_uarridx_t) index);

release:
    // Restore stack
    duk_pop(ctx); // objectBook
}

JNIEXPORT jboolean JNICALL
Java_me_ntrrgc_jsvm_JSObject_containsByKeyNative(JNIEnv *env, jobject instance, jobject jsVM_,
                                                 jint handle, jstring key_) {
    const char *key = env->GetStringUTFChars(key_, 0);

    JSVM jsVM = (JSVM) jsVM_;
    JSVMPriv* priv = JSVM_getPriv(env, jsVM);
    duk_context* ctx = priv->ctx;

    // Retrieve the object
    priv->objectBook.pushObjectWithHandle((ObjectBook::handle_t) handle);

    // Search property
    jboolean ret = (jboolean) duk_has_prop_string(ctx, -1, key);

    // Restore stack
    duk_pop(ctx); // objectBook

    env->ReleaseStringUTFChars(key_, key);
    return ret;
}


JNIEXPORT jboolean JNICALL
Java_me_ntrrgc_jsvm_JSObject_containsByIndexNative(JNIEnv *env, jobject instance, jobject jsVM_,
                                                   jint handle, jint index) {

    JSVM jsVM = (JSVM) jsVM_;
    JSVMPriv* priv = JSVM_getPriv(env, jsVM);
    duk_context* ctx = priv->ctx;

    // Retrieve the object
    priv->objectBook.pushObjectWithHandle((ObjectBook::handle_t) handle);

    // Search property
    jboolean ret = (jboolean) duk_has_prop_index(ctx, -1, index);

    // Restore stack
    duk_pop(ctx); // objectBook

    return ret;
}

}