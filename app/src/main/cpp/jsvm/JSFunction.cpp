//
// Created by ntrrgc on 1/25/17.
//

#include "JSFunction.h"
#include <jsvm/invokeSafe.h>
#include <jsvm/JSValue.h>
using namespace jsvm;

extern "C" {

JNIEXPORT jobject JNICALL
Java_me_ntrrgc_jsvm_JSFunction_callNative(JNIEnv *env, jobject instance, jobject jsVM_, jint handle,
                                          jobject thisArg_, jobjectArray args) {

    JSVM jsVM = (JSVM) jsVM_;
    JSVMPriv* priv = JSVM_getPriv(env, jsVM);
    JSValue thisArg = (JSValue) thisArg_;

    return JSVMPriv_invokeSafe<JSValue>(priv, [priv, env, jsVM, handle, thisArg, args] (duk_context *ctx) {

        // Push the function
        priv->objectBook.pushObjectWithHandle((ObjectBook::handle_t) handle);

        // Push thisArg
        JSValue_push(env, thisArg, ctx);

        // Push arguments
        const jint numArgs = env->GetArrayLength(args);
        for (jint i = 0; i < numArgs; i++) {
            JSValue arg = (JSValue) env->GetObjectArrayElement(args, i);
            JSValue_push(env, arg, ctx);
        }

        // Invoke the function
        duk_call_method(ctx, numArgs);

        // Retrieve the return value
        JSValue ret = JSValue_createFromStack(env, jsVM, -1);

        // Restore stack
        duk_pop(ctx); // return value

        return ret;

    });

}

};