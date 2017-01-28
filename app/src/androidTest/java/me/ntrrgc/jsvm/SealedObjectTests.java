package me.ntrrgc.jsvm;

import android.support.test.runner.AndroidJUnit4;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;
import org.junit.runner.RunWith;

import static junit.framework.Assert.assertSame;
import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertFalse;
import static org.junit.Assert.assertNotEquals;
import static org.junit.Assert.assertNotNull;
import static org.junit.Assert.assertNull;
import static org.junit.Assert.assertTrue;

/**
 * Instrumentation test, which will execute on an Android device.
 *
 * @see <a href="http://d.android.com/tools/testing">Testing documentation</a>
 */
@RunWith(AndroidJUnit4.class)
public class SealedObjectTests {
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
    public void testSealed() throws Exception {
        JSObject result = jsvm.evaluateScript("Object.seal({x: 2})").asObject();

        assertEquals(2, result.get("x").asInt());

        boolean threw = false;
        try {
            result.set("y", JSValue.aNumber(3));
        } catch (JSError err) {
            threw = true;
            assertEquals("not extensible", err.getMessage());
        }
        assertTrue(threw);

        assertTrue(result.get("y").isUndefined());
    }

    @Test
    public void testFrozen() throws Exception {
        JSObject result = jsvm.evaluateScript("Object.freeze({x: 2})").asObject();

        assertEquals(2, result.get("x").asInt());

        boolean threw = false;
        try {
            result.set("x", JSValue.aNumber(3));
        } catch (JSError err) {
            threw = true;
            assertEquals("not writable", err.getMessage());
        }
        assertTrue(threw);

        assertEquals(2, result.get("x").asInt());
    }
}
