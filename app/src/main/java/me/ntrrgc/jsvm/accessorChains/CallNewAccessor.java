package me.ntrrgc.jsvm.accessorChains;

import org.jetbrains.annotations.NotNull;

/**
 * Created by ntrrgc on 3/21/17.
 */

public final class CallNewAccessor extends TraversingAccessor {
    public CallNewAccessor(@NotNull AccessorChain parentChain) {
        super(parentChain);
    }

    @Override
    protected String joinWithOther(AccessorChain parentTraversing, boolean addEllipsis) {
        return "new (" + parentTraversing.getFullPath() + (addEllipsis ? "..." : "") + ")()";
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
