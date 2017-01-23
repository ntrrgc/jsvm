package me.ntrrgc.jsvm;

/**
 * Thrown when the user expected a certain type for a JSValue
 * but a different, incompatible type was found instead.
 *
 * Created by ntrrgc on 1/23/17.
 */
public class InvalidJSValueType extends AssertionError {
    public final JSValue value;

    public InvalidJSValueType(String expectedType, String actualType, JSValue value) {
        super(expectedType + " was expected but " + actualType + " was found.");
        this.value = value;
    }
}
