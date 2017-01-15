package me.ntrrgc.jsvm;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.widget.TextView;

import org.apache.commons.io.Charsets;
import org.apache.commons.io.IOUtils;

import java.io.IOException;

public class MainActivity extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        JSVM jsvm = new JSVM();

        // Example of a call to a native method
        TextView tv = (TextView) findViewById(R.id.sample_text);
        tv.setText(JSVM.stringFromJNI());
//        JSValue v = jsvm.evaluateScript("'Hello from' + ' JS'");
//        Log.i("OKA", String.valueOf(v.jsVM == jsvm));
//        tv.setText(v.asString());


        String bluebird = null;
        try {
            bluebird = IOUtils.toString(getResources().openRawResource(R.raw.bluebird), Charsets.UTF_8);
        } catch (IOException e) {
            e.printStackTrace();
        }
        jsvm.evaluateScript("function setTimeout() {}");

        Long a = System.currentTimeMillis();
        jsvm.evaluateScript(bluebird);
        Long b = System.currentTimeMillis();
        Log.i("BLUEBIRD", "Loaded in " + (b - a) + " ms");
    }

}
