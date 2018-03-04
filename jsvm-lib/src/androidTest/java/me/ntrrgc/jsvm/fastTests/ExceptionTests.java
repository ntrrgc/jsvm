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

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNotNull;
import static org.junit.Assert.assertTrue;

public class ExceptionTests {
    private JSVM jsvm;

    @Before
    public void setUp() {
        jsvm = new JSVM();
        assertEquals(0, jsvm.getStackSize());
    }

    @After
    public void tearDown() {
    }

    @Test
    public void testUnhandledErrorsThrow() throws Exception {
        boolean threw = false;
        try {
            jsvm.evaluate("throw new Error('oops!');");
        } catch (JSError error) {
            threw = true;
            assertEquals(0, jsvm.getStackSize());

            JSObject errorObject = error.getErrorValue().asObjectOrNull();
            assertEquals(0, jsvm.getStackSize());
            assertNotNull(errorObject);

            assertEquals("oops!", errorObject.get("message").asStringOrNull());
            assertEquals(0, jsvm.getStackSize());

            assertEquals("oops!", error.getMessage());
        }
        assertTrue(threw);
    }

    @Test
    public void testUnhandledErrorsInsideFunctionThrow() throws Exception {
        boolean threw = false;
        try {
            jsvm.evaluate("function hello() { throw new Error('oops!'); } hello();");
        } catch (JSError error) {
            threw = true;
            assertEquals(0, jsvm.getStackSize());

            JSObject errorObject = error.getErrorValue().asObjectOrNull();
            assertNotNull(errorObject);
            assertEquals("oops!", errorObject.get("message").asStringOrNull());
            assertEquals(0, jsvm.getStackSize());

            assertEquals("oops!", error.getMessage());
        }
        assertTrue(threw);
    }
}
