/* Copyright (c) the JSVM authors
 * https://github.com/ntrrgc/jsvm
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

package me.ntrrgc.jsvm.accessorChains;

import org.jetbrains.annotations.Contract;

import me.ntrrgc.jsvm.JSTypeError;

/**
 * A linked list of instances of this class is used to construct useful accessor chains
 * for {@link JSTypeError} errors.
 *
 * An accessor is either a property or a method, represented in accessorString as "myProp" or
 * "getMyProp()". Traversing these nodes and joining strings backwards allows you to recover
 * access strings like this:
 *
 *  pokedex.pokemon().experience
 *
 * A special kind of node is the root class accessor, which is used to start a chain when the
 * root object does not come from reading a property of the global scope (AKA a global variable).
 * That node has isRootClassNode set to true and its accessorString is the name of the constructor
 * function (AKA class) that created it. When formatted as a path, the root class accessor gets a
 * colon instead of a dot, in order to make clear we're not referring to a global variable.
 *
 *  Pokedex:pokemon().experience
 *
 * Created by ntrrgc on 3/20/17.
 */

public interface AccessorChain {
    @Contract(pure = true)
    String getFullPath();

    @Contract(pure = true)
    int getDepthRemaining();
}
