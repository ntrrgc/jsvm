package me.ntrrgc.jsvm;

import android.support.test.runner.AndroidJUnit4;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;
import org.junit.runner.RunWith;

import java.util.ArrayList;

import me.ntrrgc.jsvm.JSObject;
import me.ntrrgc.jsvm.JSVM;

import static org.junit.Assert.assertEquals;

@RunWith(AndroidJUnit4.class)
public class JSObjectIdentityRace {
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

    /* package */ static void hopeForGC() throws Exception {
        // Do something that generates garbage and hopefully will
        // not be optimized out by the JIT compiler

        ArrayList<Object> list = new ArrayList<>(1024);
        for (int i = 0; i < 2048; i++) {
            list.add(new Object());
        }

        int hash = 0;
        for (int i = 2047; i >= 0; i--) {
            hash = hash ^ list.get(i).hashCode();
        }

        // Request GC and wait a bit
        System.gc();
        // generates a number between 0 and 1 from a signed int32
        double maxOne = (double) Math.abs(hash) / Math.pow(2.0, 31.0);
        // Wait 100 ms + up to 10 ms
        Thread.sleep(100 + Math.round(maxOne * 10));
        System.runFinalization();
    }

    private int getJSObjectHandle(JSObject jsObject) throws Exception {
        return JSObject.class.getDeclaredField("handle").getInt(jsObject);
    }

    @Test
    public void testNoRaceConditions() throws Exception {
        jsvm.evaluate("jsThing = {};");

        int handleFirst = getJSObjectHandle(jsvm.evaluate("jsThing").asObject());

        hopeForGC();

        int handleSecond = getJSObjectHandle(jsvm.evaluate("jsThing").asObject());

        assertEquals(handleFirst, handleSecond);
    }
}
