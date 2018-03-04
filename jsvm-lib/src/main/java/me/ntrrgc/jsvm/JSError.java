/* Copyright (c) the JSVM authors
 * https://github.com/ntrrgc/jsvm
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

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
    // Fun fact: errorValue may be not be a object.
    // Albeit less helpful, `throw "whatever";`, or even
    // `throw undefined;` are valid JavaScript.
    @NotNull
    private final JSValue errorValue;

    public JSError(@NotNull JSValue errorValue) {
        super(extractErrorMessage(errorValue.lateInitAccessorChainDefault()));
        this.errorValue = errorValue;
    }

    @NotNull
    public final JSValue getErrorValue() {
        return errorValue;
    }

    @NotNull
    private static String extractErrorMessage(@NotNull JSValue jsError) {
        JSObject jsErrorObject = jsError.asObjectOrNull();
        if (jsErrorObject != null) {
            return jsErrorObject.get("message").toString();
        } else {
            return jsError.toString();
        }
    }
}
