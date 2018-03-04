/* Copyright (c) the JSVM authors
 * https://github.com/ntrrgc/jsvm
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

package me.ntrrgc.jsvm.manualTests;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.TextView;

import org.apache.commons.io.Charsets;
import org.apache.commons.io.IOUtils;

import me.ntrrgc.jsvm.manualTests.performanceTests.PerformanceTests;
import me.ntrrgc.jsvm.R;

public class MainActivity extends AppCompatActivity {
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

//        JSVM jsvm = new JSVM();
//        JSValue v = jsvm.evaluate("'Hello from' + ' JS'");
//        Log.i("OKA", String.valueOf(v.jsVM == jsvm));
//        tv.setText(v.asStringOrNull());

//
//        String bluebird = null;
//        try {
//            bluebird = IOUtils.toString(getResources().openRawResource(R.raw.bluebird), Charsets.UTF_8);
//        } catch (IOException e) {
//            e.printStackTrace();
//        }
//        jsvm.evaluate("function setTimeout() {}");
//
//        Long a = System.currentTimeMillis();
//        jsvm.evaluate(bluebird);
//        Long b = System.currentTimeMillis();
//        Log.i("BLUEBIRD", "Loaded in " + (b - a) + " ms");
    }

    public void runTest(View view) {
        // Example of a call to a native method
        TextView tv = (TextView) findViewById(R.id.sample_text);

        PerformanceTests tests = new PerformanceTests();

        String testResults = tests.testPropertyRead() + "\n" + tests.testObjectCreation();

        tv.setText(testResults);
    }
}
