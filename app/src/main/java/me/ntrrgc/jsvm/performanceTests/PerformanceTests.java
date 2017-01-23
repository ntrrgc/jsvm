package me.ntrrgc.jsvm.performanceTests;

import android.util.Log;

import me.ntrrgc.jsvm.JSObject;
import me.ntrrgc.jsvm.JSVM;
import me.ntrrgc.jsvm.JSValue;
import me.ntrrgc.jsvm.performanceTests.controlGroup.DummyPOJO;

/**
 * Created by ntrrgc on 1/22/17.
 */

public class PerformanceTests {
    private JSObject createDummyJSObject(JSVM jsVM, double value) {
        JSObject object = jsVM.evaluateScript("({0: 1})").asObjectOrNull();
        assert object != null;
        object.set(0, JSValue.aNumber(value));
        return object;
    }

    private String formatInstant(Double seconds) {
        if (seconds > 1.0) {
            return seconds + "s";
        } else if (seconds > 1e-3) {
            return seconds * 1e3 + "ms";
        } else if (seconds > 1e-6) {
            return seconds * 1e6 + "us";
        } else if (seconds > 1e-9) {
            return seconds * 1e9 + "ns";
        } else {
            return seconds * 1e12 + "ps";
        }
    }

    public String testJNIOverhead() {
        final long iterations = 100000;
        long i;
        long start;
        double tmp = 1;
        StringBuilder log = new StringBuilder();

        log.append("Starting testJNIOverhead:\n");

        start = System.currentTimeMillis();
        for (i = 0; i < iterations; i++) {
            tmp = tmp * JSVM.returnADouble();
        }
        final long totalTime = System.currentTimeMillis() - start;
        log.append("Time JNI overhead test: " + totalTime + ".\n");
        log.append("Time per iteration: " +
                formatInstant((double)totalTime / iterations / 1000) + ".\n");
        Log.d("tmp", Double.toString(tmp));

        Log.i("Test results", log.toString());
        return log.toString();
    }

    public String testPropertyRead() {
        final long iterations = 100000;
        long i;
        long start;
        double tmp = 1;
        StringBuilder log = new StringBuilder();

        JSVM jsVM = new JSVM();
        JSObject[] jsObjects = new JSObject[] {
                createDummyJSObject(jsVM, 1.0),
                createDummyJSObject(jsVM, 2.0),
                createDummyJSObject(jsVM, 3.0),
                createDummyJSObject(jsVM, 4.0),
                createDummyJSObject(jsVM, 5.0),
        };

        DummyPOJO[] pojos = new DummyPOJO[] {
                new DummyPOJO(1.0),
                new DummyPOJO(2.0),
                new DummyPOJO(3.0),
                new DummyPOJO(4.0),
                new DummyPOJO(5.0)
        };

        log.append("Starting testPropertyRead:\n");

        start = System.currentTimeMillis();
        for (i = 0; i < iterations; i++) {
            tmp = tmp * pojos[(int) (i % 5)].getValue();
        }
        final long timeJavaPojos = System.currentTimeMillis() - start;
        log.append("Time Java POJOs: " + timeJavaPojos + ".\n");
        log.append("Time per iteration: " +
                formatInstant((double)timeJavaPojos / iterations / 1000) + ".\n");

        start = System.currentTimeMillis();
        for (i = 0; i < iterations; i++) {
            tmp = tmp * jsObjects[(int) (i % 5)].get(0).asDoubleOrNull();
        }
        final long timeJSObjects = System.currentTimeMillis() - start;
        log.append("Time JS Objects: " + timeJSObjects + ".\n");
        log.append("Time per iteration: " +
                formatInstant((double)timeJSObjects / iterations / 1000) + ".\n");

        log.append("JS is " + (double)timeJSObjects / timeJavaPojos + " times slower.\n");

        Log.i("Test results", log.toString());
        return log.toString();
    }
}