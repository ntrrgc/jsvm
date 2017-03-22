package me.ntrrgc.jsvm.fastTests;

import android.support.test.runner.AndroidJUnit4;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;
import org.junit.runner.RunWith;

import me.ntrrgc.jsvm.JSTypeError;
import me.ntrrgc.jsvm.JSObject;
import me.ntrrgc.jsvm.JSVM;
import me.ntrrgc.jsvm.JSValue;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertFalse;
import static org.junit.Assert.assertNotNull;
import static org.junit.Assert.assertSame;
import static org.junit.Assert.assertTrue;
import static org.junit.Assert.fail;

/**
 * Instrumentation test, which will execute on an Android device.
 *
 * @see <a href="http://d.android.com/tools/testing">Testing documentation</a>
 */
@RunWith(AndroidJUnit4.class)
public class JSTypeErrorTests {
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
    public void testNonExistentMethodBaseObject() throws Exception {
        JSObject obj = jsvm.newObject();

        try {
            obj.invokeMethod("add", JSValue.aNumber(2));
            fail();
        } catch (JSTypeError error) {
            assertEquals("In Object:add, function was expected but undefined was found.", error.getMessage());
        }
    }

    @Test
    public void testNonExistentMethodBareObject() throws Exception {
        JSObject obj = jsvm.newObjectWithProto(null);

        try {
            obj.invokeMethod("add", JSValue.aNumber(2));
            fail();
        } catch (JSTypeError error) {
            assertEquals("In [unknown object]:add, function was expected but undefined was found.", error.getMessage());
        }
    }

    @Test
    public void testNonExistentMethodInObjectWithMaliciousConstructorProperty() throws Exception {
        JSObject obj = jsvm.evaluate("({get constructor() { throw new Error('boom!'); }})").asObject();

        try {
            obj.invokeMethod("add", JSValue.aNumber(2));
            fail();
        } catch (JSTypeError error) {
            assertEquals("In [eval expression].add, function was expected but undefined was found.", error.getMessage());
        }
    }

    @Test
    public void testPropertyChain() throws Exception {
        jsvm.evaluate("function getNestedStructure() {\n" +
                "return {a: {b: [{c: {}}]}};\n" +
                "}");

        try {
            jsvm.getGlobalScope()
                    .invokeMethod("getNestedStructure").asObject()
                    .get("a").asObject()
                    .get("b").asObject()
                    .get(0).asObject()
                    .get("c").asObject()
                    .get("d").asObject();
            fail();
        } catch (JSTypeError error) {
            assertEquals("In getNestedStructure().a.b[0].c.d, object was expected but undefined was found.",
                    error.getMessage());
        }
    }

    @Test
    public void testPropertyChainWithEval() throws Exception {
        jsvm.evaluate("function getNestedStructure() {\n" +
                "return {a: {b: [{c: {}}]}};\n" +
                "}");

        try {
            jsvm.evaluate("getNestedStructure().a").asObject()
                    .get("b").asObject()
                    .get(0).asObject()
                    .get("c").asObject()
                    .get("d").asObject();
            fail();
        } catch (JSTypeError error) {
            assertEquals("In getNestedStructure().a.b[0].c.d, object was expected but undefined was found.",
                    error.getMessage());
        }
    }

    @Test
    public void testPropertyChainDepthLimit() throws Exception {
        jsvm.evaluate("var strangeArrayThatPointsToItself = [];\n" +
                "for (var i = 0; i < 10; i++) {\n" +
                "   strangeArrayThatPointsToItself[i] = strangeArrayThatPointsToItself;\n" +
                "}\n" +
                "strangeArrayThatPointsToItself");

        try {
            JSObject array = jsvm.getGlobalScope().get("strangeArrayThatPointsToItself").asObject();
            for (int i = 2; i < 100; i++) {
                array = array.get(i).asObject();
            }
            fail();
        } catch (JSTypeError error) {
            // It should fail when attempting to read the element with index 10.
            // The accessor chain only extends up to 8 levels
            assertEquals("In strangeArrayThatPointsToItself[2][3][4][5][6][7]...[10], object was expected but undefined was found.",
                    error.getMessage());
        }
    }

    @Test
    public void testNewOperator() throws Exception {
        try {
            jsvm.getGlobalScope().get("Object").asFunction()
                    .callNew().asObject()
                    .get("nonexistent").asInt();
            fail();
        } catch (JSTypeError error) {
            assertEquals("In new Object().nonexistent, number was expected but undefined was found.",
                    error.getMessage());
        }
    }

    @Test
    public void testMoreComplexNewOperator() throws Exception {
        jsvm.evaluate("var obj = { ctor: function () {} }");

        try {
            jsvm.getGlobalScope().get("obj").asObject()
                    .get("ctor").asFunction()
                    .callNew().asObject()
                    .get("nonexistent").asInt();
            fail();
        } catch (JSTypeError error) {
            assertEquals("In new obj.ctor().nonexistent, number was expected but undefined was found.",
                    error.getMessage());
        }
    }

    @Test
    public void testEvenMoreComplexNewOperator() throws Exception {
        jsvm.evaluate("var obj = { ctor: function () {} }\n" +
                "function objFactory() { return {obj: obj}; }");

        try {
            jsvm.getGlobalScope().get("objFactory").asFunction()
                    .invoke().asObject()
                    .get("obj").asObject()
                    .get("ctor").asFunction()
                    .callNew().asObject()
                    .get("nonexistent").asInt();
            fail();
        } catch (JSTypeError error) {
            assertEquals("In new (objFactory().obj.ctor)().nonexistent, number was expected but undefined was found.",
                    error.getMessage());
        }
    }

    @Test
    public void testGlobalIndex() throws Exception {
        try {
            jsvm.getGlobalScope().get(3).asDouble();
            fail();
        } catch (JSTypeError error) {
            assertEquals("In global[3], number was expected but undefined was found.",
                    error.getMessage());
        }
    }

    @Test
    public void testClassRootAndIndex() throws Exception {
        JSObject MyClassPrototype = jsvm.evaluate("new (function MyClass() {})").asObject();

        try {
            jsvm.newObjectWithProto(MyClassPrototype)
                    .get(3).asInt();
            fail();
        } catch (JSTypeError error) {
            assertEquals("In MyClass:[3], number was expected but undefined was found.",
                    error.getMessage());
        }
    }
}
