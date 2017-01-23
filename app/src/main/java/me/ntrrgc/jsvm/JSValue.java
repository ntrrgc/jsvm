package me.ntrrgc.jsvm;

import org.jetbrains.annotations.Contract;
import org.jetbrains.annotations.NotNull;
import org.jetbrains.annotations.Nullable;

/**
 * A variant type class that wraps any value that can be received or sent  to the JS VM.
 *
 * Created by ntrrgc on 1/14/17.
 */
public final class JSValue {
    private int type;
    private Object value;

    private JSValue(int type, Object value) {
        this.type = type;
        this.value = value;
    }

    private static final int TYPE_UNSUPPORTED = 0;
    private static final int TYPE_UNDEFINED = 1;
    private static final int TYPE_NULL = 2;
    private static final int TYPE_BOOLEAN = 3;
    private static final int TYPE_NUMBER = 4;
    private static final int TYPE_STRING = 5;
    private static final int TYPE_OBJECT = 6;

    @Contract(pure = true)
    public final boolean isUnsupported() {
        return type == TYPE_UNSUPPORTED;
    }

    @Contract(pure = true)
    public final boolean isUndefined() {
        return type == TYPE_UNDEFINED;
    }

    @Contract(pure = true)
    public final boolean isNull() {
        return type == TYPE_NULL;
    }

    @Contract(pure = true)
    public final boolean isBoolean() {
        return type == TYPE_BOOLEAN;
    }

    @Contract(pure = true)
    public final boolean isNumber() {
        return type == TYPE_NUMBER;
    }

    @Contract(pure = true)
    public final boolean isString() {
        return type == TYPE_STRING;
    }

    @Contract(pure = true)
    public final boolean isObject() {
        return type == TYPE_OBJECT;
    }

    @NotNull
    @Contract(pure = true)
    private static String getTypeAsString(int type) {
        switch (type) {
            case TYPE_UNDEFINED:
                return "undefined";
            case TYPE_NULL:
                return "null";
            case TYPE_BOOLEAN:
                return "boolean";
            case TYPE_NUMBER:
                return "number";
            case TYPE_STRING:
                return "string";
            case TYPE_OBJECT:
                return "object";
            case TYPE_UNSUPPORTED:
                return "<unsupported type>";
            default:
                throw new RuntimeException("Unknown type");
        }
    }

    @Nullable
    @Contract(pure = true)
    public final Boolean asBooleanOrNull() {
        if (isBoolean()) {
            return (Boolean) value;
        } else {
            return null;
        }
    }

    @Nullable
    @Contract(pure = true)
    public final Double asDoubleOrNull() {
        if (isNumber()) {
            return (Double) value;
        } else {
            return null;
        }
    }

    @Nullable
    @Contract(pure = true)
    public final Integer asIntOrNull() {
        if (isNumber()) {
            return ((Double) value).intValue();
        } else {
            return null;
        }
    }

    @Nullable
    @Contract(pure = true)
    public final Long asLongOrNull() {
        if (isNumber()) {
            return ((Double) value).longValue();
        } else {
            return null;
        }
    }

    @Nullable
    @Contract(pure = true)
    public final String asStringOrNull() {
        if (isString()) {
            return (String) value;
        } else if (isNumber()) {
            Double doubleValue = (Double) value;
            if (doubleValue == Math.rint(doubleValue)) {
                // no fractional part
                return Integer.toString(doubleValue.intValue());
            } else {
                // fractional part
                return Double.toString((Double) value);
            }
        } else if (isBoolean()) {
            return Boolean.toString((Boolean) value);
        } else if (isObject()) {
            return value.toString();
        } else {
            return null;
        }
    }

    @Nullable
    @Contract(pure = true)
    public final JSObject asObjectOrNull() {
        if (isObject()) {
            return (JSObject) value;
        } else {
            return null;
        }
    }

    @Contract(pure = true)
    public final boolean asBoolean() {
        Boolean value = this.asBooleanOrNull();
        if (value != null) {
            return value;
        } else {
            throw new InvalidJSValueType(getTypeAsString(TYPE_BOOLEAN), getTypeAsString(type), this);
        }
    }

    @Contract(pure = true)
    public final double asDouble() {
        Double value = this.asDoubleOrNull();
        if (value != null) {
            return value;
        } else {
            throw new InvalidJSValueType(getTypeAsString(TYPE_NUMBER), getTypeAsString(type), this);
        }
    }

    @Contract(pure = true)
    public final int asInt() {
        Integer value = this.asIntOrNull();
        if (value != null) {
            return value;
        } else {
            throw new InvalidJSValueType(getTypeAsString(TYPE_NUMBER), getTypeAsString(type), this);
        }
    }

    @Contract(pure = true)
    public final long asLong() {
        Long value = this.asLongOrNull();
        if (value != null) {
            return value;
        } else {
            throw new InvalidJSValueType(getTypeAsString(TYPE_NUMBER), getTypeAsString(type), this);
        }
    }

    @NotNull
    @Contract(pure = true)
    public final String asString() {
        String value = this.asStringOrNull();
        if (value != null) {
            return value;
        } else {
            throw new InvalidJSValueType(getTypeAsString(TYPE_STRING), getTypeAsString(type), this);
        }
    }

    @NotNull
    @Contract(pure = true)
    public final JSObject asObject() {
        if (isObject()) {
            return (JSObject) value;
        } else {
            throw new InvalidJSValueType(getTypeAsString(TYPE_OBJECT), getTypeAsString(type), this);
        }
    }

    @NotNull
    public final static JSValue aNull() {
        return new JSValue(TYPE_NULL, null);
    }

    @NotNull
    public final static JSValue anUndefined() {
        return new JSValue(TYPE_UNDEFINED, null);
    }

    @NotNull
    public final static JSValue aBoolean(boolean value) {
        return new JSValue(TYPE_BOOLEAN, value);
    }

    @NotNull
    public final static JSValue aNumber(double value) {
        return new JSValue(TYPE_NUMBER, value);
    }

    @NotNull
    public final static JSValue aString(@NotNull String value) {
        return new JSValue(TYPE_STRING, value);
    }

    @NotNull
    public final static JSValue anObject(@NotNull JSObject value) {
        return new JSValue(TYPE_OBJECT, value);
    }

    @Override
    public final boolean equals(Object obj) {
        if (obj instanceof JSValue) {
            JSValue other = (JSValue) obj;
            return other.type == type && other.value == value;
        } else {
            return false;
        }
    }

    @Override
    public final int hashCode() {
        return this.type ^ this.value.hashCode();
    }

    @Override
    @NotNull
    public final String toString() {
        switch (type) {
            case TYPE_UNDEFINED:
                return "undefined";
            case TYPE_NULL:
                return "null";
            case TYPE_BOOLEAN:
            case TYPE_NUMBER:
            case TYPE_STRING:
            case TYPE_OBJECT:
                return asStringOrNull();
            case TYPE_UNSUPPORTED:
                return "<unsupported type>";
            default:
                throw new RuntimeException("Unknown type");
        }
    }
}
