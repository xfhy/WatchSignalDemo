#include <jni.h>
#include <string>
#include <android/log.h>
#include <unistd.h>
#include <pthread.h>
#include "managed_jnienv.h"

static jclass utilClazz;
static jmethodID utilPrintMainThreadTrace;

void anrDumpCallback() {
    JNIEnv *env = JniInvocation::getEnv();
    if (!env) {
        return;
    }
    env->CallStaticVoidMethod(utilClazz, utilPrintMainThreadTrace);
}

static void *anrCallback(void *arg) {
    //当前进程收到了其他进程发来的SIGQUIT信号,可能是当前进程发生ANR了,先质疑
    anrDumpCallback();
//    if (strlen(mAnrTraceFile) > 0) {
//        //hook app进程发往系统进程的socket的write,
//        hookAnrTraceWrite(false);
//    }
//    //重新转发信号
//    sendSigToSignalCatcher();
    return nullptr;
}

void signalHandler(int sig, siginfo_t *info, void *uc) {
    __android_log_print(ANDROID_LOG_DEBUG, "xfhy_anr", "我监听到SIGQUIT信号了,可能发生anr了");

    //这里必须得新起一个线程去dump主线程堆栈,否则的话,这里是主线程,主线程本来就卡住了,你还给它增加负担,这是不合适的
    //创建线程,在线程里面去获取主线程堆栈
    pthread_t thd;
    pthread_create(&thd, nullptr, anrCallback, nullptr);
    pthread_detach(thd);
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

//android虚拟机执行到System.loadLibrary()函数时,会首先执行JNI_OnLoad.
//JNI_OnLoad有2个作用:
//1. 告诉虚拟机,此C组件使用的是哪一个版本的JNI
//2. 初始化一些开发者自己的东西
JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *) {
    JniInvocation::init(vm);

    JNIEnv *env = JniInvocation::getEnv();
    utilClazz = env->FindClass("com/xfhy/watchsignaldemo/Util");
    utilPrintMainThreadTrace = env->GetStaticMethodID(utilClazz, "printMainThreadTrace", "()V");

    return JNI_VERSION_1_6;
}