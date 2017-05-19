package me.ntrrgc.jsvm.fastTests;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;

import me.ntrrgc.jsvm.JSVM;
import me.ntrrgc.jsvm.JSValue;

import static junit.framework.Assert.assertSame;
import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertFalse;
import static org.junit.Assert.assertNotNull;
import static org.junit.Assert.assertNull;
import static org.junit.Assert.assertTrue;

public class JSValueTests {
    private JSVM jsvm;

    @Before
    public void setUp() {
        jsvm = new JSVM();
        assertEquals(0, jsvm.getStackSize());
    }

    @After
    public void tearDown() {
        assertEquals(0, jsvm.getStackSize());
    }

    @Test
    public void evalSimpleScript() throws Exception {
        JSVM jsvm = new JSVM();
        assertEquals("hello world", jsvm.evaluate("'hello ' + 'world'").asStringOrNull());
    }

    @Test
    public void testInteger() throws Exception {
        JSValue result = jsvm.evaluate("1 + 2");
        assertTrue(result.isNumber());
        assertEquals(3, result.asIntOrNull().intValue());
        assertEquals(3.0, result.asDoubleOrNull().doubleValue(), 0.0);
        assertEquals(3L, result.asLongOrNull().longValue());
        assertEquals("3", result.asStringOrNull());
        assertNull(result.asObjectOrNull());
        assertNull(result.asBooleanOrNull());
    }

    @Test
    public void testFractionalNumber() throws Exception {
        JSValue result = jsvm.evaluate("9 / 10");
        assertTrue(result.isNumber());
        assertEquals(0, result.asIntOrNull().intValue());
        assertEquals(0.9, result.asDoubleOrNull().doubleValue(), 0.0);
        assertEquals("0.9", result.asStringOrNull());
        assertNull(result.asObjectOrNull());
        assertNull(result.asBooleanOrNull());
    }

    @Test
    public void testEmptyTypes() throws Exception {
        assertTrue(jsvm.evaluate("null").isNull());
        assertFalse(jsvm.evaluate("null").isUndefined());

        assertTrue(jsvm.evaluate("undefined").isUndefined());
        assertFalse(jsvm.evaluate("undefined").isNull());
    }

    @Test
    public void testBoolean() throws Exception {
        JSValue result = jsvm.evaluate("9 < 10");
        assertTrue(result.isBoolean());
        assertTrue(result.asBooleanOrNull());

        assertFalse(jsvm.evaluate("9 > 10").asBooleanOrNull());

        assertNull(jsvm.evaluate("10 - 9").asBooleanOrNull());
    }

    @Test
    public void testObjectToString() throws Exception {
        JSValue result = jsvm.evaluate("({x: 3, toString: function() {" +
            "return 'I am the number ' + this.x;" +
        "} })");
        assertFalse(result.isNumber());
        assertFalse(result.isNull());
        assertFalse(result.isUnsupported());

        assertTrue(result.isObject());
        assertEquals("I am the number 3", result.asObjectOrNull().toString());
    }

    @Test
    public void testFunctionsAreObjects() throws Exception {
        JSValue result = jsvm.evaluate("(function() {})");
        assertTrue(result.isFunction());
        assertTrue(result.isObject());
        assertNotNull(result.asObject());
        assertNotNull(result.asFunction());
        assertSame(result.asObject(), result.asFunction());
    }

    @Test
    public void testEquals() throws Exception {
        JSValue five = JSValue.aNumber(5);
        assertEquals(five, jsvm.evaluate("5"));
    }
}
