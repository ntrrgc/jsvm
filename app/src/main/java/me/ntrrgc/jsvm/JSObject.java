package me.ntrrgc.jsvm;

import org.jetbrains.annotations.Contract;
import org.jetbrains.annotations.NotNull;
import org.jetbrains.annotations.Nullable;

import java.io.Closeable;

import me.ntrrgc.jsvm.accessorChains.AccessorChain;
import me.ntrrgc.jsvm.accessorChains.IndexAccessor;
import me.ntrrgc.jsvm.accessorChains.PropertyAccessor;

/**
 * Created by ntrrgc on 1/15/17.
 */

public class JSObject implements Closeable {
    protected JSVM jsVM;
    protected int handle;
    private boolean aliveHandle = true;

    protected AccessorChain accessorChain;

    @NotNull
    JSObject lateInitAccessorChain(@NotNull AccessorChain accessorChain) {
        // Note: may be initialized more than once if the same object is retrieved in several ways.
        // The latest one persists.
        this.accessorChain = accessorChain;
        return this;
    }

    protected boolean isStillAlive() {
        return aliveHandle && !jsVM.finalized;
    }

    protected JSObject() {}

    public JSValue get(String key) {
        synchronized (jsVM.lock) {
            if (!isStillAlive()) throw new UsedFinalizedJSObject(this);
            return getByKeyNative(jsVM, handle, key)
                    // adds instrumentation to track where the obtained JSValue came from
                    .lateInitAccessorChain(new PropertyAccessor(accessorChain, key));
        }
    }
    private native JSValue getByKeyNative(JSVM jsVM, int handle, String key);

    public JSValue get(int index) {
        synchronized (jsVM.lock) {
            if (!isStillAlive()) throw new UsedFinalizedJSObject(this);
            return getByIndexNative(jsVM, handle, index)
                    // adds instrumentation to track where the obtained JSValue came from
                    .lateInitAccessorChain(new IndexAccessor(accessorChain, index));
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

    public JSValue invokeMethod(@NotNull String methodName, @NotNull JSValue... args) {
        JSFunction methodFunction = this.get(methodName).asFunction();
        return methodFunction.call(JSValue.anObject(this), args);
    }

    public String toString() {
        synchronized (jsVM.lock) {
            if (!isStillAlive()) throw new UsedFinalizedJSObject(this);
            return this.toStringNative(jsVM, handle);
        }
    }
    private native String toStringNative(JSVM jsVM, int handle);

    public void close() {
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

    /**
     * Attempts to get the name of the constructor function (or class) this object was constructed
     * from. This is done by reading .constructor.name. The {@link InvalidJSValueType} error
     * uses this method to extract useful class names.
     *
     * This will fail for bare objects created with Object.create(null) or objects where those
     * properties have been deleted, but those conditions are pretty rare.
     *
     * Anonymous objects will have "Object" as their class name.
     *
     * If such accesses would trigger a JavaScript error, the error is silently thrown away
     * and null is returned.
     *
     * @return The name of the constructor function, or null if it could not be retrieved.
     */
    @Nullable
    public String getClassName() {
        synchronized (jsVM.lock) {
            if (!isStillAlive()) throw new UsedFinalizedJSObject(this);
            try {
                return this.getClassNameNative(jsVM, handle);
            } catch (JSError ignored) {
                return null;
            }
        }
    }
    private native String getClassNameNative(JSVM jsVM, int handle);

    /**
     * Like getClassName(), but never returns null.
     *
     * @return A printable class name.
     */
    String getRepresentableClassName() {
        String className = getClassName();
        if (className != null) {
            return className;
        } else {
            return "<unknown object>";
        }
    }
}
