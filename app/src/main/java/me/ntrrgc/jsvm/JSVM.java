package me.ntrrgc.jsvm;

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
     * Native (duk_context *)
     */
    private long hPriv;

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
}
