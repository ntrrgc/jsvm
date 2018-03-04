/* Copyright (c) the JSVM authors
 * https://github.com/ntrrgc/jsvm
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

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
