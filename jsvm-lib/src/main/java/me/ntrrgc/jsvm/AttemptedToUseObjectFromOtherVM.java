/* Copyright (c) the JSVM authors
 * https://github.com/ntrrgc/jsvm
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

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
