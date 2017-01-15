//
// Created by ntrrgc on 1/15/17.
//
#ifndef JSVM_JSVM
#define JSVM_JSVM
#pragma once

#include <stdafx.h>
#include <stack>
#include "ObjectBook.h"

namespace jsvm {

    class _JSVM : public _jobject {};
    typedef _JSVM* JSVM;

    /**
     * This class stores and manages JSVM internals.
     *
     * A pointer to this class is stored in JSVM::hPriv.
     */
    struct JSVMPriv {
        JSVMPriv();
        ~JSVMPriv();

        duk_context *ctx;
        ObjectBook objectBook;
    };

    JSVMPriv * JSVM_getPriv(JNIEnv *env, JSVM jsVM);

}

#endif