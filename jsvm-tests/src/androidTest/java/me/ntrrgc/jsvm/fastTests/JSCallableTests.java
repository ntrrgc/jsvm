/* Copyright (c) the JSVM authors
 * https://github.com/ntrrgc/jsvm
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

package me.ntrrgc.jsvm.fastTests;

import org.jetbrains.annotations.NotNull;
import org.junit.After;
import org.junit.Before;
import org.junit.Test;

import me.ntrrgc.jsvm.JSCallable;
import me.ntrrgc.jsvm.JSError;
import me.ntrrgc.jsvm.JSFunction;
import me.ntrrgc.jsvm.JSObject;
import me.ntrrgc.jsvm.JSVM;
import me.ntrrgc.jsvm.JSValue;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.fail;

public class JSCallableTests {
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
    public void createSimpleFunction() throws Exception {
        JSFunction jsFunction = jsvm.newFunction(new JSCallable() {
            @NotNull
            @Override
            public JSValue call(@NotNull JSValue[] args, @NotNull JSValue thisArg, @NotNull JSVM jsvm) {
                return JSValue.aNumber(args[0].asDouble() + args[1].asDouble());
            }
        });
        JSValue ret = jsFunction.invoke(JSValue.aNumber(3), JSValue.aNumber(2));
        assertEquals(5, ret.asInt());
    }

    @Test
    public void functionThatReceivesObject() throws Exception {
        JSObject obj = jsvm.newObject();
        obj.set("x", JSValue.aNumber(5));
        JSFunction jsFunction = jsvm.newFunction(new JSCallable() {
            @NotNull
            @Override
            public JSValue call(@NotNull JSValue[] args, @NotNull JSValue thisArg, @NotNull JSVM jsvm) {
                return args[0].asObject().get("x");
            }
        });
        JSValue ret = jsFunction.invoke(JSValue.anObject(obj));
        assertEquals(5, ret.asInt());
    }

    @Test
    public void functionThatFails() throws Exception {
        JSObject obj = jsvm.newObject();
        obj.set("x", JSValue.aNumber(5));
        JSFunction jsFunction = jsvm.newFunction(new JSCallable() {
            @NotNull
            @Override
            public JSValue call(@NotNull JSValue[] args, @NotNull JSValue thisArg, @NotNull JSVM jsvm) {
                throw new RuntimeException("Oops");
            }
        });
        try {
            jsFunction.invoke(JSValue.anObject(obj));
            fail();
        } catch (JSError jsError) {
            assertEquals("Java callable threw an exception: Oops", jsError.getMessage());
        }
    }

    @Test
    public void functionThatReceivesObjectFromJSCode() throws Exception {
        JSFunction jsFunction = jsvm.newFunction(new JSCallable() {
            @NotNull
            @Override
            public JSValue call(@NotNull JSValue[] args, @NotNull JSValue thisArg, @NotNull JSVM jsvm) {
                return args[0].asObject().get("x");
            }
        });
        jsvm.getGlobalScope().set("fun", JSValue.anObject(jsFunction));
        jsvm.evaluate("fun({x: 3})");
    }

    @Test
    public void jsCallsJavaCallsJS() throws Exception {
        JSFunction jsFunction = jsvm.newFunction(new JSCallable() {
            @NotNull
            @Override
            public JSValue call(@NotNull JSValue[] args, @NotNull JSValue thisArg, @NotNull JSVM jsvm) {
                assertEquals(2, jsvm.evaluate("({a:2})").asObject().get("a").asInt());
                assertEquals(3, jsvm.evaluate("({a:3})").asObject().get("a").asInt());
                return jsvm.evaluate("({a:4})");
            }
        });
        jsvm.getGlobalScope().set("fun", JSValue.anObject(jsFunction));
        assertEquals(4, jsvm.evaluate("fun()").asObject().get("a").asInt());
    }

    @Test
    public void jsCallsJavaCallsJS_ExceptionInsideJS() throws Exception {
        JSFunction jsFunction = jsvm.newFunction(new JSCallable() {
            @NotNull
            @Override
            public JSValue call(@NotNull JSValue[] args, @NotNull JSValue thisArg, @NotNull JSVM jsvm) {
                assertEquals(2, jsvm.evaluate("({a:2})").asObject().get("a").asInt());
                assertEquals(3, jsvm.evaluate("({a:3})").asObject().get("a").asInt());
                throw new RuntimeException("Oops");
            }
        });
        jsvm.getGlobalScope().set("fun", JSValue.anObject(jsFunction));
        try {
            jsvm.evaluate("fun()");
            fail();
        } catch (JSError jsError) {
            assertEquals("Java callable threw an exception: Oops", jsError.getMessage());
        }
    }

    @Test
    public void jsCallsJavaCallsJSCallsJava() throws Exception {
        jsvm.getGlobalScope().set("simpleJavaFunction", JSValue.anObject(jsvm.newFunction(new JSCallable() {
            @NotNull
            @Override
            public JSValue call(@NotNull JSValue[] args, @NotNull JSValue thisArg, @NotNull JSVM jsvm) {
                if (args.length != 1) {
                    throw new RuntimeException("Invalid args");
                }
                if (args[0].asInt() == 0) {
                    return JSValue.aString("Hello");
                } else {
                    return JSValue.aString("World");
                }
            }
        })));

        jsvm.getGlobalScope().set("jsFunction", jsvm.evaluate("" +
                "(function jsFunction() {" +
                "    return simpleJavaFunction(0) + ' ' + simpleJavaFunction(1);" +
                "})"));

        jsvm.getGlobalScope().set("complexJavaFunction", JSValue.anObject(jsvm.newFunction(new JSCallable() {
            @NotNull
            @Override
            public JSValue call(@NotNull JSValue[] args, @NotNull JSValue thisArg, @NotNull JSVM jsvm) {
                return jsvm.evaluate("jsFunction()");
            }
        })));

        assertEquals("Hello World", jsvm.evaluate("complexJavaFunction()").asString());
    }

    @Test
    public void jsCallsJavaCallsDifferentJSVMThenJS() throws Exception {
        final JSVM otherJSVM = new JSVM();
        otherJSVM.getGlobalScope().set("hello", JSValue.anObject(otherJSVM.newFunction(new JSCallable() {
            @NotNull
            @Override
            public JSValue call(@NotNull JSValue[] args, @NotNull JSValue thisArg, @NotNull JSVM jsvm) {
                return JSValue.aString("Hello");
            }
        })));

        jsvm.getGlobalScope().set("world", JSValue.anObject(jsvm.newFunction(new JSCallable() {
            @NotNull
            @Override
            public JSValue call(@NotNull JSValue[] args, @NotNull JSValue thisArg, @NotNull JSVM jsvm) {
                return JSValue.aString("World");
            }
        })));

        jsvm.getGlobalScope().set("helloWorld", JSValue.anObject(jsvm.newFunction(new JSCallable() {
            @NotNull
            @Override
            public JSValue call(@NotNull JSValue[] args, @NotNull JSValue thisArg, @NotNull JSVM jsvm) {
                String part1 = otherJSVM.evaluate("hello()").asString();
                String part2 = jsvm.evaluate("world()").asString();
                return JSValue.aString(part1 + " " + part2);
            }
        })));

        assertEquals("Hello World", jsvm.evaluate("helloWorld()").asString());
    }

    @Test
    public void jsCallsJavaWhichUsesDifferentJSVMThenCallsJavaAgain() throws Exception {
        jsvm.getGlobalScope().set("hello", JSValue.anObject(jsvm.newFunction(new JSCallable() {
            @NotNull
            @Override
            public JSValue call(@NotNull JSValue[] args, @NotNull JSValue thisArg, @NotNull JSVM jsvm) {
                String hello = new JSVM().evaluate("'Hello'").asString();
                return JSValue.aString(hello);
            }
        })));

        jsvm.getGlobalScope().set("world", JSValue.anObject(jsvm.newFunction(new JSCallable() {
            @NotNull
            @Override
            public JSValue call(@NotNull JSValue[] args, @NotNull JSValue thisArg, @NotNull JSVM jsvm) {
                return JSValue.aString("World");
            }
        })));

        jsvm.evaluate("hello() + world()");
    }
}
