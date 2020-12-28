/* Copyright (c) the JSVM authors
 * https://github.com/ntrrgc/jsvm
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

package me.ntrrgc.jsvm.fastTests;

import org.jetbrains.annotations.NotNull;
import org.junit.After;
import org.junit.Assert;
import org.junit.Before;
import org.junit.Test;

import me.ntrrgc.jsvm.JSCallable;
import me.ntrrgc.jsvm.JSFunction;
import me.ntrrgc.jsvm.JSObject;
import me.ntrrgc.jsvm.JSVM;
import me.ntrrgc.jsvm.JSValue;
import me.ntrrgc.jsvm.TestUtils;

import static org.junit.Assert.assertEquals;

public class JSFunctionTests {
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
    public void evalSimpleFunction() throws Exception {
        JSFunction jsFunction = jsvm.evaluate("(function hello() { return 'Hello world'; })").asFunction();
        JSValue ret = jsFunction.call(JSValue.aNull());
        assertEquals("Hello world", ret.asString());
    }

    @Test
    public void evalMathFunction() throws Exception {
        JSFunction jsFunction = jsvm.evaluate("(function sum(a, b) { return a + b; })").asFunction();
        JSValue ret = jsFunction.call(JSValue.aNull(), JSValue.aNumber(10), JSValue.aNumber(20));
        assertEquals(30, ret.asInt());
    }

    @Test
    public void functionToString() throws Exception {
        JSFunction jsFunction = jsvm.evaluate("(function hello() { return 'Hello world'; })").asFunction();
        assertEquals("function hello() { [ecmascript code] }", jsFunction.toString());
    }

    @Test
    public void functionAsObject() throws Exception {
        JSFunction jsFunction = jsvm.evaluate("(function hello() { return 'Hello world'; })").asFunction();
        jsFunction.set("hi", JSValue.aString("hello"));
        assertEquals("hello", jsFunction.get("hi").asString());
    }

    @Test
    public void testConstructor() throws Exception {
        JSFunction Dog = jsvm.evaluate("(function Dog(name) { this.name = name; })").asFunction();
        JSObject dog = Dog.callNew(JSValue.aString("Seymour Asses")).asObject();
        assertEquals("Seymour Asses", dog.get("name").asString());
    }

    @Test
    public void testShortCircuitedConstructor() throws Exception {
        JSFunction Dog = jsvm.evaluate("var staticDog = {name: 'Beethoven'};" +
                "(function Dog() { return staticDog; })").asFunction();

        JSObject dog = Dog.callNew().asObject();
        // the static dog is returned
        assertEquals("Beethoven", dog.get("name").asString());

        // always returns the same instance
        JSObject dogAgain = Dog.callNew().asObject();
        assertEquals(dog, dogAgain);
    }

    @Test
    public void testNativeFunctionsAreGCed() throws Exception {
        Assert.assertEquals(0, TestUtils.getAliveCallableCount(jsvm));

        JSFunction dummyFunction = jsvm.newFunction(new JSCallable() {
            @NotNull
            @Override
            public JSValue call(@NotNull JSValue[] args, @NotNull JSValue thisArg, @NotNull JSVM jsvm) {
                return JSValue.aNull();
            }
        });

        assertEquals("Function has been freed too soon", 1, TestUtils.getAliveCallableCount(jsvm));

        jsvm.getGlobalScope().set("dummyFunction", JSValue.anObject(dummyFunction));

        // Now there are two references to the JS function that trampolines to our dummy callable:
        // One it's in the global scope, the other is in the hidden stash under the handle of our
        // JSFunction object. The later is important as it's what has allowed it to be alive to
        // this point!

        // Nevertheless, now we want to get rid of both references so that the function can be GCed
        // in the JS side.
        dummyFunction.close();
        jsvm.getGlobalScope().set("dummyFunction", JSValue.anUndefined());

        // Force JS side garbage collection
        jsvm.getGlobalScope().get("Duktape").asObject().invokeMethod("gc");

        // The function should have been freed.
        assertEquals("Function has not been freed", 0, TestUtils.getAliveCallableCount(jsvm));
    }

}
