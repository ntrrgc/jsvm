package me.ntrrgc.jsvm.accessorChains;

import org.jetbrains.annotations.NotNull;

/**
 * Created by ntrrgc on 3/21/17.
 */

public final class PropertyAccessor extends TraversingAccessor {
    final private String propertyName;

    public PropertyAccessor(@NotNull AccessorChain parentChain, String propertyName) {
        super(parentChain);
        this.propertyName = propertyName;
    }

    @Override
    protected String joinWithOther(AccessorChain parentTraversing, boolean addEllipsis) {
        return parentTraversing.getFullPath() + (addEllipsis ? "..." : ".") + propertyName;
    }

    @Override
    protected String joinWithClassChainRoot(ClassChainRoot parentClassChainRoot) {
        return parentClassChainRoot.className + ":" + propertyName;
    }

    @Override
    protected String joinWithGlobalScope() {
        return propertyName;
    }
}
