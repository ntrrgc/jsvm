package me.ntrrgc.jsvm.accessorChains;

import org.jetbrains.annotations.NotNull;

/**
 * Created by ntrrgc on 3/21/17.
 */

abstract class TraversingAccessor implements AccessorChain {
    @NotNull
    private final AccessorChain parentChain;

    private final int depthRemaining;

    TraversingAccessor(@NotNull AccessorChain parentChain) {
        if (parentChain == null) throw new AssertionError();
        if (parentChain.getDepthRemaining() > 0) {
            this.parentChain = parentChain;
        } else {
            // depth limit exceeded, link to the grandparent instead in order for the depth to
            // remain bound.
            this.parentChain = ((TraversingAccessor) parentChain).parentChain;
        }

        this.depthRemaining = this.parentChain.getDepthRemaining() - 1;
    }

    abstract protected String joinWithGlobalScope();
    abstract protected String joinWithClassChainRoot(ClassChainRoot parentClassChainRoot);
    abstract protected String joinWithOther(AccessorChain parentTraversing, boolean addEllipsis);

    @Override
    public String getFullPath() {
        if (parentChain instanceof ClassChainRoot) {
            return joinWithClassChainRoot((ClassChainRoot) parentChain);
        } else if (parentChain instanceof GlobalScopeChainRoot) {
            return joinWithGlobalScope();
        } else {
            return joinWithOther(parentChain, this.depthRemaining == 0);
        }
    }

    @Override
    public int getDepthRemaining() {
        return depthRemaining;
    }
}
