//
// Created by ntrrgc on 1/22/17.
//

#include <jsvm/exceptions.h>
#include <sstream>
#include "JSValue.h"
#include "JSObject.h"

using namespace jsvm;

JSError::JSError(JNIEnv *env, JSValue jsErrorValue)
        : InstantiatedJavaException((jthrowable) env->NewObject(
        JSError_Class, JSError_ctor, (jobject) jsErrorValue)) { }

AttemptedToUseObjectFromOtherVM::AttemptedToUseObjectFromOtherVM(JNIEnv *env, JSObject jsObject,
                                                                 JSVM jsVM)
        : InstantiatedJavaException((jthrowable) env->NewObject(
        AttemptedToUseObjectFromOtherVM_Class,
        AttemptedToUseObjectFromOtherVM_ctor,
        jsObject, jsVM)) { }

IllegalArgumentException::IllegalArgumentException(JNIEnv *env, const std::string &message)
        : InstantiatedJavaException((jthrowable) env->NewObject(
        IllegalArgumentException_Class,
        IllegalArgumentException_ctor,
        env->NewStringUTF(message.c_str())
)) { }

void
JSVMInternalError::propagateToJava(JNIEnv *env) {
    jsvm_assert(env->ExceptionCheck() == JNI_FALSE);
    env->Throw((jthrowable) env->NewObject(
            JSVMInternalError_Class,
            JSVMInternalError_ctor,
            env->NewStringUTF(m_message.c_str())));
}

void ::jsvm::_throwAssertionFailed(const char *file, int line, const char *functionName,
                                   const char *expression) {
    std::ostringstream ss;
    ss << std::string("Assertion failed: (") << expression << ") in " << file << ":" << line
        << " (" << functionName << ")";
    throw JSVMInternalError(ss.str());
}