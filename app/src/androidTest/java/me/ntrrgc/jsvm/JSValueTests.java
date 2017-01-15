package me.ntrrgc.jsvm;

import android.content.Context;
import android.support.test.InstrumentationRegistry;
import android.support.test.runner.AndroidJUnit4;
import android.util.Log;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;
import org.junit.runner.RunWith;

import static org.junit.Assert.*;

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
    }

    @After
    public void tearDown() {
    }

    @Test
    public void useAppContext() throws Exception {
        // Context of the app under test.
        Context appContext = InstrumentationRegistry.getTargetContext();

        assertEquals("me.ntrrgc.jsvm", appContext.getPackageName());
        assertEquals("Hello from C++", JSVM.stringFromJNI());
    }

    @Test
    public void evalSimpleScript() throws Exception {
        JSVM jsvm = new JSVM();
        assertEquals("hello world", jsvm.evaluateScript("'hello ' + 'world'").asString());
    }

    @Test
    public void testInteger() throws Exception {
        JSValue result = jsvm.evaluateScript("1 + 2");
        assertTrue(result.isNumber());
        assertEquals(3, result.asInt().intValue());
        assertEquals(3.0, result.asDouble().doubleValue(), 0.0);
        assertEquals(3L, result.asLong().longValue());
        assertEquals("3", result.asString());
        assertNull(result.asObject());
        assertNull(result.asBoolean());
    }

    @Test
    public void testFractionalNumber() throws Exception {
        JSValue result = jsvm.evaluateScript("9 / 10");
        assertTrue(result.isNumber());
        assertEquals(0, result.asInt().intValue());
        assertEquals(0.9, result.asDouble().doubleValue(), 0.0);
        assertEquals("0.9", result.asString());
        assertNull(result.asObject());
        assertNull(result.asBoolean());
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
        assertTrue(result.asBoolean());

        assertFalse(jsvm.evaluateScript("9 > 10").asBoolean());

        assertNull(jsvm.evaluateScript("10 - 9").asBoolean());
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
        assertEquals("I am the number 3", result.asObject().toString());
    }
}
