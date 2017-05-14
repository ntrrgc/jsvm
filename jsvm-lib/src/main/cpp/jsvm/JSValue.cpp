//
// Created by ntrrgc on 1/15/17.
//

#include "JSValue.h"
#include "JSObject.h"
#include "JSFunction.h"
#include <jsvm/JSVM.h>
using namespace jsvm;

#define JSVALUE_TYPE_UNSUPPORTED 0
#define JSVALUE_TYPE_UNDEFINED 1
#define JSVALUE_TYPE_NULL 2
#define JSVALUE_TYPE_BOOLEAN 3
#define JSVALUE_TYPE_NUMBER 4
#define JSVALUE_TYPE_STRING 5
#define JSVALUE_TYPE_OBJECT 6

/**
 * Retrieve the top of the stack and wrap it into a JSValue.
 *
 * May throw if the stack top contains a corrupted string
 * (corrupted strings can only be created in C++ code).
 */
JSValue
jsvm::JSValue_createFromStack(JNIEnv *env, JSVM jsVM, int stackPosition) {
    JSVMPriv *priv = JSVM_getPriv(env, jsVM);
    duk_context *ctx = priv->ctx;

    duk_int_t dukType = duk_get_type(ctx, stackPosition);

    int valueType;
    jobject boxedValue = NULL;
    switch (dukType) {
        case DUK_TYPE_UNDEFINED:
            valueType = JSVALUE_TYPE_UNDEFINED;
            break;
        case DUK_TYPE_NULL:
            valueType = JSVALUE_TYPE_NULL;
            break;
        case DUK_TYPE_BOOLEAN:
            valueType = JSVALUE_TYPE_BOOLEAN;
            boxedValue = env->NewObject(Boolean_Class, Boolean_ctor,
                                        duk_get_boolean(ctx, stackPosition));
            break;
        case DUK_TYPE_NUMBER:
            valueType = JSVALUE_TYPE_NUMBER;
            boxedValue = env->NewObject(Double_Class, Double_ctor,
                                        duk_get_number(ctx, stackPosition));
            break;
        case DUK_TYPE_STRING: {
            valueType = JSVALUE_TYPE_STRING;
            boxedValue = String_createFromStack(env, ctx, stackPosition);
            break; }
        case DUK_TYPE_OBJECT:
            valueType = JSVALUE_TYPE_OBJECT;
            boxedValue = priv->objectBook.exposeObject(env, stackPosition);
            break;
        default:
            valueType = JSVALUE_TYPE_UNSUPPORTED;
    }

    JSValue jsValue = (JSValue) env->AllocObject(JSValue_Class);
    env->SetIntField(jsValue, JSValue_type, valueType);
    env->SetObjectField(jsValue, JSValue_value, boxedValue);

    return jsValue;
}

/**
 * May throw:
 * - AttemptedToUseObjectFromOtherVM
 * - IllegalArgumentException
 */
void
jsvm::JSValue_push(JNIEnv *env, JSValue jsValue, duk_context *ctx) {
    int valueType = env->GetIntField(jsValue, JSValue_type);
    jobject boxedValue = env->GetObjectField(jsValue, JSValue_value);

    switch (valueType) {
        case JSVALUE_TYPE_UNDEFINED:
            duk_push_undefined(ctx);
            break;
        case JSVALUE_TYPE_NULL:
            duk_push_null(ctx);
            break;
        case JSVALUE_TYPE_BOOLEAN:
            duk_push_boolean(ctx, env->GetBooleanField(boxedValue, Boolean_value));
            break;
        case JSVALUE_TYPE_NUMBER:
            duk_push_number(ctx, env->GetDoubleField(boxedValue, Double_value));
            break;
        case JSVALUE_TYPE_STRING:
            String_pushJString(env, (jstring) boxedValue, ctx);
            break;
        case JSVALUE_TYPE_OBJECT: {
            JSObject jsObject = (JSObject) boxedValue;
            JSVM jsObjectVM = (JSVM) env->GetObjectField(jsObject, JSObject_jsVM);
            duk_context* objectContext = JSVM_getPriv(env, jsObjectVM)->ctx;

            if (objectContext != ctx) {
                throw AttemptedToUseObjectFromOtherVM(env, jsObject, jsObjectVM);
            }

            JSObject_push(env, jsObject);
            break; }
        case JSVALUE_TYPE_UNSUPPORTED:
            throw IllegalArgumentException(env, "Attempted to use JSValue of unsupported type");
        default:
            throw JSVMInternalError("Unknown JSValue type");
    }
}