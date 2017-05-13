//
// Created by ntrrgc on 5/13/17.
//
#include <jni.h>

extern "C" {

JNIEXPORT jint JNICALL
Java_me_ntrrgc_mylibrary_Miau_meow(JNIEnv *env, jclass type) {

    return 5;

}

};