/* Copyright (c) the JSVM authors
 * https://github.com/ntrrgc/jsvm
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

//
// Created by ntrrgc on 1/25/17.
//

#ifndef JSVM_JSFUNCTION_H
#define JSVM_JSFUNCTION_H

#include <stdafx.h>
#include <jsvm/JSObject.h>

namespace jsvm {

    class _JSFunction : public _JSObject {
    };

    typedef _JSFunction *JSFunction;

    JSFunction JSFunction_createFromStack(JNIEnv *env, JSVM jsVM, int stackPosition);

}

#endif //JSVM_JSFUNCTION_H
