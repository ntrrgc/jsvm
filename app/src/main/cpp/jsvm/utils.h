//
// Created by ntrrgc on 1/15/17.
//

#include <stdafx.h>

namespace jsvm {
    Result <jstring> String_createFromStackTop(JNIEnv *env, duk_context *ctx);
    void String_pushUTF16(duk_context *ctx, const jchar* utf16String, jsize codePointLength);
    void String_pushJString(JNIEnv *env, jstring string, duk_context *ctx);
}