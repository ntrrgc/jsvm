/* Copyright (c) the JSVM authors
 * https://github.com/ntrrgc/jsvm
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

package me.ntrrgc.jsvm.accessorChains;

/**
 * Created by ntrrgc on 3/21/17.
 */

public final class ClassChainRoot extends AccessorChainRoot {
    public final String className;

    public ClassChainRoot(String className) {
        this.className = className;
    }

    @Override
    public String getFullPath() {
        throw new RuntimeException();
    }
}
