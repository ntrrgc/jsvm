package me.ntrrgc.jsvm.accessorChains;

/**
 * Created by ntrrgc on 3/21/17.
 */

public final class GlobalScopeChainRoot extends AccessorChainRoot {
    private GlobalScopeChainRoot() {
        super();
    }

    public final static GlobalScopeChainRoot INSTANCE = new GlobalScopeChainRoot();

    @Override
    public String getFullPath() {
        return "global";
    }
}
