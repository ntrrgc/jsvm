package me.ntrrgc.jsvm;

import org.jetbrains.annotations.NotNull;

/**
 * Created by ntrrgc on 1/24/17.
 */

public final class JSFunction extends JSObject {
    public JSValue invoke(JSValue... args) {
        return call(JSValue.aNull(), args);
    }

    public JSValue call(JSValue thisArg, JSValue... args) {
        synchronized (jsVM.lock) {
            if (!isStillAlive()) throw new UsedFinalizedJSObject(this);
            return callNative(jsVM, handle, thisArg, args);
        }
    }
    private native JSValue callNative(JSVM jsVM, int handle, JSValue thisArg, JSValue[] args);
}
