package me.ntrrgc.jsvm;

import android.support.test.runner.AndroidJUnit4;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;
import org.junit.runner.RunWith;

import static org.junit.Assert.assertEquals;

/**
 * These are simple tests intended to test GC behavior is stable when
 * running applications that create lots of temporary JSObject's.
 */
@RunWith(AndroidJUnit4.class)
public class SlowTests {
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
    public void testLotsOfSmallObjects() throws Exception {
        jsvm.evaluateScript("function getObject() { return {'x': 2 }; }");
        JSFunction getObject = jsvm.getGlobalScope().get("getObject").asFunction();

        for (long i = 0; i < 400000L; i++) {
            getObject.invoke();
        }
    }
}
