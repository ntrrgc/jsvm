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
public class JSObjectTests {
    private JSVM jsvm;

    @Before
    public void setUp() {
        jsvm = new JSVM();
    }

    @After
    public void tearDown() {
    }

    @Test
    public void testReadProperties() throws Exception {
        JSObject obj = jsvm.evaluateScript("({x: 5, 1: 'one'})").asObject();
        assertNotNull(obj);

        assertTrue(obj.contains("x"));
        assertTrue(obj.contains(1));
        assertFalse(obj.contains("y"));
        assertFalse(obj.contains(0));

        assertEquals(5, obj.get("x").asInt().intValue());
        assertEquals("one", obj.get(1).asString());
        assertTrue(obj.get("y").isUndefined());
        assertTrue(obj.get(0).isUndefined());
    }

    @Test
    public void testWriteProperties() throws Exception {
        JSObject obj = jsvm.evaluateScript("({})").asObject();
        assertNotNull(obj);

        obj.set("x", JSValue.aNumber(5));
        obj.set(1, JSValue.aString("one"));

        assertTrue(obj.contains("x"));
        assertTrue(obj.contains(1));
        assertFalse(obj.contains("y"));
        assertFalse(obj.contains(0));

        assertEquals(5, obj.get("x").asInt().intValue());
        assertEquals("one", obj.get(1).asString());
        assertTrue(obj.get("y").isUndefined());
        assertTrue(obj.get(0).isUndefined());
    }
}
