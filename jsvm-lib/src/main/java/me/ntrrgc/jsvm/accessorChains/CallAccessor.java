package me.ntrrgc.jsvm.accessorChains;

import org.jetbrains.annotations.NotNull;

/**
 * Created by ntrrgc on 3/21/17.
 */

public final class CallAccessor extends TraversingAccessor {
    public CallAccessor(@NotNull AccessorChain parentChain) {
        super(parentChain);
    }

    @Override
    protected String joinWithOther(AccessorChain parentTraversing, boolean addEllipsis) {
        return parentTraversing.getFullPath() + (addEllipsis ? "..." : "") + "()";
    }

    @Override
    protected String joinWithClassChainRoot(ClassChainRoot parentClassChainRoot) {
        return parentClassChainRoot.className + ":()";
    }

    @Override
    protected String joinWithGlobalScope() {
        return "global()";
    }
}
