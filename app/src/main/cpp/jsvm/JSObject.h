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

    JSObject JSObject_createFromStackTop(JNIEnv *env, JSVM jsVM);

}
