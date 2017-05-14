package me.ntrrgc.jsvm;

import android.support.test.runner.AndroidJUnit4;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;
import org.junit.runner.RunWith;

import java.util.ArrayList;

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
        jsvm.evaluate("function getObject() { return {'x': 2 }; }");
        JSFunction getObject = jsvm.getGlobalScope().get("getObject").asFunction();

        for (long i = 0; i < 400000L; i++) {
            getObject.invoke();
        }
    }

    @Test
    public void testLotsOfHugeObjects() throws Exception {
        StringBuffer megabyteStringBuilder;
        megabyteStringBuilder = new StringBuffer();
        for (int i = 0; i < 1024 * 1024; i++) {
            megabyteStringBuilder.append("a");
        }
        String megabyteString = megabyteStringBuilder.toString();
        megabyteStringBuilder = null;

        // This test is successful if it's able to create 1 GB worth of these
        // objects without crashing, which is more memory than the Android
        // emulator has with default settings.
        ArrayList<JSObject> arr = new ArrayList<>();
        for (int i = 0; i < 1000; i++) {
            JSObject obj = jsvm.newObject();
            obj.set("heldHugeString", JSValue.aString(megabyteString + Integer.toHexString(i)));
        }
    }

    /*
     * It would be nice if OOM conditions produced inside JS were caught
     * inside JSVM and not the OS, but getting such thing to work
     * would require custom allocators, which could get complicated.
     *
     * So far, the test is disabled in order not to crash the tests process.
     */

    /*
    @Test
    public void testOOMInsideJS() throws Exception {
        try {
            jsvm.evaluate("var array = [''];\n" +
                    "var i = 0;\n" +
                    "while (true) {\n" +
                    "   // Each iteration stores a 1 character longer string," +
                    "   // which makes the program to exhaust memory very quickly.\n" +
                    "   array.push(array[i++] + 'a');\n" +
                    "}\n");
        } catch (OutOfMemoryError ignored) {
            // Happy Java error instead of JNI process crash
        }
    }
    */

}
