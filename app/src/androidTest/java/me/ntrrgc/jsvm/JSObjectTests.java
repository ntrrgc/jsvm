package me.ntrrgc.jsvm;

import android.support.test.runner.AndroidJUnit4;

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
        assertEquals(0, jsvm.getStackSize());
    }

    @After
    public void tearDown() {
        assertEquals(0, jsvm.getStackSize());
    }

    @Test
    public void testReadProperties() throws Exception {
        JSObject obj = jsvm.evaluateScript("({x: 5, 1: 'one'})").asObjectOrNull();
        assertNotNull(obj);

        assertTrue(obj.contains("x"));
        assertTrue(obj.contains(1));
        assertFalse(obj.contains("y"));
        assertFalse(obj.contains(0));

        assertEquals(5, obj.get("x").asIntOrNull().intValue());
        assertEquals("one", obj.get(1).asStringOrNull());
        assertTrue(obj.get("y").isUndefined());
        assertTrue(obj.get(0).isUndefined());
    }

    @Test
    public void testWriteProperties() throws Exception {
        JSObject obj = jsvm.evaluateScript("({})").asObjectOrNull();
        assertNotNull(obj);

        obj.set("x", JSValue.aNumber(5));
        obj.set(1, JSValue.aString("one"));

        assertTrue(obj.contains("x"));
        assertTrue(obj.contains(1));
        assertFalse(obj.contains("y"));
        assertFalse(obj.contains(0));

        assertEquals(5, obj.get("x").asIntOrNull().intValue());
        assertEquals("one", obj.get(1).asStringOrNull());
        assertTrue(obj.get("y").isUndefined());
        assertTrue(obj.get(0).isUndefined());
    }

    @Test
    public void testWriteNestedObjects() throws Exception {
        JSObject nodeA = jsvm.evaluateScript("({value: 0, next: null})").asObjectOrNull();
        assertNotNull(nodeA);
        JSObject nodeB = jsvm.evaluateScript("({value: 1, next: null})").asObjectOrNull();
        assertNotNull(nodeB);

        nodeA.set("next", JSValue.anObject(nodeB));
        assertEquals(1, nodeA.get("next").asObjectOrNull().get("value").asIntOrNull().intValue());

        nodeB.set("value", JSValue.aNumber(2));
        assertEquals(2, nodeA.get("next").asObjectOrNull().get("value").asIntOrNull().intValue());
    }

    @Test
    public void crossingVMsThrows() throws Exception {
        JSObject obj = jsvm.evaluateScript("({})").asObjectOrNull();
        assertNotNull(obj);

        JSVM anotherVM = new JSVM();
        JSObject foreignThingy = anotherVM.evaluateScript("({})").asObjectOrNull();
        assertNotNull(foreignThingy);

        boolean caught = false;
        try {
            obj.set(0, JSValue.anObject(foreignThingy));
        } catch (AttemptedToUseObjectFromOtherVM err) {
            assertEquals(err.usedObject, foreignThingy);
            assertEquals(err.usedObjectVM, anotherVM);
            caught = true;
        }
        assertTrue(caught);
    }

    @Test
    public void functionsAreObjects() throws Exception {
        JSObject fun = jsvm.evaluateScript("(function hello() { return 'Hi'; })").asObjectOrNull();
        assertNotNull(fun);

        assertEquals("hello", fun.get("name").asStringOrNull());
    }
}
