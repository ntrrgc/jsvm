//
// Created by ntrrgc on 1/15/17.
//

#include "JSValue.h"
#include <jsvm/JSVM.h>
using namespace jsvm;

duk_context *
jsvm::JSValue_getDukContext(JNIEnv *env, JSValue jsValue) {
    JSVM jsVM = (JSVM) env->GetObjectField(jsValue, JSValue_jsVM);
    return JSVM_getPriv(env, jsVM)->ctx;
}

JSValue
jsvm::JSValue_createFromStackTop(JNIEnv *env, JSVM jsVM) {
    // Create JSValue (get a pointer to the new instance, too!)
    JSValue jsValue = (JSValue) env->AllocObject(JSValue_Class);
    env->SetObjectField(jsValue, JSValue_jsVM, jsVM);

    duk_context *ctx = JSVM_getPriv(env, jsVM)->ctx;

    // Use the JSValue pointer as index in the global stash so that
    // the value is not GCed in JSland while JSValue is alive in Javaland.
    duk_push_global_stash(ctx); // stash
    duk_push_pointer(ctx, jsValue); // key
    duk_dup(ctx, -3); // value
    duk_put_prop(ctx, -3); // save in stash

    // Pop the stash
    duk_pop(ctx);

    return jsValue;
}

extern "C" {

JNIEXPORT jstring JNICALL
Java_me_ntrrgc_jsvm_JSValue_asString(JNIEnv *env, jobject instance) {
    JSValue jsValue = (JSValue) instance;

    duk_context *ctx = JSValue_getDukContext(env, jsValue);

    duk_push_global_stash(ctx);
    duk_push_pointer(ctx, jsValue);
    duk_get_prop(ctx, -2);

    jstring returnValue = env->NewStringUTF(duk_get_string(ctx, -1));

    duk_pop_2(ctx); // pop value and stash

    return returnValue;
}

};