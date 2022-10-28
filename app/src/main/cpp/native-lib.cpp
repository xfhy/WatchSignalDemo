#include <jni.h>
#include <string>
#include <android/log.h>
#include <unistd.h>
#include <pthread.h>

extern "C" JNIEXPORT jstring JNICALL
Java_com_xfhy_watchsignaldemo_MainActivity_stringFromJNI(
        JNIEnv* env,
        jobject /* this */) {
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}

//bool SignalHandler::installHandlersLocked() {
//    if (sHandlerInstalled) {
//        return false;
//    }
//
//    //sigaction函数的功能是检查或修改与指定信号相关联的处理动作, 函数原型：int sigaction(int signum, const struct sigaction *act, struct sigaction *oldact);
//    //signum参数指出要捕获的信号类型，act参数指定新的信号处理方式，oldact参数输出先前信号的处理方式（如果不为NULL的话）
//    //建立一个Signal Handler,TARGET_SIG的定义: static const int TARGET_SIG = SIGQUIT;
//    if (sigaction(TARGET_SIG, nullptr, &sOldHandlers) == -1) {
//        return false;
//    }
//
//    struct sigaction sa{};
//    //方法地址,收到信号的地方,收到信号之后会调用signalHandler方法
//    sa.sa_sigaction = signalHandler;
//    sa.sa_flags = SA_ONSTACK | SA_SIGINFO | SA_RESTART;
//
//    //将sa设置为Signal Handler
//    if (sigaction(TARGET_SIG, &sa, nullptr) == -1) {
//        return false;
//    }
//
//    sHandlerInstalled = true;
//    return true;
//}

void signalHandler(int sig, siginfo_t* info, void* uc) {
//    int fromPid1 = info->_si_pad[3];
//    int fromPid2 = info->_si_pad[4];
//    int myPid = getpid();
//    //判断是否为自己进程发的SIGQUIT信号
//    bool fromMySelf = fromPid1 == myPid || fromPid2 == myPid;
//    if (sig == SIGQUIT) {
//        pthread_t thd;
//        if (!fromMySelf) {
//            pthread_create(&thd, nullptr, anrCallback, nullptr);
//        }
//        pthread_detach(thd);
//    }
    __android_log_print(ANDROID_LOG_DEBUG, "xfhy_anr", "我监听到信号了");
}

extern "C"
JNIEXPORT jboolean JNICALL
Java_com_xfhy_watchsignaldemo_MainActivity_startWatch(JNIEnv *env, jobject thiz) {
    sigset_t set, old_set;
    sigemptyset(&set);
    sigaddset(&set, SIGQUIT);

    /*
     * 这里需要调用SIG_UNBLOCK，因为目标进程被Zogyte fork出来的时候，主线程继承了
     * Zogyte的主线程的信号屏蔽关系，Zogyte主线程在初始化的时候，通过
     * pthread_sigmask SIG_BLOCK把SIGQUIT的信号给屏蔽了，因此我们需要在自己进程的主线程，
     * 设置pthread_sigmask SIG_UNBLOCK ，这会导致原来的SignalCatcher sigwait将失效，
     * 原因是SignalCatcher 线程会对SIGQUIT 信号处理
     */
    int r = pthread_sigmask(SIG_UNBLOCK, &set, &old_set);
    if (0 != r) {
        return false;
    }

    struct sigaction sa{};
    sa.sa_sigaction = signalHandler;
    sa.sa_flags = SA_ONSTACK | SA_SIGINFO | SA_RESTART;

    return sigaction(SIGQUIT, &sa, nullptr) == -1;
}

