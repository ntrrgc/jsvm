package me.ntrrgc.jsvm;

import org.jetbrains.annotations.NotNull;

/**
 * Thrown when the user expected a certain type for a JSValue
 * but a different, incompatible type was found instead.
 *
 * Created by ntrrgc on 1/23/17.
 */
public final class JSTypeError extends AssertionError {
    public final JSValue value;

    @NotNull
    private static String getMessagePrefix(JSValue value) {
        if (value.accessorChain != null) {
            return "In " + value.accessorChain.getFullPath() + ", ";
        } else {
            return "";
        }
    }

    JSTypeError(@NotNull String expectedType, @NotNull String actualType, @NotNull JSValue value) {
        super(getMessagePrefix(value) + expectedType + " was expected but " + actualType + " was found.");
        this.value = value;
    }
}
