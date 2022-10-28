package com.xfhy.watchsignaldemo

import android.os.Looper
import android.util.Log

/**
 * @author : xfhy
 * Create time : 2022/10/28 6:00 下午
 * Description :
 */
object Util {
    @JvmStatic
    fun printMainThreadTrace() {
        val stackTrace = Looper.getMainLooper().thread.stackTrace
        val trace = StringBuilder().apply {
            for (stackTraceElement in stackTrace) {
                append(stackTraceElement.toString())
                append("\n")
            }
        }.toString()
        Log.e("xfhy_anr", trace)
    }
}