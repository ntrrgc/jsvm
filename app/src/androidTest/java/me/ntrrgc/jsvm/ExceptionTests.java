package me.ntrrgc.jsvm;

import android.support.test.runner.AndroidJUnit4;

import org.junit.Before;
import org.junit.Test;
import org.junit.runner.RunWith;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNotNull;
import static org.junit.Assert.assertTrue;

/**
 * Instrumentation test, which will execute on an Android device.
 *
 * @see <a href="http://d.android.com/tools/testing">Testing documentation</a>
 */
@RunWith(AndroidJUnit4.class)
public class ExceptionTests {
    private JSVM jsVM;

    @Before
    public void setUp() {
        jsVM = new JSVM();
    }

    @Test
    public void testUnhandledErrorsThrow() throws Exception {
        boolean threw = false;
        try {
            jsVM.evaluateScript("throw new Error('oops!');");
        } catch (JSError error) {
            threw = true;

            JSObject errorObject = error.getErrorValue().asObject();
            assertNotNull(errorObject);
            assertEquals("oops!", errorObject.get("message").asString());

            assertEquals("oops!", error.getMessage());
        }
        assertTrue(threw);
    }
}
