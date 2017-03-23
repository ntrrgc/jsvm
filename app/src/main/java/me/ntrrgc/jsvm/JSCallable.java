package me.ntrrgc.jsvm;

import org.jetbrains.annotations.NotNull;

/**
 * Created by ntrrgc on 3/22/17.
 */

public interface JSCallable {
    @NotNull
    JSValue call(@NotNull JSValue[] args, @NotNull JSValue thisArg, @NotNull JSVM jsvm);
}
