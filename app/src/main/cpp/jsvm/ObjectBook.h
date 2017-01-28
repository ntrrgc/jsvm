//
// Created by ntrrgc on 1/15/17.
//

#pragma once

#include <stdafx.h>
#include <stack>
#include <vector>

namespace jsvm {

    class _JSObject;

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

        /**
         * Insert the object at the top of the Duktape stack
         * into the object book and return its handle.
         *
         * If the object already exists in the book, its handle
         * is reused and the object book is not modified in any
         * way.
         *
         * In `alreadyExists` it is written whether the object
         * already existed in the book.
         *
         * If the handle did not exist before the caller MUST
         * then call saveJSObjectWithHandle() with the created
         * JSObject.
         */
        handle_t storeStackValue(int stackPosition, bool* alreadyExists);

        // Save the JSObject created to access the JS object with
        // the provided handle so that it can be reused if the
        // same JS object is requested later.
        void saveJSObjectWithHandle(JNIEnv* env, handle_t handle, _JSObject *jsObject);

        _JSObject* getJSObjectWithHandle(handle_t handle);

        /**
         * Remove the object associated to handle so that its
         * memory can be reclaimed by the Duktape GC.
         *
         * The handler becomes free and will be reused for the
         * next allocated object.
         */
        void removeObjectWithHandle(JNIEnv* env, handle_t handle);

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

        std::vector<_JSObject*> m_jsObjectsByHandle;

        ObjectBook() : m_ctx(NULL), m_nextFree(0) { }
        void lateInit(duk_context *ctx);

        duk_context *m_ctx;
        FreeList m_freeList;
        handle_t m_nextFree;

        handle_t allocateHandle();

        void deallocateHandle(handle_t handle);

        void forgetJSObjectWithHandle(JNIEnv* env, handle_t handle);
    };

}
