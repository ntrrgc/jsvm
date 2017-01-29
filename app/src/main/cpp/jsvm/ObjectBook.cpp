//
// Created by ntrrgc on 1/28/17.
//
#include <jsvm/ObjectBook.h>
#include <jsvm/JSObject.h>
#include "JSFunction.h"

using namespace jsvm;

int weakRefCount = 0;

void ObjectBook::lateInit(duk_context *ctx, JSVMPriv* priv) {
    jsvm_assert(ctx);
    jsvm_assert(priv);
    jsvm_assert(this->ctx == NULL);
    jsvm_assert(this->priv == NULL);
    this->ctx = ctx;
    this->priv = priv;

    // Initialize the book object
    duk_push_global_stash(ctx);
    duk_push_object(ctx);
    duk_put_prop_index(ctx, -2, GLOBAL_STASH_INDEX_OBJECT_BOOK);
    duk_pop(ctx);
}

_JSObject *ObjectBook::createJSObjectFromStack(JNIEnv *env, ObjectBook::handle_t handle, duk_idx_t stackPosition) {
    jsvm_assert(duk_is_object(ctx, stackPosition));

    JSObject jsObject;
    if (duk_is_function(ctx, stackPosition)) {
        // Create new JSFunction
        jsObject = (JSFunction) env->NewObject(JSFunction_Class, JSFunction_ctor);
    } else {
        // Create new JSObject
        jsObject = (JSObject) env->NewObject(JSObject_Class, JSObject_ctor);
    }
    jsvm_assert(jsObject);

    env->SetObjectField(jsObject, JSObject_jsVM, priv->jsVM);
    env->SetIntField(jsObject, JSObject_handle, handle);

    return jsObject;
}

JSObject ObjectBook::exposeObject(JNIEnv *env, duk_idx_t stackPosition) {
    jsvm_assert(ctx);
    jsvm_assert(duk_is_object(ctx, stackPosition));

    // Copy the element at the provided stack position
    // into the stack top so that it can be referenced
    // easily and safely.
    duk_dup(ctx, stackPosition);

    // Read the handle property of the object
    handle_t handle = getStackTopObjectHandle();

    if (handle != NULL_HANDLE) {
        // A handle already exists!

        duk_pop(ctx); // object dup

        // Find an existing JSObject with the same handle
        JSObject existingJSObject = this->getJSObjectWithHandle(env, handle);
        if (existingJSObject != NULL) {
            return existingJSObject;
        } else {
            // The JSObject is no longer accessible due to GC, but
            // the handle has not been reclaimed yet. This may
            // happen if the JVM decides to free the JSObject while
            // we are running this code in JSVM, as the JSObject
            // finalize() method will stay blocked in the synchronize
            // barrier.

            // Create a new JSObject with the same handle and replace
            // the dead reference in m_jsObjectsByHandle with the
            // new one.
            //
            // Note JSObject.finalize() will check if its place in
            // m_jsObjectsByHandle has been taken by this process,
            // and only if that's not the case, the handle will be
            // removed from the book.

            JSObject replacementJSObject = this->createJSObjectFromStack(env, handle, stackPosition);

            env->DeleteWeakGlobalRef(m_jsObjectsByHandle[handle]);
            weakRefCount--;
            m_jsObjectsByHandle[handle] = (JSObject) env->NewWeakGlobalRef(replacementJSObject);
            weakRefCount++;

            return replacementJSObject;
        }
    } else {
        // The handle property does not exist

        // Allocate a new handle
        handle = allocateHandle();

        // Store the handle privately in the object
        setStackTopObjectHandle(handle);

        // Store the handle -> object pair in the book:

        // Get the object book
        duk_push_global_stash(ctx);
        duk_get_prop_index(ctx, -1, GLOBAL_STASH_INDEX_OBJECT_BOOK);

        // Write the object in the book:
        duk_dup(ctx, -3);
        duk_put_prop_index(ctx, -2, handle);

        // Restore the stack
        duk_pop_3(ctx); // book, global stash, object dup

        // Create the JSObject in Javaland
        JSObject jsObject = createJSObjectFromStack(env, handle, stackPosition);

        // Store a weak reference so it can be reused
        priv->objectBook.saveNewJSObjectWithHandle(env, handle, jsObject);

        return jsObject;
    }
}

