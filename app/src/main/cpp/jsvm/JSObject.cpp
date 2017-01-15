//
// Created by ntrrgc on 1/15/17.
//

#include "JSObject.h"

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

}