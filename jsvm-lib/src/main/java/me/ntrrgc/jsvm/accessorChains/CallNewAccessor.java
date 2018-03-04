/* Copyright (c) the JSVM authors
 * https://github.com/ntrrgc/jsvm
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

package me.ntrrgc.jsvm.accessorChains;

import org.jetbrains.annotations.NotNull;

/**
 * Created by ntrrgc on 3/21/17.
 */

public final class CallNewAccessor extends TraversingAccessor {
    public CallNewAccessor(@NotNull AccessorChain parentChain) {
        super(parentChain);
    }

    private boolean needsParentheses(@NotNull AccessorChain parent) {
        // Parentheses can be omitted when all the parent accessors are property accesses,
        // index accesses or the global root.
        while (true) {
            if (!(parent instanceof PropertyAccessor ||
                    parent instanceof IndexAccessor ||
            parent instanceof GlobalScopeChainRoot)) {
                return true;
            }

            if (parent instanceof TraversingAccessor) {
                parent = ((TraversingAccessor)parent).parentChain;
            } else {
                return false;
            }
        }
    }

    @Override
    protected String joinWithOther(AccessorChain parentTraversing, boolean addEllipsis) {
        if (needsParentheses(parentTraversing)) {
            return "new (" + parentTraversing.getFullPath() + (addEllipsis ? "..." : "") + ")()";
        } else {
            return "new " + parentTraversing.getFullPath() + (addEllipsis ? "..." : "") + "()";
        }
    }

    @Override
    protected String joinWithClassChainRoot(ClassChainRoot parentClassChainRoot) {
        return "new " + parentClassChainRoot.className + "()";
    }

    @Override
    protected String joinWithGlobalScope() {
        return "new global()";
    }
}
