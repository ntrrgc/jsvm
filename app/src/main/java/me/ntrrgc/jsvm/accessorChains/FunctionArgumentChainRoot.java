package me.ntrrgc.jsvm.accessorChains;

/**
 * Created by ntrrgc on 3/21/17.
 */

public final class FunctionArgumentChainRoot extends AccessorChainRoot {
    private final int argumentIndex;

    private FunctionArgumentChainRoot(int argumentIndex) {
        super();
        this.argumentIndex = argumentIndex;
    }

    private final static FunctionArgumentChainRoot[] INSTANCES = new FunctionArgumentChainRoot[] {
            new FunctionArgumentChainRoot(0),
            new FunctionArgumentChainRoot(1),
            new FunctionArgumentChainRoot(2),
            new FunctionArgumentChainRoot(3),
            new FunctionArgumentChainRoot(4),
            new FunctionArgumentChainRoot(5),
            new FunctionArgumentChainRoot(6),
            new FunctionArgumentChainRoot(7),
    };

    public static FunctionArgumentChainRoot getInstance(int argumentIndex) {
        if (argumentIndex < 8) {
            return INSTANCES[argumentIndex];
        } else {
            return new FunctionArgumentChainRoot(argumentIndex);
        }
    }

    @Override
    public String getFullPath() {
        return "arg[" + argumentIndex + "]";
    }
}
