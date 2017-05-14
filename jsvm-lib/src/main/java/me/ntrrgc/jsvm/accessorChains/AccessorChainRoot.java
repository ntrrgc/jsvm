package me.ntrrgc.jsvm.accessorChains;

/**
 * Created by ntrrgc on 3/21/17.
 */

abstract class AccessorChainRoot implements AccessorChain {
    AccessorChainRoot() {}

    @Override
    public int getDepthRemaining() {
        return 8;
    }
}
