package me.ntrrgc.jsvm;

/**
 * Created by ntrrgc on 1/15/17.
 */

public class JSObject {
    protected JSVM jsVM;
    protected int handle;
    private boolean aliveHandle = true;

    protected boolean isStillAlive() {
        return aliveHandle && !jsVM.finalized;
    }

    protected JSObject() {}

    public JSValue get(String key) {
        synchronized (jsVM.lock) {
            if (!isStillAlive()) throw new UsedFinalizedJSObject(this);
            return getByKeyNative(jsVM, handle, key);
        }
    }
    private native JSValue getByKeyNative(JSVM jsVM, int handle, String key);

    public JSValue get(int index) {
        synchronized (jsVM.lock) {
            if (!isStillAlive()) throw new UsedFinalizedJSObject(this);
            return getByIndexNative(jsVM, handle, index);
        }
    }
    private native JSValue getByIndexNative(JSVM jsVM, int handle, int index);

    public void set(String key, JSValue value) {
        synchronized (jsVM.lock) {
            if (!isStillAlive()) throw new UsedFinalizedJSObject(this);
            setByKeyNative(jsVM, handle, key, value);
        }
    }
    private native void setByKeyNative(JSVM jsVM, int handle, String key, JSValue value);

    public void set(int index, JSValue value) {
        synchronized (jsVM.lock) {
            if (!isStillAlive()) throw new UsedFinalizedJSObject(this);
            setByIndexNative(jsVM, handle, index, value);
        }
    }
    private native void setByIndexNative(JSVM jsVM, int handle, int index, JSValue value);

    public boolean contains(String key) {
        synchronized (jsVM.lock) {
            if (!isStillAlive()) throw new UsedFinalizedJSObject(this);
            return containsByKeyNative(jsVM, handle, key);
        }
    }
    private native boolean containsByKeyNative(JSVM jsVM, int handle, String key);

    public boolean contains(int index) {
        synchronized (jsVM.lock) {
            if (!isStillAlive()) throw new UsedFinalizedJSObject(this);
            return containsByIndexNative(jsVM, handle, index);
        }
    }
    private native boolean containsByIndexNative(JSVM jsVM, int handle, int index);

    public String toString() {
        synchronized (jsVM.lock) {
            if (!isStillAlive()) throw new UsedFinalizedJSObject(this);
            return this.toStringNative(jsVM, handle);
        }
    }
    private native String toStringNative(JSVM jsVM, int handle);

    protected void finalize() {
        synchronized (jsVM.lock) {
            // If the entire VM was already destroyed in this
            // same GC cycle there is no point in trying to
            // destroy individual referenced objects.
            if (isStillAlive()) {
                aliveHandle = false;

                if (!jsVM.finalized) {
                    finalizeNative(jsVM, handle);
                }
            }
        }
    }
    private native void finalizeNative(JSVM jsVM, int handle);
}
