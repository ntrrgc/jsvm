//
// Created by ntrrgc on 1/15/17.
//

#pragma once

#include <stdafx.h>
#include <jsvm/JSVM.h>

namespace jsvm {

    class _JSValue : public _jobject {
    };

    typedef _JSValue *JSValue;

    Result<JSValue> JSValue_createFromStack(JNIEnv *env, JSVM jsVM, int stackPosition);

    may_throw JSValue_push(JNIEnv *env, JSValue jsValue, duk_context *ctx);

}