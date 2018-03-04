/* Copyright (c) the JSVM authors
 * https://github.com/ntrrgc/jsvm
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

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
