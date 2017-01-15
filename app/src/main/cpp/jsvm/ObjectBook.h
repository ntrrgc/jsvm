//
// Created by ntrrgc on 1/15/17.
//

#pragma once

#include <stdafx.h>
#include <stack>

namespace jsvm {

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
         */
        handle_t storeStackTop() {
            assert(m_ctx);
            handle_t handle = allocateHandle();

            // Store the object in the book:

            // Get the object book
            duk_push_global_stash(m_ctx);
            duk_push_int(m_ctx, GLOBAL_STASH_INDEX_OBJECT_BOOK);
            duk_get_prop(m_ctx, -2);

            // Write the object in the book:
            duk_dup(m_ctx, -3);
            duk_put_prop_index(m_ctx, -2, handle);

            // Restore the stack
            duk_pop_2(m_ctx);

            return handle;
        }

        /**
         * Remove the object associated to handle so that its
         * memory can be reclaimed by the Duktape GC.
         *
         * The handler becomes free and will be reused for the
         * next allocated object.
         */
        void removeObjectWithHandle(handle_t handle) {
            assert(m_ctx);
            assert(handle < m_nextFree);

            // Get the object book
            duk_push_global_stash(m_ctx);
            duk_push_int(m_ctx, GLOBAL_STASH_INDEX_OBJECT_BOOK);
            duk_get_prop(m_ctx, -2);

            // Overwrite the handle entry with null in the book:
            duk_push_null(m_ctx);
            duk_put_prop_index(m_ctx, -2, handle);

            // Restore the stack
            duk_pop_2(m_ctx);

            deallocateHandle(handle);
        }

        /**
         * Retrieve an object from the book using its handle
         * and push it to the top of the Duktape stack.
         */
        void pushObjectWithHandle(handle_t handle) {
            assert(m_ctx);
            assert(handle < m_nextFree);

            // Get the object book
            duk_push_global_stash(m_ctx);
            duk_push_int(m_ctx, GLOBAL_STASH_INDEX_OBJECT_BOOK);
            duk_get_prop(m_ctx, -2);

            // Fetch the handle entry with the provided handle:
            duk_get_prop_index(m_ctx, -1, handle);

            // Remove the stack and book from the stack,
            // leaving the retrieved object on top of what was
            // the previous stack top.
            duk_copy(m_ctx, -1, -3);
            duk_pop_2(m_ctx);
        }

    private:
        friend class JSVMPriv; // can only be constructed and initialized inside JSVM
        typedef std::stack <ObjectBook::handle_t> FreeList;

        ObjectBook() : m_ctx(NULL), m_nextFree(0) { }
        void lateInit(duk_context *ctx) {
            assert(ctx);
            assert(this->m_ctx == NULL);
            this->m_ctx = ctx;
        }

        duk_context *m_ctx;
        FreeList m_freeList;
        handle_t m_nextFree;

        handle_t allocateHandle() {
            if (!m_freeList.empty()) {
                handle_t ret = m_freeList.top();
                m_freeList.pop();
                return ret;
            } else {
                return m_nextFree++;
            }
        }

        void deallocateHandle(handle_t handle) {
            m_freeList.push(handle);
        }
    };

}
