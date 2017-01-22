package me.ntrrgc.jsvm;

/**
 * Created by ntrrgc on 1/15/17.
 */

public class JSObject {
    private JSVM jsVM;
    private int handle;
    private boolean aliveHandle = true;

    private JSObject() {}

    public JSValue get(String key) {
        if (!aliveHandle) throw new UsedFinalizedJSObject(this);
        synchronized (jsVM.lock) {
            return getByKeyNative(jsVM, handle, key);
        }
    }
    private native JSValue getByKeyNative(JSVM jsVM, int handle, String key);

    public JSValue get(int index) {
        if (!aliveHandle) throw new UsedFinalizedJSObject(this);
        synchronized (jsVM.lock) {
            return getByIndexNative(jsVM, handle, index);
        }
    }
    private native JSValue getByIndexNative(JSVM jsVM, int handle, int index);

    public void set(String key, JSValue value) {
        if (!aliveHandle) throw new UsedFinalizedJSObject(this);
        synchronized (jsVM.lock) {
            setByKeyNative(jsVM, handle, key, value);
        }
    }
    private native void setByKeyNative(JSVM jsVM, int handle, String key, JSValue value);

    public void set(int index, JSValue value) {
        if (!aliveHandle) throw new UsedFinalizedJSObject(this);
        synchronized (jsVM.lock) {
            setByIndexNative(jsVM, handle, index, value);
        }
    }
    private native void setByIndexNative(JSVM jsVM, int handle, int index, JSValue value);

    public boolean contains(String key) {
        if (!aliveHandle) throw new UsedFinalizedJSObject(this);
        synchronized (jsVM.lock) {
            return containsByKeyNative(jsVM, handle, key);
        }
    }
    private native boolean containsByKeyNative(JSVM jsVM, int handle, String key);

    public boolean contains(int index) {
        if (!aliveHandle) throw new UsedFinalizedJSObject(this);
        synchronized (jsVM.lock) {
            return containsByIndexNative(jsVM, handle, index);
        }
    }
    private native boolean containsByIndexNative(JSVM jsVM, int handle, int index);

    public String toString() {
        if (!aliveHandle) throw new UsedFinalizedJSObject(this);
        synchronized (jsVM.lock) {
            return this.toStringNative(jsVM, handle);
        }
    }
    private native String toStringNative(JSVM jsVM, int handle);

    public void finalize() {
        if (aliveHandle) {
            aliveHandle = false;
            finalizeNative(jsVM, handle);
        }
    }

    private native void finalizeNative(JSVM jsVM, int handle);
}
