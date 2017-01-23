package me.ntrrgc.jsvm;

import android.support.test.runner.AndroidJUnit4;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;
import org.junit.runner.RunWith;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertFalse;
import static org.junit.Assert.assertNull;
import static org.junit.Assert.assertTrue;

/**
 * Instrumentation test, which will execute on an Android device.
 *
 * @see <a href="http://d.android.com/tools/testing">Testing documentation</a>
 */
@RunWith(AndroidJUnit4.class)
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
        assertEquals("hello world", jsvm.evaluateScript("'hello ' + 'world'").asStringOrNull());
    }

    @Test
    public void testInteger() throws Exception {
        JSValue result = jsvm.evaluateScript("1 + 2");
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
        JSValue result = jsvm.evaluateScript("9 / 10");
        assertTrue(result.isNumber());
        assertEquals(0, result.asIntOrNull().intValue());
        assertEquals(0.9, result.asDoubleOrNull().doubleValue(), 0.0);
        assertEquals("0.9", result.asStringOrNull());
        assertNull(result.asObjectOrNull());
        assertNull(result.asBooleanOrNull());
    }

    @Test
    public void testEmptyTypes() throws Exception {
        assertTrue(jsvm.evaluateScript("null").isNull());
        assertFalse(jsvm.evaluateScript("null").isUndefined());

        assertTrue(jsvm.evaluateScript("undefined").isUndefined());
        assertFalse(jsvm.evaluateScript("undefined").isNull());
    }

    @Test
    public void testBoolean() throws Exception {
        JSValue result = jsvm.evaluateScript("9 < 10");
        assertTrue(result.isBoolean());
        assertTrue(result.asBooleanOrNull());

        assertFalse(jsvm.evaluateScript("9 > 10").asBooleanOrNull());

        assertNull(jsvm.evaluateScript("10 - 9").asBooleanOrNull());
    }

    @Test
    public void testObjectToString() throws Exception {
        JSValue result = jsvm.evaluateScript("({x: 3, toString: function() {" +
            "return 'I am the number ' + this.x;" +
        "} })");
        assertFalse(result.isNumber());
        assertFalse(result.isNull());
        assertFalse(result.isUnsupported());

        assertTrue(result.isObject());
        assertEquals("I am the number 3", result.asObjectOrNull().toString());
    }
}
