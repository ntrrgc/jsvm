package me.ntrrgc.jsvm;

/**
 * Created by ntrrgc on 1/15/17.
 */

public class JSObject {
    private JSVM jsVM;
    private int handle;
    private boolean aliveHandle = true;

    private JSObject() {}

    public String toString() {
        if (!aliveHandle) {
            throw new UsedFinalizedJSObject(this);
        }

        synchronized (jsVM.lock) {
            return this.toStringNative(jsVM.hPriv, handle);
        }
    }

    private native String toStringNative(long hPriv, int handle);

    public void finalize() {
        if (aliveHandle) {
            aliveHandle = false;
            finalizeNative(jsVM.hPriv, handle);
        }
    }

    private native void finalizeNative(long hPriv, int handle);
}
