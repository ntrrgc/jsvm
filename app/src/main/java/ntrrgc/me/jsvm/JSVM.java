package ntrrgc.me.jsvm;

import java.util.List;

/**
 * Created by ntrrgc on 1/14/17.
 */
public class JSVM {

    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("native-lib");
        initialize();
    }

    public static native void initialize();
    public static native void destroy();

    private long hDukContext;

    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
    public static native String stringFromJNI();

    private native void nativeInit();

    public JSVM() {
        nativeInit();
    }

    public native JSValue evaluateScript(String code);

    public native void finalize();
}
