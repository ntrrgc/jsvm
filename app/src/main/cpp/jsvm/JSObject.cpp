//
// Created by ntrrgc on 1/15/17.
//

#include "JSObject.h"
#include "JSVM.h"

using namespace jsvm;

JNIEXPORT jstring JNICALL
Java_me_ntrrgc_jsvm_JSObject_toStringNative(JNIEnv *env, jobject instance, jlong hPriv,
                                            jint jsHandle) {

    JSVMPriv* priv = (JSVMPriv *) hPriv;
    duk_context* ctx = priv->ctx;

    // Retrieve the object
    priv->objectBook.pushObjectWithHandle((ObjectBook::handle_t) jsHandle);

    // Read as string
    jstring ret = env->NewStringUTF(duk_to_string(ctx, -1));

    // Restore the stack
    duk_pop(ctx);

    return ret;
}