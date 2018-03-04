/* Copyright (c) the JSVM authors
 * https://github.com/ntrrgc/jsvm
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

package me.ntrrgc.jsvm;

public class JSVMBuilder {
    private boolean accessorChainsEnabled = true;

    public final JSVMBuilder setAccessorChainsEnabled(boolean accessorChainsEnabled) {
        this.accessorChainsEnabled = accessorChainsEnabled;
        return this;
    }

    public final JSVM createJSVM() {
        return new JSVM(accessorChainsEnabled);
    }
}