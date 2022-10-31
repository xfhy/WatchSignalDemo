package com.xfhy.watchsignaldemo;

import android.os.Looper;
import android.util.Log;

/**
 * @author : xfhy
 * Create time : 2022/10/30 7:07
 * Description :
 */
public class Util {

    public static void printMainThreadTrace() {
        StackTraceElement[] stackTrace = Looper.getMainLooper().getThread().getStackTrace();
        StringBuilder stringBuilder = new StringBuilder();
        for (StackTraceElement stackTraceElement : stackTrace) {
            stringBuilder.append(stackTraceElement.toString()).append("\n");
        }
        Log.d("xfhy_anr", stringBuilder.toString());
    }
}
