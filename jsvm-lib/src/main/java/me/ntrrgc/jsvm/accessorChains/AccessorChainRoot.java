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

abstract class AccessorChainRoot implements AccessorChain {
    AccessorChainRoot() {}

    @Override
    public int getDepthRemaining() {
        return 8;
    }
}
