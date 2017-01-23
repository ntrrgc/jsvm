package me.ntrrgc.jsvm;

import android.support.test.runner.AndroidJUnit4;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;
import org.junit.runner.RunWith;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNotNull;
import static org.junit.Assert.assertTrue;

@RunWith(AndroidJUnit4.class)
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
            jsvm.evaluateScript("throw new Error('oops!');");
        } catch (JSError error) {
            threw = true;
            assertEquals(0, jsvm.getStackSize());

            JSObject errorObject = error.getErrorValue().asObject();
            assertEquals(0, jsvm.getStackSize());
            assertNotNull(errorObject);

            assertEquals("oops!", errorObject.get("message").asString());
            assertEquals(0, jsvm.getStackSize());

            assertEquals("oops!", error.getMessage());
        }
        assertTrue(threw);
    }

    @Test
    public void testUnhandledErrorsInsideFunctionThrow() throws Exception {
        boolean threw = false;
        try {
            jsvm.evaluateScript("function hello() { throw new Error('oops!'); } hello();");
        } catch (JSError error) {
            threw = true;
            assertEquals(0, jsvm.getStackSize());

            JSObject errorObject = error.getErrorValue().asObject();
            assertNotNull(errorObject);
            assertEquals("oops!", errorObject.get("message").asString());
            assertEquals(0, jsvm.getStackSize());

            assertEquals("oops!", error.getMessage());
        }
        assertTrue(threw);
    }
}
