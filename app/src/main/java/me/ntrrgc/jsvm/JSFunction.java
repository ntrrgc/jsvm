package me.ntrrgc.jsvm;

import org.jetbrains.annotations.NotNull;

import java.lang.ref.ReferenceQueue;

import me.ntrrgc.jsvm.accessorChains.CallAccessor;
import me.ntrrgc.jsvm.accessorChains.NewCallAccessor;

/**
 * Created by ntrrgc on 1/24/17.
 */

public final class JSFunction extends JSObject {
    /**
     * Call the function with the specified arguments.
     *
     * The function is run with its implicit `this` argument set to undefined.
     * This is consistent with regular function calls under strict mode in ES5.
     *
     * @param args Arguments to pass to the JavaScript function.
     * @return The return value of the JavaScript function.
     */
    public JSValue invoke(@NotNull JSValue... args) {
        return call(JSValue.anUndefined(), args);
    }

    /**
     * Call the function with the specified arguments and `this` argument.
     *
     * This is similar to calling a function using Function.prototype.call()
     * in JavaScript.
     *
     * @param thisArg Value to pass as `this` argument, usually an object or undefined.
     * @param args Arguments to pass to the JavaScript function.
     * @return The return value of the JavaScript function.
     */
    public JSValue call(@NotNull JSValue thisArg, @NotNull JSValue... args) {
        if (thisArg == null) {
            throw new IllegalArgumentException("Passed raw null pointer to JSFunction.call() instead of JSValue. Use JSValue.aNull() instead.");
        }
        for (JSValue arg : args) {
            if (arg == null) {
                throw new IllegalArgumentException("Passed raw null pointer to JSFunction.call() instead of JSValue. Use JSValue.aNull() instead.");
            }
        }

        synchronized (jsVM.lock) {
            if (!isStillAlive()) throw new UsedFinalizedJSObject(this);
            return callNative(jsVM, handle, thisArg, args)
                    .lateInitAccessorChain(new CallAccessor(accessorChain));
        }
    }
    private native JSValue callNative(JSVM jsVM, int handle, JSValue thisArg, JSValue[] args);

    /**
     * Call the function as a constructor with the specified arguments.
     *
     * This is similar to calling a function using the new operator in JavaScript.
     *
     * @param args Arguments to pass to the JavaScript constructor function.
     * @return The constructed object.
     */
    public JSValue callNew(@NotNull JSValue... args) {
        for (JSValue arg : args) {
            if (arg == null) {
                throw new IllegalArgumentException("Passed raw null pointer to JSFunction.call() instead of JSValue. Use JSValue.aNull() instead.");
            }
        }

        synchronized (jsVM.lock) {
            if (!isStillAlive()) throw new UsedFinalizedJSObject(this);
            return callNewNative(jsVM, handle, args)
                    .lateInitAccessorChain(new NewCallAccessor(accessorChain));
        }
    }
    private native JSValue callNewNative(JSVM jsVM, int handle, JSValue[] args);
}
