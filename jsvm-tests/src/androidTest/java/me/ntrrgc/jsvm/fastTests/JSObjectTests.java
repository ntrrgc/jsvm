/* Copyright (c) the JSVM authors
 * https://github.com/ntrrgc/jsvm
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

package me.ntrrgc.jsvm.fastTests;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;

import me.ntrrgc.jsvm.AttemptedToUseObjectFromOtherVM;
import me.ntrrgc.jsvm.JSObject;
import me.ntrrgc.jsvm.JSVM;
import me.ntrrgc.jsvm.JSValue;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertFalse;
import static org.junit.Assert.assertNotNull;
import static org.junit.Assert.assertSame;
import static org.junit.Assert.assertTrue;

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
        JSObject obj = jsvm.evaluate("({x: 5, 1: 'one'})").asObjectOrNull();
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
        JSObject obj = jsvm.evaluate("({})").asObjectOrNull();
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
        JSObject nodeA = jsvm.evaluate("({value: 0, next: null})").asObjectOrNull();
        assertNotNull(nodeA);
        JSObject nodeB = jsvm.evaluate("({value: 1, next: null})").asObjectOrNull();
        assertNotNull(nodeB);

        nodeA.set("next", JSValue.anObject(nodeB));
        assertEquals(1, nodeA.get("next").asObjectOrNull().get("value").asIntOrNull().intValue());

        nodeB.set("value", JSValue.aNumber(2));
        assertEquals(2, nodeA.get("next").asObjectOrNull().get("value").asIntOrNull().intValue());
    }

    @Test
    public void crossingVMsThrows() throws Exception {
        JSObject obj = jsvm.evaluate("({})").asObjectOrNull();
        assertNotNull(obj);

        JSVM anotherVM = new JSVM();
        JSObject foreignThingy = anotherVM.evaluate("({})").asObjectOrNull();
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
        JSObject fun = jsvm.evaluate("(function hello() { return 'Hi'; })").asObjectOrNull();
        assertNotNull(fun);

        assertEquals("hello", fun.get("name").asStringOrNull());
    }

    @Test
    public void newObject() throws Exception {
        JSObject obj = jsvm.newObject();

        obj.set("hi", JSValue.aString("hello"));
        assertEquals("hello", obj.get("hi").asString());

        assertTrue(obj.contains("hasOwnProperty"));
        assertTrue(obj.contains("toString"));
        assertEquals(obj.get("__proto__").asObject(), jsvm.evaluate("({}.__proto__)").asObject());
        assertSame(obj.get("__proto__").asObject(), jsvm.evaluate("({}.__proto__)").asObject());
    }

    @Test
    public void newBareObject() throws Exception {
        JSObject obj = jsvm.newObjectWithProto(null);

        obj.set("hi", JSValue.aString("hello"));
        assertEquals("hello", obj.get("hi").asString());

        assertFalse(obj.contains("hasOwnProperty"));
        assertFalse(obj.contains("toString"));
    }

    @Test
    public void newInheritingObject() throws Exception {
        JSObject parent = jsvm.newObject();
        JSObject child = jsvm.newObjectWithProto(parent);

        parent.set("hi", JSValue.aString("hello"));
        assertEquals("hello", child.get("hi").asString());
        assertSame(parent, child.get("__proto__").asObject());
    }

    @Test
    public void testMethods() throws Exception {
        JSObject obj = jsvm.evaluate("" +
                "function Adder() {\n" +
                "   this.total = 0;\n" +
                "}\n" +
                "Adder.prototype.add = function (quantity) {\n" +
                "   this.total += quantity;\n" +
                "};\n" +
                "new Adder()").asObject();

        obj.invokeMethod("add", JSValue.aNumber(2));
        obj.invokeMethod("add", JSValue.aNumber(3));
        assertEquals(5, obj.get("total").asInt());
    }

}
