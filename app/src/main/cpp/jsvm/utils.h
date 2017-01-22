//
// Created by ntrrgc on 1/15/17.
//

#include <stdafx.h>

namespace jsvm {
    jstring String_createFromStack(JNIEnv *env, duk_context *ctx, int stackPosition);
    void String_pushUTF16(duk_context *ctx, const jchar* utf16String, jsize codePointLength);
    void String_pushJString(JNIEnv *env, jstring string, duk_context *ctx);
}