ObjectBook::handle_t ObjectBook::getStackTopObjectHandle() {
    // Check if [PRIVATE_PROPERTY_HANDLE] exists in order
    // to check if this object already has a handle.
    //
    // Note that we use duk_get_prop_desc
    // (Object.getOwnPropertyDescriptor()) instead of simple
    // property access because otherwise we could wrongly
    // read the handle from the prototype, which is actually
    // a different object!
    duk_push_lstring(ctx, PRIVATE_PROPERTY_HANDLE, PRIVATE_PROPERTY_HANDLE_LENGTH);
    duk_get_prop_desc(ctx, -2, 0 /* flags */);

    if (duk_is_object(ctx, -1)) {
        // The handle property exists!

        // Fetch the existing handle
        duk_get_prop_lstring(ctx, -1, "value", 5);
        jsvm_assert(duk_is_number(ctx, -1));
        handle_t handle = (handle_t) duk_get_int(ctx, -1);

        duk_pop_2(ctx); // handle, property descriptor

        // Note: the handle may be NULL_HANDLE if it got
        // removed from the object book in the past.
        return handle;
    } else {
        duk_pop(ctx); // undefined property descriptor

        return NULL_HANDLE;
    }
}

void ObjectBook::setStackTopObjectHandle(ObjectBook::handle_t handle) {
    jsvm_assert(duk_is_object(ctx, -1));

    // Store the handle privately in the object
    duk_push_lstring(ctx, PRIVATE_PROPERTY_HANDLE, PRIVATE_PROPERTY_HANDLE_LENGTH);
    duk_push_int(ctx, handle);
    duk_def_prop(ctx, -3, DUK_DEFPROP_FORCE | DUK_DEFPROP_HAVE_VALUE);
}

void ObjectBook::saveNewJSObjectWithHandle(JNIEnv *env, ObjectBook::handle_t handle,
                                           _JSObject *jsObject) {
    jsvm_assert(jsObject);
    jsvm_assert(handle < m_jsObjectsByHandle.size());
    jsvm_assert(m_jsObjectsByHandle[handle] == NULL);

    JSObject weakRef = (JSObject) env->NewWeakGlobalRef(jsObject);
    jsvm_assert(weakRef);
    weakRefCount++;
    m_jsObjectsByHandle[handle] = weakRef;
}

void ObjectBook::forgetJSObjectWithHandle(JNIEnv *env, ObjectBook::handle_t handle) {
    jsvm_assert(env);
    jsvm_assert(handle < m_jsObjectsByHandle.size());
    jsvm_assert(m_jsObjectsByHandle[handle] != NULL);

    env->DeleteWeakGlobalRef(m_jsObjectsByHandle[handle]);
    weakRefCount--;
    m_jsObjectsByHandle[handle] = NULL;
}

_JSObject *ObjectBook::getJSObjectWithHandle(JNIEnv *env, ObjectBook::handle_t handle) {
    jsvm_assert(handle < m_jsObjectsByHandle.size());
    JSObject weakRef = m_jsObjectsByHandle[handle];
    jsvm_assert(weakRef != NULL);

    // will be null if weakRef has been GCed
    JSObject localRef = (JSObject) env->NewLocalRef(weakRef);
    return localRef;
}

void ObjectBook::finalizeJSObjectWithHandle(JNIEnv *env, JSObject jsObject, ObjectBook::handle_t handle) {
    jsvm_assert(ctx);
    jsvm_assert(jsObject);
    jsvm_assert(env);
    jsvm_assert(handle < m_jsObjectsByHandle.size());
    jsvm_assert(m_jsObjectsByHandle[handle] != NULL);

    if (!env->IsSameObject(jsObject, m_jsObjectsByHandle[handle])) {
        // This JSObject was set for GC, but before it entered
        // the JSVM synchronized block in finalize(), another thread
        // wanted a JSObject referencing the same object.
        //
        // Since this JSObject was no longer valid then, it was
        // replaced by a new one with the same handle.
        // (See exposeObject() for more details)
        //
        // Return without doing anything to the book, as the new
        // JSObject may be in use now.
        return;
    }

    // Forget the associated JSObject
    forgetJSObjectWithHandle(env, handle);

    // Get the object book
    duk_push_global_stash(ctx);
    duk_get_prop_index(ctx, -1, GLOBAL_STASH_INDEX_OBJECT_BOOK);

    // Get the object
    duk_get_prop_index(ctx, -1, handle);

    // Reset the handle property in the object
    setStackTopObjectHandle(NULL_HANDLE);

    duk_pop(ctx); // object

    // Overwrite the handle entry with null in the book:
    duk_push_null(ctx);
    duk_put_prop_index(ctx, -2, handle);

    // Restore the stack
    duk_pop_2(ctx); // book, global stash

    // Deallocate the handle so that it can be reused later
    deallocateHandle(handle);
}

void ObjectBook::pushObjectWithHandle(ObjectBook::handle_t handle) {
    jsvm_assert(ctx);
    jsvm_assert(handle < m_nextFree);

    // Get the object book
    duk_push_global_stash(ctx);
    duk_get_prop_index(ctx, -1, GLOBAL_STASH_INDEX_OBJECT_BOOK);

    // Fetch the handle entry with the provided handle:
    duk_get_prop_index(ctx, -1, handle);

    // Remove the stack and book from the stack,
    // leaving the retrieved object on top of what was
    // the previous stack top.
    duk_copy(ctx, -1, -3);
    duk_pop_2(ctx);
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
