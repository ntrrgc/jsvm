package me.ntrrgc.jsvm;

import java.lang.ref.ReferenceQueue;
import java.lang.ref.WeakReference;

/**
 * Created by ntrrgc on 1/29/17.
 */

/* package */ final class JSObjectWeakReference extends WeakReference<JSObject> {
    // Read in JNI
    private int handle;

    public JSObjectWeakReference(JSObject referent, ReferenceQueue<? super JSObject> q, int handle) {
        super(referent, q);
        this.handle = handle;
    }
}
