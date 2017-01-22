//
// Created by ntrrgc on 1/15/17.
//
#ifndef JSVM_JSVM
#define JSVM_JSVM
#pragma once

#include <stdafx.h>
#include <stack>
#include <setjmp.h>
#include <jsvm/ObjectBook.h>
#include <jsvm/exceptions.h>

namespace jsvm {

    class _JSVM : public _jobject {};
    typedef _JSVM* JSVM;

    struct WrappedJmpBuf {
        jmp_buf jmpBuf;
    };

    /**
     * This class stores and manages JSVM internals.
     *
     * A pointer to this class is stored in JSVM::hPriv.
     */
    struct JSVMPriv {
        JSVMPriv(JNIEnv *initialJNIEnv, JSVM initialJSVM);
        ~JSVMPriv();

        // Handle to the JS heap and VM.
        duk_context *ctx;
        // Used to invoke Java methods and throw Java exceptions.
        JNIEnv *env;
        // Pointer to the instance of JSVM in Java.
        // It's only valid since setUpEnv() is called
        // until control returns to Java code.
        JSVM jsVM;

        ObjectBook objectBook;

        /**
         * This method should be called just after the
         * transition from Java to C++, before anything
         * is done with duk_context, as it will be read
         * in the following situations:
         *
         * - if an unhandled exception is throw by JS code,
         *   in order to propagate it to Java code.
         * - in order to invoke exported Java methods.
         *
         * Note that JNIEnv pointers vary from thread
         * to thread... But this is not a problem since
         * JSVM instances are synchronized.
         */
        void setUpEnv(JNIEnv *env, JSVM jsVM) {
            this->env = env;
            this->jsVM = jsVM;
        }

        jmp_buf& allocateExceptionHandler() {
            unhandledExceptionHandlers.push(WrappedJmpBuf());
            return unhandledExceptionHandlers.top().jmpBuf;
        }

        void tearDownExceptionHandler() {
            assert(!unhandledExceptionHandlers.empty());
            unhandledExceptionHandlers.pop();
        }

        void unwindIntoExceptionHandler() {
            long * x = unhandledExceptionHandlers.top().jmpBuf;
            longjmp(x, THREW_EXCEPTION);
        }

    private:
        std::stack<WrappedJmpBuf> unhandledExceptionHandlers;
    };

    JSVMPriv * JSVM_getPriv(JNIEnv *env, JSVM jsVM);

}

#endif