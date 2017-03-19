package me.ntrrgc.jsvm;

import org.jetbrains.annotations.NotNull;
import org.jetbrains.annotations.Nullable;

import java.lang.ref.ReferenceQueue;
import java.lang.ref.WeakReference;
import java.util.ArrayList;

/**
 * Created by ntrrgc on 1/14/17.
 */
public class JSVM {

    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("native-lib");
        initializeLibrary();
    }

    /**
     * Initializes the JNI-side.
     */
    public static native void initializeLibrary();

    /**
     * Called to tear down the library.
     *
     * TODO This should not be public and only be called if the library is unloaded.
     */
    public static native void destroyLibrary();

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
    private ArrayList<JSObjectWeakReference> jsObjectsByHandle = new ArrayList<>();

    /**
     * When JSObjects are GCed, their {@link JSObjectWeakReference},
     * which contain their handles, are put in this queue. The
     * object book checks this queue in order to release JS object
     * references (so their memory is reclaimed if also no references
     * exist in JS code) and reuse handles.
     *
     * Accessed from JNI.
     */
    private ReferenceQueue<JSObject> deadJSObjectsRefs = new ReferenceQueue<>();

    /**
     * Synchronization monitor used so that two threads never
     * use the same Duktape heap simultaneously.
     */
    /* package */ final Object lock = new Object();

    // Dummy method used in some performance tests to measure JNI overhead.
    public static native double returnADouble();

    private native void nativeInit();

    public JSVM() {
        nativeInit();
    }

    public JSValue evaluateScript(String code) {
        synchronized (this.lock) {
            return evaluateScriptNative(code);
        }
    }
    private native JSValue evaluateScriptNative(String code);

    /* package */ boolean finalized = false;
    protected void finalize() {
        synchronized (this.lock) {
            this.finalizeNative();
            finalized = true;
        }
    }
    private native void finalizeNative();

    public int getStackFrameSize() {
        synchronized (this.lock) {
            return this.getStackFrameSizeNative();
        }
    }
    private native int getStackFrameSizeNative();

    public int getStackSize() {
        synchronized (this.lock) {
            return this.getStackSizeNative();
        }
    }
    private native int getStackSizeNative();

    @NotNull
    public JSObject newObject() {
        synchronized (this.lock) {
            return this.newObjectNative();
        }
    }
    private native JSObject newObjectNative();

    @NotNull
    public JSObject newObjectWithProto(@Nullable JSObject proto) {
        synchronized (this.lock) {
            return this.newObjectNativeWithProto(proto);
        }
    }
    private native JSObject newObjectNativeWithProto(JSObject proto);

    public native int getWeakRefCount();
}
