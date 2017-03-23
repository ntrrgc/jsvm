//
// Created by ntrrgc on 1/15/17.
//

#include <jsvm/JSVM.h>
#include "JSValue.h"
#include "invokeSafe.h"
#include "JSObject.h"
#include "JSFunction.h"
#include <functional>

using namespace jsvm;

thread_local JSVMPriv* jsvm::thisThreadJSVMPriv = NULL;

/**
 * Get a JSVMPriv object with its JNIEnv and JSVM pointers
 * already set so that Duktape functions can be called safely.
 */
JSVMPriv *
jsvm::JSVM_getPriv(JNIEnv *env, JSVM jsVM) {
    JSVMPriv* priv = (JSVMPriv *) env->GetLongField(jsVM, JSVM_hPriv);
    priv->load(env, jsVM);
    return priv;
}

extern "C" {

extern int weakRefCount;
JNIEXPORT jint JNICALL
Java_me_ntrrgc_jsvm_JSVM_getWeakRefCount(JNIEnv *env, jobject instance) {

    return weakRefCount;

}

JNIEXPORT void JNICALL
Java_me_ntrrgc_jsvm_JSVM_initializeLibrary(JNIEnv *env, jclass type) {
    initClassesAndFields(env);
}

JNIEXPORT void JNICALL
Java_me_ntrrgc_jsvm_JSVM_destroyLibrary(JNIEnv *env, jclass type) {
    tearDownClassesAndFields(env);
}

JNIEXPORT void JNICALL
Java_me_ntrrgc_jsvm_JSVM_nativeInit(JNIEnv *env, jobject instance) {
    JSVM jsVM = (JSVM) instance;
    JSVMPriv *priv = new JSVMPriv(env, jsVM);

    if (priv->ctx != NULL) {
        env->SetLongField(instance, JSVM_hPriv, (jlong) priv);
    } else {
        env->ThrowNew(JSVMInternalError_Class, "Failed to create JS heap");
    }
}

JNIEXPORT void JNICALL
Java_me_ntrrgc_jsvm_JSVM_finalizeNative(JNIEnv *env, jobject instance) {
    JSVM jsVM = (JSVM) instance;

    JSVMPriv* priv = JSVM_getPriv(env, jsVM);
    delete priv;
}

JNIEXPORT jobject JNICALL
Java_me_ntrrgc_jsvm_JSVM_evaluateNative(JNIEnv *env, jobject instance, jstring code) {
    JSVM jsVM = (JSVM) instance;
    JSVMPriv* priv = JSVM_getPriv(env, jsVM);

    return JSVMPriv_invokeSafe<JSValue>(priv, [priv, env, jsVM, code](duk_context *ctx) {
        String_pushJString(env, code, ctx);
        duk_eval(ctx);
        return JSValue_createFromStack(env, jsVM, -1);
    });
}


JNIEXPORT int JNICALL
Java_me_ntrrgc_jsvm_JSVM_getStackFrameSizeNative(JNIEnv *env, jobject instance) {

    JSVM jsVM = (JSVM) instance;
    JSVMPriv* priv = JSVM_getPriv(env, jsVM);
    duk_context* ctx = priv->ctx;

    return (int) duk_get_top(ctx);

}

JNIEXPORT int JNICALL
Java_me_ntrrgc_jsvm_JSVM_getStackSizeNative(JNIEnv *env, jobject instance) {

    JSVM jsVM = (JSVM) instance;
    JSVMPriv* priv = JSVM_getPriv(env, jsVM);
    duk_context* ctx = priv->ctx;

    duk_idx_t topIndex = duk_get_top_index(ctx);
    return topIndex != DUK_INVALID_INDEX ? (int) topIndex - 1 : 0;

}

JNIEXPORT jobject JNICALL
Java_me_ntrrgc_jsvm_JSVM_getGlobalScopeNative(JNIEnv *env, jobject instance) {

    JSVM jsVM = (JSVM) instance;
    JSVMPriv* priv = JSVM_getPriv(env, jsVM);

    return JSVMPriv_invokeSafe<JSObject>(priv, [priv, env, jsVM] (duk_context *ctx) {

        // Push global object
        duk_push_global_object(ctx);

        JSObject ret = priv->objectBook.exposeObject(env, -1);

        // Restore stack
        duk_pop(ctx); // object

        return ret;

    });
}

JNIEXPORT jobject JNICALL
Java_me_ntrrgc_jsvm_JSVM_newObjectNative(JNIEnv *env, jobject instance) {

    JSVM jsVM = (JSVM) instance;
    JSVMPriv* priv = JSVM_getPriv(env, jsVM);

    return JSVMPriv_invokeSafe<JSObject>(priv, [priv, env, jsVM] (duk_context *ctx) {

        // Push empty object
        duk_push_object(ctx);

        JSObject ret = priv->objectBook.exposeObject(env, -1);

        // Restore stack
        duk_pop(ctx); // object

        return ret;

    });
}

JNIEXPORT jobject JNICALL
Java_me_ntrrgc_jsvm_JSVM_newObjectNativeWithProto(JNIEnv *env, jobject instance, jobject proto_) {

    JSVM jsVM = (JSVM) instance;
    JSVMPriv* priv = JSVM_getPriv(env, jsVM);
    JSObject proto = (JSObject) proto_;

    return JSVMPriv_invokeSafe<JSObject>(priv, [priv, env, jsVM, proto] (duk_context *ctx) {

        if (proto == NULL) {
            // Push bare object (inherits from null)
            duk_push_bare_object(ctx);
        } else {
            // Push new object and set prototype
            duk_push_object(ctx);

            JSObject_push(env, proto);
            duk_set_prototype(ctx, -2);
        }

        JSObject ret = priv->objectBook.exposeObject(env, -1);

        // Restore stack
        duk_pop(ctx); // object

        return ret;

    });

}

JNIEXPORT jobject JNICALL
Java_me_ntrrgc_jsvm_JSVM_newFunctionNative(JNIEnv *env, jobject instance, jint callableHandle) {

    JSVM jsVM = (JSVM) instance;
    JSVMPriv* priv = JSVM_getPriv(env, jsVM);

    return JSVMPriv_invokeSafe<JSObject>(priv, [priv, env, jsVM, callableHandle] (duk_context *ctx) {

        duk_push_global_stash(ctx);
        duk_get_prop_index(ctx, -1, GLOBAL_STASH_INDEX_TRAMPOLINE_FACTORY);
        duk_push_int(ctx, callableHandle);
        duk_call(ctx, 1);

        JSFunction retFunction = (JSFunction) priv->objectBook.exposeObject(env, -1);

        // Restore stack
        duk_pop_2(ctx); // retFunction, global stash

        return retFunction;

    });

}

}

