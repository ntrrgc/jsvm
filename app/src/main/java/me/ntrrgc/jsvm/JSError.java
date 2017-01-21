package me.ntrrgc.jsvm;

import org.jetbrains.annotations.NotNull;

/**
 * This exception encapsulates a JS exception.
 *
 * This exception is thrown by JSVM is throw when a JS
 * exception escapes the JS VM and propagates into Java
 * code.
 *
 * Throwing this exception from Java method that was
 * called from JSVM will cause the provided JSValue
 * to be thrown to the JS caller.
 *
 * Created by ntrrgc on 1/21/17.
 */
public final class JSError extends RuntimeException {
    // Fun fact: jsError may be not be a JS object.
    // Albeit less helpful, `throw "whatever";`, or even
    // `throw undefined;` are valid JavaScript.
    private final JSValue jsError;

    public JSError(@NotNull JSValue jsError) {
        super(extractErrorMessage(jsError));
        this.jsError = jsError;
    }

    public JSValue getJsError() {
        return jsError;
    }

    @NotNull
    private static String extractErrorMessage(@NotNull JSValue jsError) {
        JSObject jsErrorObject = jsError.asObject();
        if (jsErrorObject != null) {
            return jsErrorObject.get("message").toString();
        } else {
            return jsError.toString();
        }
    }
}
