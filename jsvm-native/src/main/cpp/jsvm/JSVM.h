/* Copyright (c) the JSVM authors
 * https://github.com/ntrrgc/jsvm
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

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

    /**
     * This class stores and manages JSVM internals.
     *
     * A pointer to this class is stored in JSVM::hPriv.
     */
    struct JSVMPriv {
        JSVMPriv();
        ~JSVMPriv();

        // Handle to the JS heap and VM.
        duk_context *ctx;

        // Keeps track of JS objects accessible from Java.
        ObjectBook objectBook;
    };

    class JSVMCallContext {
    public:
        // A new JSVMCallContext must be created when a JNI call to use a JSVM
        // is received. The context contains a pointer to JSVMPriv and several
        // JNI local references.
        JSVMCallContext(JNIEnv* jniEnv, JSVM jsVM);
        ~JSVMCallContext();

        // A C++ handler may be invoked as a consequence of running JavaScript
        // code, for instance, to call a Java function or throw an exception.
        // Such handler can retrieve the current context from the stack using
        // this function.
        static JSVMCallContext& current();

        JSVMCallContext(const JSVMCallContext&) = delete;
        JSVMCallContext& operator=(const JSVMCallContext&) = delete;

        JSVMPriv* priv() const { return m_priv; }

        // The following are JNI local references, therefore valid only during
        // the lifetime of the JNI invocation.
        JNIEnv* jniEnv() const { return m_jniEnv; }
        JSVM jsVM() const { return m_jsVM; }
        ArrayList jsObjectsByHandle();

    private:
        JNIEnv* m_jniEnv;
        JSVM m_jsVM;
        JSVMPriv* m_priv;
        ArrayList m_jsObjectsByHandle = nullptr; // Lazy loaded.

        static thread_local std::stack<JSVMCallContext*> s_thisThreadStack;
    };

    JSVMPriv * JSVM_peekPriv(JNIEnv *env, JSVM jsVM);

}

#endif