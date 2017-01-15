package me.ntrrgc.jsvm;

/**
 * Created by ntrrgc on 1/14/17.
 */

final public class JSValue {
    private JSVM jsVM;
    private Object value;
    private int type;

    private static final int TYPE_UNSUPPORTED = 0;
    private static final int TYPE_UNDEFINED = 1;
    private static final int TYPE_NULL = 2;
    private static final int TYPE_BOOLEAN = 3;
    private static final int TYPE_NUMBER = 4;
    private static final int TYPE_STRING = 5;
    private static final int TYPE_OBJECT = 6;

    public native String asString();
}
