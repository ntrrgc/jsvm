//
// Created by ntrrgc on 1/15/17.
//

#pragma once
#include <stdafx.h>
#include <jsvm/JSVM.h>

namespace jsvm {

    class _JSObject : public _jobject {
    };

    typedef _JSObject *JSObject;

    JSObject JSObject_createFromStack(JNIEnv *env, JSVM jsVM, int stackPosition);

    void JSObject_push(JNIEnv* env, JSObject jsObject);

}
