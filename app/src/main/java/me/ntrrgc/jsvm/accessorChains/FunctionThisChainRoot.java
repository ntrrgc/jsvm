package me.ntrrgc.jsvm.accessorChains;

/**
 * Created by ntrrgc on 3/21/17.
 */

public final class FunctionThisChainRoot extends AccessorChainRoot {
    private FunctionThisChainRoot() {
        super();
    }

    public final static FunctionThisChainRoot INSTANCE = new FunctionThisChainRoot();

    @Override
    public String getFullPath() {
        return "this";
    }
}
