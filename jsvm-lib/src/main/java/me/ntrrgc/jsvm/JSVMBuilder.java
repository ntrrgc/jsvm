package me.ntrrgc.jsvm;

public class JSVMBuilder {
    private boolean accessorChainsEnabled = true;

    public final JSVMBuilder setAccessorChainsEnabled(boolean accessorChainsEnabled) {
        this.accessorChainsEnabled = accessorChainsEnabled;
        return this;
    }

    public final JSVM createJSVM() {
        return new JSVM(accessorChainsEnabled);
    }
}