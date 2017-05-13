package me.ntrrgc.jsvm.fastTests;

import org.junit.Test;

import me.ntrrgc.mylibrary.Miau;

import static org.junit.Assert.assertEquals;

/**
 * Created by ntrrgc on 5/14/17.
 */

public class LibraryTests {
    @Test
    public void testLib() {
        assertEquals(5, Miau.meow());
    }
}
