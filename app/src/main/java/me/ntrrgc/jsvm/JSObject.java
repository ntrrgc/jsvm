package me.ntrrgc.jsvm;

/**
 * Created by ntrrgc on 1/15/17.
 */

public class JSObject {
    private JSVM jsVM;
    private int jsHandle;
    private boolean aliveHandle = true;

    private JSObject() {}

    public String toString() {
        if (!aliveHandle) {
            throw new UsedFinalizedJSObject(this);
        }

        synchronized (jsVM.lock) {
            return this.toStringNative(jsVM.hPriv, jsHandle);
        }
    }

    private native String toStringNative(long hDukContext, int jsHandle);

    public void finalize() {
        if (aliveHandle) {
            aliveHandle = false;
            finalizeNative();
        }
    }

    private native void finalizeNative();
}
