package me.ntrrgc.jsvm.fastTests;

import android.support.test.runner.AndroidJUnit4;

import org.jetbrains.annotations.NotNull;
import org.junit.After;
import org.junit.Before;
import org.junit.Test;
import org.junit.runner.RunWith;

import me.ntrrgc.jsvm.JSCallable;
import me.ntrrgc.jsvm.JSError;
import me.ntrrgc.jsvm.JSFunction;
import me.ntrrgc.jsvm.JSObject;
import me.ntrrgc.jsvm.JSVM;
import me.ntrrgc.jsvm.JSValue;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.fail;

/**
 * Instrumentation test, which will execute on an Android device.
 *
 * @see <a href="http://d.android.com/tools/testing">Testing documentation</a>
 */
@RunWith(AndroidJUnit4.class)
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
        JSFunction jsFunction = jsvm.function(new JSCallable() {
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
        JSFunction jsFunction = jsvm.function(new JSCallable() {
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
        JSFunction jsFunction = jsvm.function(new JSCallable() {
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
        JSFunction jsFunction = jsvm.function(new JSCallable() {
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
    public void severalNestedEvaluations() throws Exception {
        JSFunction jsFunction = jsvm.function(new JSCallable() {
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
    public void severalNestedEvaluationsThenException() throws Exception {
        JSFunction jsFunction = jsvm.function(new JSCallable() {
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
}
