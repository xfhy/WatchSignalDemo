#include <jni.h>
#include <string>
#include <android/log.h>
#include <unistd.h>
#include <pthread.h>
#include "managed_jnienv.h"

static jclass utilClazz;
static jmethodID printMainThreadTraceMethod;

void signalHandler(int sig, siginfo_t *info, void *uc) {
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

    //todo xfhy 这里有问题
    //'JNI DETECTED ERROR IN APPLICATION: JNI GetStaticMethodID called with pending exception java.lang.NoClassDefFoundError: Class not found using the boot class loader; no stack trace available
    //    (Throwable with no stack trace)
    //
    //        in call to GetStaticMethodID
    //        from void java.lang.Thread.sleep(java.lang.Object, long, int)'
    JNIEnv *env = JniInvocation::getEnv();
    utilClazz = env->FindClass("com/xfhy/watchsignaldemo/Util");
    printMainThreadTraceMethod = env->GetStaticMethodID(utilClazz, "printMainThreadTrace", "()V");
    env->CallStaticVoidMethod(utilClazz, printMainThreadTraceMethod);
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

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *) {
    JniInvocation::init(vm);

    JNIEnv *env;
    if (vm->GetEnv(reinterpret_cast<void **>(&env), JNI_VERSION_1_6) != JNI_OK)
        return -1;



    return JNI_VERSION_1_6;
}