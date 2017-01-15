//
// Created by ntrrgc on 1/15/17.
//

#pragma once

#include <stdafx.h>

namespace jsvm {

    class _JSVM : public _jobject {};
    typedef _JSVM *JSVM;

    duk_context *JSVM_getDukContext(JNIEnv *env, JSVM jsVM);

}