package me.ntrrgc.jsvm;

import android.content.Context;
import android.support.test.InstrumentationRegistry;
import android.support.test.runner.AndroidJUnit4;
import android.util.Log;

import org.junit.Test;
import org.junit.runner.RunWith;

import static org.junit.Assert.*;

/**
 * Instrumentation test, which will execute on an Android device.
 *
 * @see <a href="http://d.android.com/tools/testing">Testing documentation</a>
 */
@RunWith(AndroidJUnit4.class)
public class ExampleInstrumentedTest {
    @Test
    public void useAppContext() throws Exception {
        // Context of the app under test.
        Context appContext = InstrumentationRegistry.getTargetContext();

        assertEquals("ntrrgc.me.jsvm", appContext.getPackageName());
        assertEquals("Hello from C++", JSVM.stringFromJNI());
    }

    @Test
    public void evalSimpleScript() throws Exception {
        JSVM jsvm = new JSVM();
        assertEquals("hello world", jsvm.evaluateScript("'hello ' + 'world'").asString());
    }

    @Test
    public void libLoadTime() throws Exception {
        JSVM jsvm = new JSVM();

//        String bluebird = IOUtils.toString(JSVM.class.getClassLoader().getResourceAsStream("/assets/bluebird.min.js"), Charsets.UTF_8);

        jsvm.evaluateScript("function setTimeout() {}");

        Long a = System.currentTimeMillis();
//        jsvm.evaluateScript(bluebird);
        Long b = System.currentTimeMillis();
        Log.i("BLUEBIRD", "Loaded in " + (b - a) + " ms");
    }
}