/**
 * Called as a Duktape function, invokes a JSCallable and passes the result to Duktape through
 * the stack.
 */
static duk_ret_t JSVM_nativeCall(duk_context *ctx) {
    JSVMPriv* priv = thisThreadJSVMPriv;
    JSVM jsVM = priv->jsVM;
    JNIEnv* env = priv->env;

    jint callableHandle = duk_require_int(ctx, 0);

    JSValue thisArg = JSValue_createFromStack(env, jsVM, 1);

    duk_require_object_coercible(ctx, 2); // args
    unsigned int countArgs = (unsigned int) duk_get_length(ctx, 2);
    jobjectArray argsArray = env->NewObjectArray(countArgs, JSValue_Class, NULL);

    for (unsigned int i = 0; i < countArgs; ++i) {
        duk_get_prop_index(ctx, 2, i);
        JSValue jsValue = JSValue_createFromStack(env, jsVM, -1);
        env->SetObjectArrayElement(argsArray, i, jsValue);
        env->DeleteLocalRef(jsValue);
        duk_pop(ctx);
    }

    HandleAllocator handleAllocator = (HandleAllocator)
            env->GetObjectField(priv->jsVM, JSVM_callableAllocator);
    JSCallable callable = (JSCallable)
            env->CallObjectMethod(handleAllocator, HandleAllocator_get, callableHandle);

    // Call the Java callable
    JSValue jsCallableRet = (JSValue)
            env->CallObjectMethod(callable, JSCallable_call, argsArray, thisArg, jsVM);

    // Return the JSValue created in JSCallable to Duktape
    JSValue_push(env, jsCallableRet, ctx);

    env->DeleteLocalRef(jsCallableRet);
    env->DeleteLocalRef(callable);
    env->DeleteLocalRef(handleAllocator);
    env->DeleteLocalRef(argsArray);
    env->DeleteLocalRef(thisArg);

    return 1; // there is a return value at the top of the stack
}

JSVMPriv::JSVMPriv(JNIEnv *initialJNIEnv, JSVM initialJSVM)
        : env(initialJNIEnv), jsVM(initialJSVM)
{
    ctx = duk_create_heap(
            NULL, NULL, NULL, NULL, NULL
    );
    if (ctx == NULL) {
        return;
    }

    // Make global object accessible with "global".
    duk_push_global_object(ctx);
    duk_push_global_object(ctx);
    duk_put_prop_string(ctx, -2, "global");
    duk_pop(ctx);

    // Create the trampoline factory function used to create functions bound to Java callables.

    // It will be stored in the global stash, so that it's hidden from the user
    duk_push_global_stash(ctx);

    // The trampoline factory function needs to have the nativeCall function in order to work.
    // We can't use variables since they would be manipulable by the user, so we have to get a
    // little creative with closures... Trampoline factory factory!
    // (This one is used only once, it dies in the stack soon after)
    duk_eval_string(ctx,
            "(function _jsvmTrampolineFactoryFactory(nativeCall) {\n"
                "return function _jsvmTrampolineFactory(callableHandle) {\n"
                    "return function _jsvmTrampoline() {\n"
                        "return nativeCall(callableHandle, this, arguments);\n"
                    "}\n"
                "};\n"
            "})\n");
    // Pass the nativeCall function
    duk_push_c_function(ctx, JSVM_nativeCall, 3);
    duk_call(ctx, 1);
    // _jsvmTrampolineFactory is now at the top of the stack, save it in the stash
    duk_put_prop_index(ctx, -2, GLOBAL_STASH_INDEX_TRAMPOLINE_FACTORY);
    duk_pop(ctx); // pop the stash

    this->objectBook.lateInit(ctx, this);
}

JSVMPriv::~JSVMPriv() {
    duk_destroy_heap(ctx);
    ctx = NULL;
}