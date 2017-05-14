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

    JSValue JSValue_createFromStack(JNIEnv *env, JSVM jsVM, int stackPosition);

    void JSValue_push(JNIEnv *env, JSValue jsValue, duk_context *ctx);

}