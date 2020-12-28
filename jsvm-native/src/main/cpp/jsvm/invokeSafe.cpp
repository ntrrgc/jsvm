/* Copyright (c) the JSVM authors
 * https://github.com/ntrrgc/jsvm
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

//
// Created by ntrrgc on 1/23/17.
//
#include <jsvm/invokeSafe.h>

using namespace jsvm;

duk_ret_t
jsvm::_duk_raw_function_call(duk_context *ctx, void *udata) {
    duk_safe_call_std_function *function = (duk_safe_call_std_function *) udata;
    return (*function)(ctx);
}