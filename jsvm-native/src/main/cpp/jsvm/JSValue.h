/* Copyright (c) the JSVM authors
 * https://github.com/ntrrgc/jsvm
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

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