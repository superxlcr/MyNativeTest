package com.app.superxlcr.mynativetest;

import android.app.Service;
import android.content.Intent;
import android.os.IBinder;
import android.support.annotation.Nullable;
import android.util.Log;

/**
 * Created by superxlcr on 2017/5/22.
 */

public class PersistService extends Service {

    private static final String TAG = "MyLog";

    @Nullable
    @Override
    public IBinder onBind(Intent intent) {
        return null;
    }

    @Override
    public void onCreate() {
        super.onCreate();
        // 运行守护Daemon
        Log.d(TAG, "run daemon");
        Daemon.run(this, WakeUpBroadcastReceiver.class);
        new Thread(new Runnable() {
            @Override
            public void run() {
                int i = 0;
                while (true) {
                    Log.d("TestLive", "number is " + i++);
                    try {
                        Thread.sleep(5000);
                    } catch (InterruptedException e) {}
                }
            }
        }).start();
    }

    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {
        Log.d(TAG, "onStartCommand");
        return super.onStartCommand(intent, flags, startId);
    }
}
