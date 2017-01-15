#include <jni.h>
#include <string>

#include <stdafx.h>

extern "C" {

jstring
Java_me_ntrrgc_jsvm_JSVM_stringFromJNI(
        JNIEnv *env,
        jobject /* this */) {
    std::string hello = "Hello from C++, again";
    return env->NewStringUTF(hello.c_str());
}

}