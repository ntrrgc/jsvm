package me.ntrrgc.jsvm;

import org.jetbrains.annotations.NotNull;

/**
 * Created by ntrrgc on 3/22/17.
 */

public final class JSNativeFunction extends JSFunction {
    JSNativeFunction(@NotNull JSVM jsVM, int handle) {
        super(jsVM, handle);
    }
}
