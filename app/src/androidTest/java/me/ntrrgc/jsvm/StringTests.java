package me.ntrrgc.jsvm;

import android.support.test.runner.AndroidJUnit4;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;
import org.junit.runner.RunWith;

import static org.junit.Assert.assertEquals;

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
        tester = jsvm.evaluateScript("var tester = {" +
                "string: '', " +
                "get length() { return this.string.length; }" +
                "}; tester").asObject();
        assertEquals(0, jsvm.getStackSize());
    }

    @After
    public void tearDown() {
        assertEquals(0, jsvm.getStackSize());
    }

    @Test
    public void testTester() throws Exception {
        tester.set("string", JSValue.aString("hello"));
        assertEquals(5, tester.get("length").asInt());
    }

    @Test
    public void testSnowmanFromJS() throws Exception {
        assertEquals("☃", jsvm.evaluateScript("'☃'").asString());
    }

    @Test
    public void testSnowmanToJS() throws Exception {
        tester.set("string", JSValue.aString("☃"));
        assertEquals("☃", tester.get("string").asString());
        assertEquals(1, tester.get("length").asInt());
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
        assertEquals(2, jsvm.evaluateScript("('\\uD83D\\uDE3A').length").asInt());
    }

    @Test
    public void testCatGrinningAsObjectKeyFromJS() throws Exception {
        JSObject obj = jsvm.evaluateScript("({'\\uD83D\\uDE3A': 'value'})").asObject();
        assertEquals("value", obj.get("\uD83D\uDE3A").asString());
    }

    @Test
    public void testCatGrinningAsObjectKeyFromJava() throws Exception {
        tester.set("\uD83D\uDE3A", JSValue.aString("value"));
        assertEquals("value", tester.get("\uD83D\uDE3A").asString());
        assertEquals("value", jsvm.evaluateScript("tester['\\uD83D\\uDE3A']").asString());
    }

    @Test
    public void testEmbeddedNullsInObjectKeyFromJS() throws Exception {
        JSObject obj = jsvm.evaluateScript("({'a\\u0000b': 'value'})").asObject();
        assertEquals("value", obj.get("a\u0000b").asString());
    }

    @Test
    public void testEmbeddedNullsInObjectKeyFromJava() throws Exception {
        tester.set("a\u0000b", JSValue.aString("value"));
        assertEquals("value", tester.get("a\u0000b").asString());
        assertEquals("value", jsvm.evaluateScript("tester['a\\u0000b']").asString());
    }

    @Test
    public void testCatGrinningToJS() throws Exception {
        tester.set("string", JSValue.aString("\uD83D\uDE3A"));
        assertEquals("\uD83D\uDE3A", tester.get("string").asString());
        assertEquals(2, tester.get("length").asInt());
    }

    @Test
    public void testEmbeddedNullsFromJS() throws Exception {
        assertEquals("a\u0000b", jsvm.evaluateScript("'a\\u0000b'").asString());
    }

    @Test
    public void testEmbeddedNullsToJS() throws Exception {
        tester.set("string", JSValue.aString("a\u0000b"));
        assertEquals("a\u0000b", tester.get("string").asString());
        assertEquals(3, tester.get("length").asInt());
    }
}
