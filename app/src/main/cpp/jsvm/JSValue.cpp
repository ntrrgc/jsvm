//
// Created by ntrrgc on 1/15/17.
//

#include "JSValue.h"
#include "JSObject.h"
#include <jsvm/JSVM.h>
using namespace jsvm;

#define JSVALUE_TYPE_UNSUPPORTED 0
#define JSVALUE_TYPE_UNDEFINED 1
#define JSVALUE_TYPE_NULL 2
#define JSVALUE_TYPE_BOOLEAN 3
#define JSVALUE_TYPE_NUMBER 4
#define JSVALUE_TYPE_STRING 5
#define JSVALUE_TYPE_OBJECT 6

JSValue
jsvm::JSValue_createFromStackTop(JNIEnv *env, JSVM jsVM) {
    JSVMPriv *priv = JSVM_getPriv(env, jsVM);
    duk_context *ctx = priv->ctx;

    duk_int_t dukType = duk_get_type(ctx, -1);

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
                                        duk_get_boolean(ctx, -1));
            break;
        case DUK_TYPE_NUMBER:
            valueType = JSVALUE_TYPE_NUMBER;
            boxedValue = env->NewObject(Double_Class, Double_ctor,
                                        duk_get_number(ctx, -1));
            break;
        case DUK_TYPE_STRING:
            valueType = JSVALUE_TYPE_STRING;
            boxedValue = String_createFromStackTop(env, ctx);
            break;
        case DUK_TYPE_OBJECT:
            valueType = JSVALUE_TYPE_OBJECT;
            boxedValue = JSObject_createFromStackTop(env, jsVM);
            break;
        default:
            valueType = JSVALUE_TYPE_UNSUPPORTED;
    }

    JSValue jsValue = (JSValue) env->AllocObject(JSValue_Class);
    env->SetIntField(jsValue, JSValue_type, valueType);
    env->SetObjectField(jsValue, JSValue_value, boxedValue);

    return jsValue;
}