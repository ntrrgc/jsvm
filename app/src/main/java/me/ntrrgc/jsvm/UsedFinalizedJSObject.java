package me.ntrrgc.jsvm;

/**
 * Created by ntrrgc on 1/15/17.
 */

final public class UsedFinalizedJSObject extends RuntimeException {
    public final JSObject accessedJSObject;

    UsedFinalizedJSObject(JSObject jsObject) {
        super("Attempted to use finalized jsObject");
        accessedJSObject = jsObject;
    }
}
