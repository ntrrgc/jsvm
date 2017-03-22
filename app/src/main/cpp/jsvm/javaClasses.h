//
// Created by ntrrgc on 1/15/17.
//

#pragma once

#include <stdafx.h>

namespace jsvm {

    extern jclass       Boolean_Class;
    extern jmethodID    Boolean_ctor;
    extern jfieldID     Boolean_value;

    extern jclass       Double_Class;
    extern jmethodID    Double_ctor;
    extern jfieldID     Double_value;

    extern jclass       JSVM_Class;
    extern jfieldID     JSVM_hPriv;
    extern jfieldID     JSVM_jsObjectsByHandle;
    extern jfieldID     JSVM_deadJSObjectsRefs;

    extern jclass       JSObjectWeakReference_Class;
    extern jmethodID    JSObjectWeakReference_ctor;
    extern jmethodID    JSObjectWeakReference_get;
    extern jfieldID     JSObjectWeakReference_handle;

    extern jclass       ArrayList_Class;
    extern jmethodID    ArrayList_add;
    extern jmethodID    ArrayList_get;
    extern jmethodID    ArrayList_set;
    extern jmethodID    ArrayList_size;

    extern jclass       ReferenceQueue_Class;
    extern jmethodID    ReferenceQueue_poll;

    extern jclass       JSVMInternalError_Class;
    extern jmethodID    JSVMInternalError_ctor;

    extern jclass       JSError_Class;
    extern jmethodID    JSError_ctor;

    extern jclass       IllegalArgumentException_Class;
    extern jmethodID    IllegalArgumentException_ctor;

    extern jclass       AttemptedToUseObjectFromOtherVM_Class;
    extern jmethodID    AttemptedToUseObjectFromOtherVM_ctor;

    extern jclass       JSValue_Class;
    extern jfieldID     JSValue_type;
    extern jfieldID     JSValue_value;

    extern jclass       JSObject_Class;
    extern jmethodID    JSObject_ctor;
    extern jfieldID     JSObject_jsVM;
    extern jfieldID     JSObject_handle;
    extern jfieldID     JSObject_aliveHandle;
    extern jfieldID     JSObject_accessorChain;

    extern jclass       JSFunction_Class;
    extern jmethodID    JSFunction_ctor;

    class _JSObjectWeakReference: public _jobject {};
    typedef _JSObjectWeakReference* JSObjectWeakReference;

    class _ArrayList: public _jobject {};
    typedef _ArrayList* ArrayList;

    class _ReferenceQueue: public _jobject {};
    typedef _ReferenceQueue* ReferenceQueue;

    void initClassesAndFields(JNIEnv *env);
    void tearDownClassesAndFields(JNIEnv *env);

}
