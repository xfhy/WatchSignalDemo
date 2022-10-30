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
            Util().setClsRef()
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


        //in call to CallStaticVoidMethodV
        //        from void java.lang.Thread.sleep(java.lang.Object, long, int)'
    }

}