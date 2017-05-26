package com.app.superxlcr.mynativetest;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.util.Log;

/**
 * Created by superxlcr on 2017/5/22.
 */

public class WakeUpBroadcastReceiver extends BroadcastReceiver {

    private static final String TAG = "MyLog";

    @Override
    public void onReceive(Context context, Intent intent) {
        // 拉活处理
        Log.d(TAG, "Receiver Start Service");
        Intent serviceIntent = new Intent(context, PersistService.class);
        context.startService(serviceIntent);
    }
}
