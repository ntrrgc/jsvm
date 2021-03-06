/* Copyright (c) the JSVM authors
 * https://github.com/ntrrgc/jsvm
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

package me.ntrrgc.jsvm;

import org.jetbrains.annotations.NotNull;
import org.jetbrains.annotations.Nullable;

import java.lang.ref.ReferenceQueue;
import java.util.ArrayList;

import me.ntrrgc.jsvm.accessorChains.ClassChainRoot;
import me.ntrrgc.jsvm.accessorChains.EvalExpressionChainRoot;
import me.ntrrgc.jsvm.accessorChains.FunctionArgumentChainRoot;
import me.ntrrgc.jsvm.accessorChains.FunctionThisChainRoot;
import me.ntrrgc.jsvm.accessorChains.GlobalScopeChainRoot;

/**
 * Created by ntrrgc on 1/14/17.
 */
public class JSVM {

    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("jsvm");
        initializeLibrary();
    }

    /**
     * Initializes the JNI-side.
     */
    public final static native void initializeLibrary();

    /**
     * Called to tear down the library.
     *
     * TODO This should not be public and only be called if the library is unloaded.
     */
    public final static native void destroyLibrary();

    /**
     * JSVMPriv* (contains duk_context* and other implementation fields)
     *
     * Accessed from JNI.
     */
    private long hPriv;

    /**
     * Allows getting an existing JSObject from its handle.
     * Weak references are used, so they can still be GC'ed.
     *
     * Accessed from JNI.
     */
    private final ArrayList<JSObjectWeakReference> jsObjectsByHandle = new ArrayList<>();

    /**
     * When JSObjects are GCed, their {@link JSObjectWeakReference},
     * which contain their handles, are put in this queue. The
     * object book checks this queue in order to release JS object
     * references (so their memory is reclaimed if also no references
     * exist in JS code) and reuse handles.
     *
     * Accessed from JNI.
     */
    private final ReferenceQueue<JSObject> deadJSObjectsRefs = new ReferenceQueue<>();

    /**
     * Synchronization monitor used so that two threads never
     * use the same Duktape heap simultaneously.
     */
    /* package */ final Object lock = new Object();

    /* package */ final boolean accessorChainsEnabled;

    /**
     * Manages Java callables accessible from JS.
     */
    /* package */ final HandleAllocator<JSCallable> callableAllocator = new HandleAllocator<>();

    /**
     * Called from JNI to unref Java callables GCed in JS.
     */
    private void finalizeCallable(int callableHandle) {
        callableAllocator.free(callableHandle);
    }

    // Dummy method used in some performance tests to measure JNI overhead.
    public static native double returnADouble();

    private native void nativeInit();

    public JSVM() {
        this.accessorChainsEnabled = true;
        nativeInit();
    }

    JSVM(boolean accessorChainsEnabled) {
        this.accessorChainsEnabled = accessorChainsEnabled;
        nativeInit();
    }

    @NotNull
    public final JSValue evaluate(String code) {
        synchronized (this.lock) {
            return evaluateNative(code)
                    .lateInitAccessorChain(new EvalExpressionChainRoot(code));
        }
    }
    private native JSValue evaluateNative(String code);

    /* package */ boolean finalized = false;
    protected final void finalize() {
        synchronized (this.lock) {
            this.finalizeNative();
            finalized = true;
        }
    }
    private native void finalizeNative();

    public final int getStackFrameSize() {
        synchronized (this.lock) {
            return this.getStackFrameSizeNative();
        }
    }
    private native int getStackFrameSizeNative();

    public final int getStackSize() {
        synchronized (this.lock) {
            return this.getStackSizeNative();
        }
    }
    private native int getStackSizeNative();

    /**
     * Get the object backing the global scope, where global variables are stored.
     *
     * @return the JS object backing the global scope.
     */
    @NotNull
    public final JSObject getGlobalScope() {
        synchronized (this.lock) {
            JSObject global = this.getGlobalScopeNative();
            if (accessorChainsEnabled) {
                global.lateInitAccessorChain(GlobalScopeChainRoot.INSTANCE);
            }
            return global;
        }
    }
    private native JSObject getGlobalScopeNative();

    /**
     * Create a new empty object inheriting from Object.
     *
     * This is similar to evaluating {} in JavaScript.
     *
     * @return The newly created object.
     */
    @NotNull
    public final JSObject newObject() {
        synchronized (this.lock) {
            JSObject obj = this.newObjectNative();
            if (accessorChainsEnabled) {
                obj.lateInitAccessorChain(new ClassChainRoot(obj.getRepresentableClassName()));
            }
            return obj;
        }
    }
    private native JSObject newObjectNative();

    /**
     * Create a new object with the specified prototype.
     *
     * This is similar to calling Object.create(proto) in JavaScript.
     *
     * @param proto The prototype for the new object. Passing a null here creates a bare object that
     *              does not inherit from JavaScript's Object base class.
     * @return The newly created object.
     */
    @NotNull
    public final JSObject newObjectWithProto(@Nullable JSObject proto) {
        synchronized (this.lock) {
            JSObject obj = this.newObjectNativeWithProto(proto);
            if (accessorChainsEnabled) {
                obj.lateInitAccessorChain(new ClassChainRoot(obj.getRepresentableClassName()));
            }
            return obj;
        }
    }
    private native JSObject newObjectNativeWithProto(JSObject proto);

    @NotNull
    public final JSFunction newFunction(@NotNull JSCallable callable) {
        synchronized (this.lock) {
            int callableHandle = callableAllocator.allocate(callable);

            JSFunction function = this.newFunctionNative(callableHandle);
            if (accessorChainsEnabled) {
                function.lateInitAccessorChain(new ClassChainRoot(function.getRepresentableClassName()));
            }
            return function;
        }
    }
    private native JSFunction newFunctionNative(int callableHandle);

    /**
     * This function is called from JNI when Duktape tries to call a native function.
     */
    private JSValue callNative(int callableHandle, JSValue thisArg, JSValue[] args) {
        // Initialize accessor chains for parameters
        thisArg.lateInitAccessorChain(FunctionThisChainRoot.INSTANCE);
        int i = 0;
        for (JSValue arg: args) {
            arg.lateInitAccessorChain(FunctionArgumentChainRoot.getInstance(i));
            i++;
        }
        try {
            return callableAllocator.get(callableHandle).call(args, thisArg, this);
        } catch (JSError jsError) {
            throw jsError;
        } catch (Throwable unexpectedError) {
            // TODO: Use custom subclass of JS' Error.
            // TODO: Retrieve stacktrace from the point callNative() was called.
            JSObject jsErrorObject = getGlobalScope().get("Error").asFunction()
                    .callNew(JSValue.aString("Java callable threw an exception: " + unexpectedError.getMessage()))
                    .asObject();
            throw new JSError(JSValue.anObject(jsErrorObject));
        }
    }

    public native int getWeakRefCount();
}
