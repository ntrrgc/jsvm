//
// Created by ntrrgc on 1/15/17.
//

#pragma once

#include <stdafx.h>
#include <stack>
#include <vector>

namespace jsvm {

    class _JSObject;
    class JSVMPriv;

    /**
     * This class manages the "object book", an array
     * inside the global stash where references to objects
     * are stored to ensure they are not GCed while they
     * are accessed in Java-land.
     *
     * The index is used as a handle that allows accessing
     * the objects from Java at anytime without worries
     * about JS-land GC memory compaction.
     */
    struct ObjectBook {
        typedef unsigned int handle_t;
        static const handle_t NULL_HANDLE = (handle_t) -1;

        /**
         * Insert the object at the top of the Duktape stack
         * into the object book and return a JSObject that
         * references it.
         *
         * If the object already exists in the book, the same
         * JSObject is reused and the object book is not modified
         * in any way.
         */
        _JSObject * exposeObject(JNIEnv *pEnv, duk_idx_t stackPos);

        /**
         * Remove the object associated to a handle so that its
         * memory can be reclaimed by the Duktape GC.
         *
         * The handler becomes free and will be reused for the
         * next allocated object.
         *
         * Called by JSObject.finalizeNative()
         */
        void finalizeJSObjectWithHandle(JNIEnv *env,
                                        ObjectBook::handle_t handle);

        /**
         * Retrieve an object from the book using its handle
         * and push it to the top of the Duktape stack.
         */
        void pushObjectWithHandle(handle_t handle);

    private:
        friend class JSVMPriv; // can only be constructed and initialized inside JSVM
        typedef std::stack <ObjectBook::handle_t> FreeList;

        // This property can be read and written only in C code.
        static constexpr const char *const PRIVATE_PROPERTY_HANDLE = "\xffH";
        static constexpr const int PRIVATE_PROPERTY_HANDLE_LENGTH = 2;

        ObjectBook() : ctx(NULL), priv(NULL), m_nextFree(0) { }
        void lateInit(duk_context *ctx, JSVMPriv* priv);

        duk_context *ctx;
        JSVMPriv *priv;
        FreeList m_freeList;
        handle_t m_nextFree;

        /**
         * Create or reuse a handle for a JSObject.
         */
        handle_t allocateHandle(JNIEnv* env);

        /**
         * Free a handle so that it can be reused.
         */
        void deallocateHandle(handle_t handle);

        /**
         * Remove objects from the book whose java-land JSObject's
         * have been GCed, so they can also be GCed in JS.
         */
        void gcDeadJSObjects(JNIEnv* env);

        /**
         * Save the created JSObject so that it can be reused if
         * the same JS object (and therefore handle) is requested later.
         */
        void saveNewJSObjectWithHandle(JNIEnv *env, _JSObject *jsObject,
                                       ObjectBook::handle_t handle);

        /**
         * Retrieve the latest JSObject that uses a certain handle
         * from the handle -> Weak JSObject table.
         */
        _JSObject* getJSObjectWithHandle(JNIEnv *env, ObjectBook::handle_t handle);

        /**
         * Remove a JSObject from the handle -> Weak JSObject table
         * and release its GlobalWeakRef.
         */
        void forgetJSObjectWithHandle(JNIEnv* env, handle_t handle);

        /**
         * Instantiate a JSObject or JSFunction with the given handle.
         * `stackPosition` is used to read the type of the object and
         * create the correct subclass.
         */
        _JSObject *createJSObjectFromStack(JNIEnv *env, ObjectBook::handle_t handle,
                                           duk_idx_t stackPosition);

        /**
         * Store the handle in a private property of the object at the top of
         * the stack. This suceeds even if the object is sealed or frozen.
         */
        void setStackTopObjectHandle(handle_t handle);

        /**
         * Read the handle stored in a private property of the object at the
         * top of the stack.
         */
        handle_t getStackTopObjectHandle();

        /**
         * Read the handle stored in the object at the top of the stack, if
         * any and try to find a living JSObject wrapping it.
         *
         * @return The found JSObject, or null if not found.
         */
        _JSObject* getStackTopExistingJSObject(JNIEnv *env);
    };

}
