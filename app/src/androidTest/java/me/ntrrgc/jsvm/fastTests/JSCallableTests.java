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

}
