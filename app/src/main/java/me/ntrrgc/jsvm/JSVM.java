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

    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
    public static native String stringFromJNI();

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

    public native void finalize();
}
