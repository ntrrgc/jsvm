/* Copyright (c) the JSVM authors
 * https://github.com/ntrrgc/jsvm
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

package me.ntrrgc.jsvm;

import org.jetbrains.annotations.NotNull;

/**
 * These methods allow doing things that normally would not be possible for users of JSVM due
 * to package-level visibility, but are useful for the JSVM unit tests.
 */
public class TestUtils {
    public static int getAliveCallableCount(@NotNull JSVM jsvm) {
        return jsvm.callableAllocator.countAliveHandles();
    }
}
