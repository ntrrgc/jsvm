package me.ntrrgc.jsvm;

/**
 * Created by ntrrgc on 1/15/17.
 */

public final class AttemptedToUseObjectFromOtherVM extends RuntimeException {
    public final JSObject usedObject;
    public final JSVM usedObjectVM;

    public AttemptedToUseObjectFromOtherVM(JSObject usedObject, JSVM usedObjectVM) {
        super("Attempted to use a JSObject in a different JSVM");
        this.usedObject = usedObject;
        this.usedObjectVM = usedObjectVM;
    }
}
