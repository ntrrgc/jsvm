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

    // Try to find a previously created JSObject
    JSObject existingJSObject = this->getStackTopExistingJSObject(env);

    if (existingJSObject != NULL) {
        return existingJSObject;
    } else {
        // The object does not have handle associated, or
        // its handle has dead but has not been finalized.

        // This is a good time to perform finalization:
        // Handles of JSObjects deleted by the JVM GC will
        // be released, freeing memory for Duktape and
        // getting more handles to reuse.
        // This process is fast if no JSObjects have been
        // deleted since the last execution.
        gcDeadJSObjects(env);

        // Allocate a new handle
        handle_t handle = allocateHandle(env);

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
        saveNewJSObjectWithHandle(env, jsObject, handle);

        return jsObject;
    }
}

JSObject ObjectBook::getStackTopExistingJSObject(JNIEnv *env) {
    // Read the current handle property of the object
    handle_t handle = getStackTopObjectHandle();

    if (handle != NULL_HANDLE) {
        // A handle exists.
        // Find an existing JSObject with the same handle.
        JSObject existingJSObject = this->getJSObjectWithHandle(env, handle);
        if (existingJSObject != NULL) {
            return existingJSObject;
        } else {
            // The JSObject is no longer accessible due to GC, but
            // the handle has not been reclaimed yet. That will
            // happen in the next call to gcDeadJSObjects().
            return NULL;
        }
    } else {
        return NULL;
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

void ObjectBook::saveNewJSObjectWithHandle(JNIEnv *env, _JSObject *jsObject,
                                           ObjectBook::handle_t handle) {
    jsvm_assert(jsObject);

    ArrayList jsObjectsByHandle = this->priv->jsObjectsByHandle;
    jsvm_assert(handle < env->CallIntMethod(jsObjectsByHandle, ArrayList_size));
    jsvm_assert(NULL == env->CallObjectMethod(jsObjectsByHandle, ArrayList_get, handle));

    ReferenceQueue deadJSObjectsRefs = (ReferenceQueue) env->GetObjectField(priv->jsVM, JSVM_deadJSObjectsRefs);
    jsvm_assert(deadJSObjectsRefs != NULL);

    JSObjectWeakReference weakRef = (JSObjectWeakReference) env->NewObject(
            JSObjectWeakReference_Class, JSObjectWeakReference_ctor,
            jsObject, deadJSObjectsRefs, handle);
    jsvm_assert(weakRef);

    weakRefCount++;
    env->DeleteLocalRef(env->CallObjectMethod(jsObjectsByHandle, ArrayList_set, handle, weakRef));
    env->DeleteLocalRef(weakRef);
}

void ObjectBook::forgetJSObjectWithHandle(JNIEnv *env, ObjectBook::handle_t handle) {
    jsvm_assert(env != NULL);
    ArrayList jsObjectsByHandle = this->priv->jsObjectsByHandle;
    jsvm_assert(jsObjectsByHandle != NULL);
    jsvm_assert(handle < env->CallIntMethod(jsObjectsByHandle, ArrayList_size));

    JSObjectWeakReference weakReference = (JSObjectWeakReference)
            env->CallObjectMethod(jsObjectsByHandle, ArrayList_get, handle);
    jsvm_assert(weakReference != NULL);

    env->DeleteLocalRef(env->CallObjectMethod(jsObjectsByHandle, ArrayList_set, handle, NULL));
    env->DeleteLocalRef(weakReference);
    weakRefCount--;
}

_JSObject *ObjectBook::getJSObjectWithHandle(JNIEnv *env, ObjectBook::handle_t handle) {
    ArrayList jsObjectsByHandle = this->priv->jsObjectsByHandle;
    jsvm_assert(jsObjectsByHandle != NULL);
    jsvm_assert(handle < env->CallIntMethod(jsObjectsByHandle, ArrayList_size));

    JSObjectWeakReference weakRef = (JSObjectWeakReference)
            env->CallObjectMethod(jsObjectsByHandle, ArrayList_get, handle);
    jsvm_assert(weakRef != NULL);

    // will be null if the JSObject has been GCed
    JSObject existingJSObject = (JSObject) env->CallObjectMethod(weakRef, JSObjectWeakReference_get);

    env->DeleteLocalRef(weakRef);

    return existingJSObject;
}

void ObjectBook::finalizeJSObjectWithHandle(JNIEnv *env, ObjectBook::handle_t handle) {
    jsvm_assert(ctx);
    jsvm_assert(env);

    // Delete the weak reference from jsObjectsByHandle
    ArrayList jsObjectsByHandle = this->priv->jsObjectsByHandle;
    jsvm_assert(jsObjectsByHandle != NULL);
    jsvm_assert(handle < env->CallIntMethod(jsObjectsByHandle, ArrayList_size));
    jsvm_assert(checkLocalRefIsNotNullAndRelease(env, env->CallObjectMethod(
            jsObjectsByHandle, ArrayList_get, handle)));

    // Forget the associated JSObject weak reference
    forgetJSObjectWithHandle(env, handle);

    // Remove it from the object book:

    // Get the object book
    duk_push_global_stash(ctx);
    duk_get_prop_index(ctx, -1, GLOBAL_STASH_INDEX_OBJECT_BOOK);

    // Get the object
    duk_get_prop_index(ctx, -1, handle);

    // Read its currently associated handle property
    handle_t objectCurrentHandle = this->getStackTopObjectHandle();

    // If the object still has this handle written in its
    // hidden symbol, unset it.
    // It may not have this handle if the object was given
    // a new handle before this one was finalized.
    if (objectCurrentHandle == handle) {
        // Reset the handle property in the object
        this->setStackTopObjectHandle(NULL_HANDLE);
    }

    duk_pop(ctx); // object

    // Overwrite the handle entry with null in the book:
    duk_push_null(ctx);
    duk_put_prop_index(ctx, -2, handle);

    // Restore the stack
    duk_pop_2(ctx); // book, global stash

    // Deallocate the handle so that it can be reused in later allocations
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
    jsvm_assert(duk_is_object(ctx, -1));

    // Remove the stack and book from the stack,
    // leaving the retrieved object on top of what was
    // the previous stack top.
    duk_copy(ctx, -1, -3);
    duk_pop_2(ctx);
}

ObjectBook::handle_t ObjectBook::allocateHandle(JNIEnv* env) {
    if (!m_freeList.empty()) {
        handle_t ret = m_freeList.top();
        m_freeList.pop();
        return ret;
    } else {
        // Expand the jsObjectsByHandle table one entry larger
        ArrayList jsObjectsByHandle = this->priv->jsObjectsByHandle;
        jsvm_assert(jsObjectsByHandle != NULL);
        env->CallBooleanMethod(jsObjectsByHandle, ArrayList_add, NULL);

        return m_nextFree++;
    }
}

void ObjectBook::deallocateHandle(ObjectBook::handle_t handle) {
    m_freeList.push(handle);
}

void ObjectBook::gcDeadJSObjects(JNIEnv *env) {
    jsvm_assert(env != NULL);
    ReferenceQueue deadJSObjectsRefs = (ReferenceQueue)
            env->GetObjectField(priv->jsVM, JSVM_deadJSObjectsRefs);
    jsvm_assert(deadJSObjectsRefs != NULL);

    // For each dead weak reference...
    JSObjectWeakReference nextReference = NULL;
    while (NULL != (nextReference = (JSObjectWeakReference)
            env->CallObjectMethod(deadJSObjectsRefs, ReferenceQueue_poll)))
    {
        // Recover the handle of the dead JSObject
        handle_t handle = (handle_t)
                env->GetIntField(nextReference, JSObjectWeakReference_handle);

        // Clean the resources associated with it
        finalizeJSObjectWithHandle(env, handle);

        env->DeleteLocalRef(nextReference);
    }
}
