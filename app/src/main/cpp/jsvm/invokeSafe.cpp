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