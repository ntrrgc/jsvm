/* Copyright (c) the JSVM authors
 * https://github.com/ntrrgc/jsvm
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

//
// Created by ntrrgc on 1/15/17.
//

#include <stdafx.h>

namespace jsvm {
    jstring String_createFromStack(JNIEnv *env, duk_context *ctx, int stackPosition);
    void String_pushUTF16(duk_context *ctx, const jchar* utf16String, jsize codePointLength);
    void String_pushJString(JNIEnv *env, jstring string, duk_context *ctx);

    /**
     * Used in asserts, for instance:
     * jsvm_assert(checkLocalRefIsNotNullAndRelease(env->CallObjectMethod(...)));
     *
     * Checks if the passed local reference to object is not null.
     * If it is not null, it deletes it before returning.
     *
     * @param localRef
     * @return True if the reference was not null.
     */
    bool checkLocalRefIsNotNullAndRelease(JNIEnv* env, jobject localRef);
}

