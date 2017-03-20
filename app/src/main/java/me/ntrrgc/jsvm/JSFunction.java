package me.ntrrgc.jsvm;

import org.jetbrains.annotations.NotNull;

/**
 * Created by ntrrgc on 1/24/17.
 */

public final class JSFunction extends JSObject {
    public JSValue invoke(@NotNull JSValue... args) {
        return call(JSValue.aNull(), args);
    }

    public JSValue call(@NotNull JSValue thisArg, @NotNull JSValue... args) {
        if (thisArg == null || args == null) {
            throw new IllegalArgumentException("Passed raw null pointer to JSFunction.call() instead of JSValue. Use JSValue.aNull() instead.");
        }
        for (JSValue arg : args) {
            if (arg == null) {
                throw new IllegalArgumentException("Passed raw null pointer to JSFunction.call() instead of JSValue. Use JSValue.aNull() instead.");
            }
        }

        synchronized (jsVM.lock) {
            if (!isStillAlive()) throw new UsedFinalizedJSObject(this);
            return callNative(jsVM, handle, thisArg, args);
        }
    }
    private native JSValue callNative(JSVM jsVM, int handle, JSValue thisArg, JSValue[] args);
}
