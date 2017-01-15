package me.ntrrgc.jsvm;

import org.jetbrains.annotations.Contract;
import org.jetbrains.annotations.Nullable;

/**
 * Created by ntrrgc on 1/14/17.
 */

final public class JSValue {
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
    public boolean isUnsupported() {
        return type == TYPE_UNSUPPORTED;
    }

    @Contract(pure = true)
    public boolean isUndefined() {
        return type == TYPE_UNDEFINED;
    }

    @Contract(pure = true)
    public boolean isNull() {
        return type == TYPE_NULL;
    }

    @Contract(pure = true)
    public boolean isBoolean() {
        return type == TYPE_BOOLEAN;
    }

    @Contract(pure = true)
    public boolean isNumber() {
        return type == TYPE_NUMBER;
    }

    @Contract(pure = true)
    public boolean isString() {
        return type == TYPE_STRING;
    }

    @Contract(pure = true)
    public boolean isObject() {
        return type == TYPE_OBJECT;
    }

    @Nullable
    @Contract(pure = true)
    public Boolean asBoolean() {
        if (isBoolean()) {
            return (Boolean) value;
        } else {
            return null;
        }
    }

    @Nullable
    @Contract(pure = true)
    public Double asDouble() {
        if (isNumber()) {
            return (Double) value;
        } else {
            return null;
        }
    }

    @Nullable
    public Integer asInt() {
        if (isNumber()) {
            return ((Double) value).intValue();
        } else {
            return null;
        }
    }

    @Nullable
    public Long asLong() {
        if (isNumber()) {
            return ((Double) value).longValue();
        } else {
            return null;
        }
    }

    @Nullable
    public String asString() {
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
    public JSObject asObject() {
        if (isObject()) {
            return (JSObject) value;
        } else {
            return null;
        }
    }

    @Contract(" -> !null")
    public static JSValue aNull() {
        return new JSValue(TYPE_NULL, null);
    }

    @Contract(" -> !null")
    public static JSValue anUndefined() {
        return new JSValue(TYPE_UNDEFINED, null);
    }

    @Contract("_ -> !null")
    public static JSValue aBoolean(boolean value) {
        return new JSValue(TYPE_BOOLEAN, value);
    }

    @Contract("_ -> !null")
    public static JSValue aNumber(double value) {
        return new JSValue(TYPE_NUMBER, value);
    }

    @Contract("_ -> !null")
    public static JSValue aString(String value) {
        return new JSValue(TYPE_STRING, value);
    }

    @Contract("_ -> !null")
    public static JSValue anObject(JSObject value) {
        return new JSValue(TYPE_OBJECT, value);
    }

    @Override
    public boolean equals(Object obj) {
        if (obj instanceof JSValue) {
            JSValue other = (JSValue) obj;
            return other.type == type && other.value == value;
        } else {
            return false;
        }
    }

    @Override
    public int hashCode() {
        return this.type ^ this.value.hashCode();
    }
}
