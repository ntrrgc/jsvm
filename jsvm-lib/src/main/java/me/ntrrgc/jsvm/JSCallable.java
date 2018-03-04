/* Copyright (c) the JSVM authors
 * https://github.com/ntrrgc/jsvm
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

package me.ntrrgc.jsvm;

import org.jetbrains.annotations.NotNull;

/**
 * Created by ntrrgc on 3/22/17.
 */

public interface JSCallable {
    @NotNull
    JSValue call(@NotNull JSValue[] args, @NotNull JSValue thisArg, @NotNull JSVM jsvm);
}
