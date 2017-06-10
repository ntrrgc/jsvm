package me.ntrrgc.jsvm;

import org.jetbrains.annotations.NotNull;

/**
 * These methods allow doing things that normally would not be possible for users of JSVM due
 * to package-level visibility, but are useful for the JSVM unit tests.
 */
public class TestUtils {
    public static int getAliveCallableCount(@NotNull JSVM jsvm) {
        return jsvm.callableAllocator.countAliveHandles();
    }
}
