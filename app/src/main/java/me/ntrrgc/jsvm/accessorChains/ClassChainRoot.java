package me.ntrrgc.jsvm.accessorChains;

/**
 * Created by ntrrgc on 3/21/17.
 */

public class ClassChainRoot extends AccessorChainRoot {
    public final String className;

    public ClassChainRoot(String className) {
        this.className = className;
    }

    @Override
    public String getFullPath() {
        throw new RuntimeException();
    }
}
