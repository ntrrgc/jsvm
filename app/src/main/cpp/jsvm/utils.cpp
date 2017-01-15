//
// Created by ntrrgc on 1/15/17.
//


#include <jni.h>
#include <duktape/duk_config.h>
#include "utils.h"

jstring
jsvm::String_createFromStackTop(JNIEnv *env, duk_context *ctx) {
    return env->NewStringUTF(duk_get_string(ctx, -1));
}

