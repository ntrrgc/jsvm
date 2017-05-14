package me.ntrrgc.jsvm;

import org.jetbrains.annotations.NotNull;

/**
 * Created by ntrrgc on 1/15/17.
 */

public final class AttemptedToUseObjectFromOtherVM extends RuntimeException {
    public final @NotNull JSObject usedObject;
    public final @NotNull JSVM usedObjectVM;

    public AttemptedToUseObjectFromOtherVM(@NotNull JSObject usedObject, @NotNull JSVM usedObjectVM) {
        super("Attempted to use a JSObject in a different JSVM");
        this.usedObject = usedObject;
        this.usedObjectVM = usedObjectVM;
    }
}
