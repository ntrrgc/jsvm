//
// Created by ntrrgc on 1/28/17.
//
#include <jsvm/ObjectBook.h>
#include <jsvm/JSObject.h>
using namespace jsvm;

ObjectBook::handle_t ObjectBook::storeStackValue(int stackPosition, bool *alreadyExists) {
    jsvm_assert(m_ctx);
    jsvm_assert(alreadyExists); // required param!
    jsvm_assert(duk_is_object(m_ctx, stackPosition));

    // Copy the element at the provided stack position
    // into the stack top so that it can be referenced
    // easily and safely.
    duk_dup(m_ctx, stackPosition);

    // Check if [PRIVATE_PROPERTY_HANDLE] exists in order
    // to check if this object already has a handle.
    //
    // Note that we use duk_get_prop_desc
    // (Object.getOwnPropertyDescriptor()) instead of simple
    // property access because otherwise we could wrongly
    // read the handle from the prototype, which is actually
    // a different object!
    duk_push_lstring(m_ctx, PRIVATE_PROPERTY_HANDLE, PRIVATE_PROPERTY_HANDLE_LENGTH);
    duk_get_prop_desc(m_ctx, -2, 0 /* flags */);

    if (((*alreadyExists) = (bool) duk_is_object(m_ctx, -1))) {
        // The property exists!

        // Fetch the existing handle
        duk_get_prop_lstring(m_ctx, -1, "value", 5);
        jsvm_assert(duk_is_number(m_ctx, -1));
        handle_t handle = (handle_t) duk_get_int(m_ctx, -1);

        duk_pop_3(m_ctx); // handle, property descriptor, object dup

        return handle;
    } else {
        duk_pop(m_ctx); // property descriptor

        // Allocate a new handle
        handle_t handle = allocateHandle();

        // Store the handle privately in the book
        duk_push_string(m_ctx, PRIVATE_PROPERTY_HANDLE);
        duk_push_int(m_ctx, handle);
        duk_def_prop(m_ctx, -3, DUK_DEFPROP_FORCE | DUK_DEFPROP_HAVE_VALUE);

        // Store the object in the book:

        // Get the object book
        duk_push_global_stash(m_ctx);
        duk_get_prop_index(m_ctx, -1, GLOBAL_STASH_INDEX_OBJECT_BOOK);

        // Write the object in the book:
        duk_dup(m_ctx, -3);
        duk_put_prop_index(m_ctx, -2, handle);

        // Restore the stack
        duk_pop_3(m_ctx);

        return handle;
    }
}

void ObjectBook::saveJSObjectWithHandle(JNIEnv *env, ObjectBook::handle_t handle,
                                        _JSObject *jsObject) {
    jsvm_assert(jsObject);
    jsvm_assert(handle < m_jsObjectsByHandle.size());
    jsvm_assert(m_jsObjectsByHandle[handle] == NULL);

    m_jsObjectsByHandle[handle] = (_JSObject *) env->NewGlobalRef(jsObject);
}

_JSObject *ObjectBook::getJSObjectWithHandle(ObjectBook::handle_t handle) {
    jsvm_assert(handle < m_jsObjectsByHandle.size());
    _JSObject* ret = m_jsObjectsByHandle[handle];
    jsvm_assert(ret != NULL);
    return ret;
}

void ObjectBook::removeObjectWithHandle(JNIEnv *env, ObjectBook::handle_t handle) {
    jsvm_assert(m_ctx);
    jsvm_assert(handle < m_nextFree);

    // Forget the associated JSObject
    forgetJSObjectWithHandle(env, handle);

    // Get the object book
    duk_push_global_stash(m_ctx);
    duk_get_prop_index(m_ctx, -1, GLOBAL_STASH_INDEX_OBJECT_BOOK);

    // Overwrite the handle entry with null in the book:
    duk_push_null(m_ctx);
    duk_put_prop_index(m_ctx, -2, handle);

    // Restore the stack
    duk_pop_2(m_ctx);

    deallocateHandle(handle);
}

void ObjectBook::pushObjectWithHandle(ObjectBook::handle_t handle) {
    jsvm_assert(m_ctx);
    jsvm_assert(handle < m_nextFree);

    // Get the object book
    duk_push_global_stash(m_ctx);
    duk_get_prop_index(m_ctx, -1, GLOBAL_STASH_INDEX_OBJECT_BOOK);

    // Fetch the handle entry with the provided handle:
    duk_get_prop_index(m_ctx, -1, handle);

    // Remove the stack and book from the stack,
    // leaving the retrieved object on top of what was
    // the previous stack top.
    duk_copy(m_ctx, -1, -3);
    duk_pop_2(m_ctx);
}

void ObjectBook::lateInit(duk_context *ctx) {
    jsvm_assert(ctx);
    jsvm_assert(this->m_ctx == NULL);
    this->m_ctx = ctx;

    // Initialize the book object
    duk_push_global_stash(m_ctx);
    duk_push_object(m_ctx);
    duk_put_prop_index(m_ctx, -2, GLOBAL_STASH_INDEX_OBJECT_BOOK);
    duk_pop(m_ctx);
}

ObjectBook::handle_t ObjectBook::allocateHandle() {
    if (!m_freeList.empty()) {
        handle_t ret = m_freeList.top();
        m_freeList.pop();
        return ret;
    } else {
        m_jsObjectsByHandle.push_back(NULL);
        return m_nextFree++;
    }
}

void ObjectBook::deallocateHandle(ObjectBook::handle_t handle) {
    m_freeList.push(handle);
}

void ObjectBook::forgetJSObjectWithHandle(JNIEnv *env, ObjectBook::handle_t handle) {
    jsvm_assert(env);
    jsvm_assert(handle < m_jsObjectsByHandle.size());
    jsvm_assert(m_jsObjectsByHandle[handle] != NULL);

    env->DeleteGlobalRef(m_jsObjectsByHandle[handle]);
#ifndef NDEBUG
    m_jsObjectsByHandle[handle] = NULL;
#endif
}
