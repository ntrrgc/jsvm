//
// Created by ntrrgc on 1/15/17.
//

#pragma once

#include <stdafx.h>
#include "JSVM.h"

namespace jsvm {

    class _JSValue : public _jobject {
    };

    typedef _JSValue *JSValue;

    JSValue JSValue_createFromStackTop(JNIEnv *env, JSVM jsVM);

    duk_context *JSValue_getDukContext(JNIEnv *env, JSValue jsValue);

}