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

import me.ntrrgc.jsvm.JSError;
import me.ntrrgc.jsvm.JSObject;
import me.ntrrgc.jsvm.JSVM;
import me.ntrrgc.jsvm.JSValue;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertTrue;

public class SealedObjectTests {
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
    public void testSealed() throws Exception {
        JSObject result = jsvm.evaluate("Object.seal({x: 2})").asObject();

        assertEquals(2, result.get("x").asInt());

        boolean threw = false;
        try {
            result.set("y", JSValue.aNumber(3));
        } catch (JSError err) {
            threw = true;
            assertEquals("not extensible", err.getMessage());
        }
        assertTrue(threw);

        assertTrue(result.get("y").isUndefined());
    }

    @Test
    public void testFrozen() throws Exception {
        JSObject result = jsvm.evaluate("Object.freeze({x: 2})").asObject();

        assertEquals(2, result.get("x").asInt());

        boolean threw = false;
        try {
            result.set("x", JSValue.aNumber(3));
        } catch (JSError err) {
            threw = true;
            assertEquals("not writable", err.getMessage());
        }
        assertTrue(threw);

        assertEquals(2, result.get("x").asInt());
    }
}
