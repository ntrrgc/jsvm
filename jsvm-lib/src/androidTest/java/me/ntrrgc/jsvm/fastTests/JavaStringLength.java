/* Copyright (c) the JSVM authors
 * https://github.com/ntrrgc/jsvm
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

package me.ntrrgc.jsvm.fastTests;

import org.junit.Test;

import java.util.Arrays;

import static org.junit.Assert.assertEquals;

/**
 * Test some assumptions on the behavior of Java strings in the JVM
 * for code compiled in this project.
 *
 * If this test suite fails, it probably has been compiled with the
 * wrong character encoding.
 */
public class JavaStringLength {

    @Test
    public void testAscii() throws Exception {
        assertEquals(1, "a".length());
    }

    @Test
    public void testAcute() throws Exception {
        assertEquals(1, "á".length());
    }

    @Test
    public void testSnowman() throws Exception {
        assertEquals(1, "☃".length());
    }

    @Test
    public void testGrinningCat() throws Exception {
        assertEquals(2, "\uD83D\uDE3A".length());
    }

    @Test
    public void testAcuteCodePoint() throws Exception {
        String string = "á";
        int[] actualCodePoints = new int[string.length()];
        for (int i = 0; i < string.length(); i++) {
            actualCodePoints[i] = string.codePointAt(i);
        }
        assertEquals("[225]", Arrays.toString(actualCodePoints));
    }

}
