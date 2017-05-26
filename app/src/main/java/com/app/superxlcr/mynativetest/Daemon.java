package com.app.superxlcr.mynativetest;

import android.app.AlarmManager;
import android.app.PendingIntent;
import android.content.Context;
import android.content.Intent;
import android.os.Process;
import android.os.SystemClock;
import android.util.Log;

/**
 * Created by superxlcr on 2017/5/13.
 */

public class Daemon {

    private static final String TAG = "MyLog";

    private static final String DIR_NAME = "bin";
    private static final String FILE_NAME = "serviceDaemon";

    private static AlarmManager alarmManager;
    public static PendingIntent wakeUpIntent;

    static {
        System.loadLibrary("daemon-lib");
    }

    /**
     * 运行守护进程
     * @param context 上下文
     * @param wakeUpClass 唤醒Service类
     */
    public static void run(final Context context, final Class wakeUpClass) {
        // 初始化闹钟与唤醒用的intent
        alarmManager = ((AlarmManager)context.getSystemService(Context.ALARM_SERVICE));
        Intent intent = new Intent(context, wakeUpClass);
        wakeUpIntent = PendingIntent.getBroadcast(context, 0, intent, 0);
        // 启动守护进程
        new Thread(new Runnable() {
            @Override
            public void run() {
                // 复制binary文件
                String binaryFilePath = Command.install(context, DIR_NAME, FILE_NAME);
                if (binaryFilePath != null) {
                    // 运行
                    start(context.getPackageName(), wakeUpClass.getSimpleName(), binaryFilePath);
                }
            }
        }).start();
    }

    private native static void start(String packageName, String serviceClassName, String binaryFilePath);

    public static void onDaemonDead() {
        Log.d(TAG, "call onDaemonDead!");
        // 闹钟启动拉活进程
        alarmManager.set(AlarmManager.ELAPSED_REALTIME, SystemClock.elapsedRealtime() + 5000, wakeUpIntent);
        // 自杀
        Process.killProcess(Process.myPid());
    }

}
