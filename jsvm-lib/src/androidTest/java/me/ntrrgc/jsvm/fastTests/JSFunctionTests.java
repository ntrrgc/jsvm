package me.ntrrgc.jsvm.fastTests;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;

import me.ntrrgc.jsvm.JSFunction;
import me.ntrrgc.jsvm.JSObject;
import me.ntrrgc.jsvm.JSVM;
import me.ntrrgc.jsvm.JSValue;

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

}
