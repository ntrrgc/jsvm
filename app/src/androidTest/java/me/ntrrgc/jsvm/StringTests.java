package me.ntrrgc.jsvm;

import android.content.Context;
import android.support.test.InstrumentationRegistry;
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
public class StringTests {
    private JSVM jsvm;
    private JSObject tester;

    @Before
    public void setUp() {
        jsvm = new JSVM();
        tester = jsvm.evaluateScript("({string: '', get length() { return this.string.length; }})").asObject();
    }

    @After
    public void tearDown() {
    }

    @Test
    public void testTester() throws Exception {
        tester.set("string", JSValue.aString("hello"));
        assertEquals(5, tester.get("length").asInt().intValue());
    }

    @Test
    public void testSnowmanFromJS() throws Exception {
        assertEquals("☃", jsvm.evaluateScript("'☃'").asString());
    }

    @Test
    public void testSnowmanToJS() throws Exception {
        tester.set("string", JSValue.aString("☃"));
        assertEquals("☃", tester.get("string").asString());
        assertEquals(1, tester.get("length").asInt().intValue());
    }

    @Test
    public void testAcutesFromJS() throws Exception {
        assertEquals("Lucía", jsvm.evaluateScript("'Lucía'").asString());
    }

    @Test
    public void testCatGrinningFromJS() throws Exception {
        assertEquals("\uD83D\uDE3A", jsvm.evaluateScript("'\\uD83D\\uDE3A'").asString());
    }

    @Test
    public void testCatGrinningInJS() throws Exception {
        assertEquals(2, jsvm.evaluateScript("('\\uD83D\\uDE3A').length").asInt().intValue());
    }

    @Test
    public void testCatGrinningToJS() throws Exception {
        tester.set("string", JSValue.aString("\uD83D\uDE3A"));
        assertEquals("\uD83D\uDE3A", tester.get("string").asString());
        assertEquals(2, tester.get("length").asInt().intValue());
    }

    @Test
    public void testEmbeddedNullsFromJS() throws Exception {
        assertEquals("a\u0000b", jsvm.evaluateScript("'a\\u0000b'").asString());
    }

    @Test
    public void testEmbeddedNullsToJS() throws Exception {
        tester.set("string", JSValue.aString("a\u0000b"));
        assertEquals("a\u0000b", tester.get("string").asString());
        assertEquals(3, tester.get("length").asInt().intValue());
    }
}
