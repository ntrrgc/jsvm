package me.ntrrgc.jsvm.accessorChains;

/**
 * Created by ntrrgc on 3/21/17.
 */

public class GlobalScopeChainRoot extends AccessorChainRoot {
    @Override
    public String getFullPath() {
        return "global";
    }
}
