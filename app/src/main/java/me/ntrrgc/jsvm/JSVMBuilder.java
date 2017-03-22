package me.ntrrgc.jsvm;

public class JSVMBuilder {
    private boolean accessorChainsEnabled = true;

    public JSVMBuilder setAccessorChainsEnabled(boolean accessorChainsEnabled) {
        this.accessorChainsEnabled = accessorChainsEnabled;
        return this;
    }

    public JSVM createJSVM() {
        return new JSVM(accessorChainsEnabled);
    }
}