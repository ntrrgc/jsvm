/* Copyright (c) the JSVM authors
 * https://github.com/ntrrgc/jsvm
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

//
// Created by ntrrgc on 1/21/17.
//

#ifndef JSVM_EXCEPTIONS_H
#define JSVM_EXCEPTIONS_H

#include <stdafx.h>

namespace jsvm {

#ifndef NDEBUG
#define	jsvm_assert(e) ((e) ? (void)0 : _throwAssertionFailed(__FILE__, __LINE__, __func__, #e))
#else
#define jsvm_assert(e) ((void)0)
#endif

    void _throwAssertionFailed(const char *file, int line, const char *functionName,
                               const char *expression);

    class JavaException: public std::exception {
    public:
        virtual void propagateToJava(JNIEnv* env) = 0;
        virtual std::unique_ptr<JavaException> moveClone() throw() = 0;
    };

    class JSVMInternalError final: public JavaException {
    public:
        JSVMInternalError(const std::string& message) : m_message(message) {}

        virtual void propagateToJava(JNIEnv *env) override;

        virtual std::unique_ptr<JavaException> moveClone() throw() override {
            return std::unique_ptr<JavaException>(new JSVMInternalError(m_message));
        }

    private:
        std::string m_message;
    };

    /**
     * Convenience superclass used by most other exceptions.
     */
    class InstantiatedJavaException: public JavaException {
    public:
        InstantiatedJavaException(jthrowable errorObject): m_errorObject(errorObject) {}
        virtual ~InstantiatedJavaException() {}

        virtual void propagateToJava(JNIEnv* env) override {
            jsvm_assert(env->ExceptionCheck() == JNI_FALSE);
            env->Throw(m_errorObject);
        }

    protected:
        jthrowable m_errorObject;
    };

    class IllegalArgumentException final: public InstantiatedJavaException {
    public:
        IllegalArgumentException(JNIEnv* env, const std::string& message);

        virtual std::unique_ptr<JavaException> moveClone() throw() override {
            return std::unique_ptr<JavaException>(new IllegalArgumentException(m_errorObject));
        }

    private:
        IllegalArgumentException(jthrowable errorObject): InstantiatedJavaException(errorObject) {}
    };

    class _JSValue;
    class _JSObject;
    class _JSVM;

    class JSError final: public InstantiatedJavaException {
    public:
        JSError(JNIEnv* env, _JSValue* jsErrorValue);

        virtual std::unique_ptr<JavaException> moveClone() throw() override {
            return std::unique_ptr<JavaException>(new JSError(m_errorObject));
        }

    private:
        JSError(jthrowable errorObject): InstantiatedJavaException(errorObject) {}
    };

    class AttemptedToUseObjectFromOtherVM final: public InstantiatedJavaException {
    public:
        AttemptedToUseObjectFromOtherVM(JNIEnv* env, _JSObject* jsObject, _JSVM* jsVM);

        virtual std::unique_ptr<JavaException> moveClone() throw() override {
            return std::unique_ptr<JavaException>(new AttemptedToUseObjectFromOtherVM(m_errorObject));
        }

    private:
        AttemptedToUseObjectFromOtherVM(jthrowable errorObject): InstantiatedJavaException(errorObject) {}
    };

}

#endif //JSVM_EXCEPTIONS_H
