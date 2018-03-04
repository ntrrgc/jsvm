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

import me.ntrrgc.jsvm.JSObject;
import me.ntrrgc.jsvm.JSVM;
import me.ntrrgc.jsvm.JSValue;

import static org.junit.Assert.assertEquals;

public class StringTests {
    private JSVM jsvm;
    private JSObject tester;

    @Before
    public void setUp() {
        jsvm = new JSVM();
        tester = jsvm.evaluate("var tester = {" +
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
        assertEquals("☃", jsvm.evaluate("'☃'").asString());
        assertEquals(1, jsvm.evaluate("'☃'").asString().length());
    }

    @Test
    public void testSnowmanToJS() throws Exception {
        tester.set("string", JSValue.aString("☃"));
        assertEquals("☃", tester.get("string").asString());
        assertEquals(1, tester.get("length").asInt());
    }

    @Test
    public void testAcutesFromJS() throws Exception {
        assertEquals("Lucía", jsvm.evaluate("'Lucía'").asString());
    }

    @Test
    public void testCatGrinningFromJS() throws Exception {
        assertEquals("\uD83D\uDE3A", jsvm.evaluate("'\\uD83D\\uDE3A'").asString());
        assertEquals("\uD83D\uDE3A".length(), jsvm.evaluate("'\\uD83D\\uDE3A'").asString().length());
    }

    @Test
    public void testCatGrinningInJS() throws Exception {
        assertEquals(2, jsvm.evaluate("('\\uD83D\\uDE3A').length").asInt());
    }

    @Test
    public void testCatGrinningAsObjectKeyFromJS() throws Exception {
        JSObject obj = jsvm.evaluate("({'\\uD83D\\uDE3A': 'value'})").asObject();
        assertEquals("value", obj.get("\uD83D\uDE3A").asString());
    }

    @Test
    public void testCatGrinningAsObjectKeyFromJava() throws Exception {
        tester.set("\uD83D\uDE3A", JSValue.aString("value"));
        assertEquals("value", tester.get("\uD83D\uDE3A").asString());
        assertEquals("value", jsvm.evaluate("tester['\\uD83D\\uDE3A']").asString());
    }

    @Test
    public void testEmbeddedNullsInObjectKeyFromJS() throws Exception {
        JSObject obj = jsvm.evaluate("({'a\\u0000b': 'value'})").asObject();
        assertEquals("value", obj.get("a\u0000b").asString());
    }

    @Test
    public void testEmbeddedNullsInObjectKeyFromJava() throws Exception {
        tester.set("a\u0000b", JSValue.aString("value"));
        assertEquals("value", tester.get("a\u0000b").asString());
        assertEquals("value", jsvm.evaluate("tester['a\\u0000b']").asString());
    }

    @Test
    public void testCatGrinningToJS() throws Exception {
        tester.set("string", JSValue.aString("\uD83D\uDE3A"));
        assertEquals("\uD83D\uDE3A", tester.get("string").asString());
        assertEquals(2, tester.get("length").asInt());
    }

    @Test
    public void testEmbeddedNullsFromJS() throws Exception {
        assertEquals("a\u0000b", jsvm.evaluate("'a\\u0000b'").asString());
    }

    @Test
    public void testEmbeddedNullsToJS() throws Exception {
        tester.set("string", JSValue.aString("a\u0000b"));
        assertEquals("a\u0000b", tester.get("string").asString());
        assertEquals(3, tester.get("length").asInt());
    }
}
