package me.ntrrgc.jsvm;

/**
 * Thrown when the user expected a certain type for a JSValue
 * but a different, incompatible type was found instead.
 *
 * Created by ntrrgc on 1/23/17.
 */
public class InvalidJSValueType extends AssertionError {
    public final JSValue value;

    private static String getMessagePrefix(JSValue value) {
        if (value.accessorChain != null) {
            return "In " + value.accessorChain.getFullPath() + ", ";
        } else {
            return "";
        }
    }

    InvalidJSValueType(String expectedType, String actualType, JSValue value) {
        super(getMessagePrefix(value) + expectedType + " was expected but " + actualType + " was found.");
        this.value = value;
    }
}
