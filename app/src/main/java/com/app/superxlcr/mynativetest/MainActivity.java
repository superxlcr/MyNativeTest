package com.app.superxlcr.mynativetest;

import android.content.Intent;
import android.os.Bundle;
import android.os.Process;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.widget.TextView;

public class MainActivity extends AppCompatActivity {

    private static final String TAG = "MyLog";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        // 运行Service
        Log.d(TAG, "Activity Start Service");
        Intent intent = new Intent(this, PersistService.class);
        startService(intent);
        finish();
    }
}
