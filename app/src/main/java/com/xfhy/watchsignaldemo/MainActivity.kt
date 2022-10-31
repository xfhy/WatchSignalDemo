package com.xfhy.watchsignaldemo

import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.os.SystemClock
import android.util.Log
import android.view.View
import com.xfhy.watchsignaldemo.databinding.ActivityMainBinding

class MainActivity : AppCompatActivity() {

    companion object {
        init {
            System.loadLibrary("watchsignaldemo")
        }
    }

    private lateinit var binding: ActivityMainBinding

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        binding = ActivityMainBinding.inflate(layoutInflater)
        setContentView(binding.root)
    }

    external fun startWatch(): Boolean

    fun startWatch(view: View) {
        val result = startWatch()
        Log.d("xfhy_anr", "startWatch result = $result")
    }

    fun makeAnr(view: View) {
        SystemClock.sleep(20000)
        //多次点击,会触发input anr,根据anr日志,一下就能定位到
        //java.lang.Thread.sleep(Native Method)
        //    java.lang.Thread.sleep(Thread.java:443)
        //    java.lang.Thread.sleep(Thread.java:359)
        //    android.os.SystemClock.sleep(SystemClock.java:131)
        //    com.xfhy.watchsignaldemo.MainActivity.makeAnr(MainActivity.kt:35)
        //    java.lang.reflect.Method.invoke(Native Method)
        //    androidx.appcompat.app.AppCompatViewInflater$DeclaredOnClickListener.onClick(AppCompatViewInflater.java:441)
        //    android.view.View.performClick(View.java:7317)
        //    com.google.android.material.button.MaterialButton.performClick(MaterialButton.java:1219)
        //    android.view.View.performClickInternal(View.java:7291)
        //    android.view.View.access$3600(View.java:838)
        //    android.view.View$PerformClick.run(View.java:28247)
        //    android.os.Handler.handleCallback(Handler.java:900)
        //    android.os.Handler.dispatchMessage(Handler.java:103)
        //    android.os.Looper.loop(Looper.java:219)
        //    android.app.ActivityThread.main(ActivityThread.java:8668)
        //    java.lang.reflect.Method.invoke(Native Method)
        //    com.android.internal.os.RuntimeInit$MethodAndArgsCaller.run(RuntimeInit.java:513)
        //    com.android.internal.os.ZygoteInit.main(ZygoteInit.java:1109)
    }

}