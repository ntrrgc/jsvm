#include <jni.h>
#include <string>

#include "duktape/duktape.h"

extern "C" {

static jclass JSVM;
static jfieldID JSVM_hDukContext;

static jclass JSRuntimeException;

static jclass JSValue;
static jfieldID JSValue_jsVM;

static duk_context * JSVM_getDukContext(JNIEnv *env, jobject instance) {
    return (duk_context *) env->GetLongField(instance, JSVM_hDukContext);
}

static duk_context * JSValue_getDukContext(JNIEnv *env, jobject instance) {
    jobject jsVM = env->GetObjectField(instance, JSValue_jsVM);
    return JSVM_getDukContext(env, jsVM);
}

JNIEXPORT void JNICALL
Java_ntrrgc_me_jsvm_JSVM_finalize(JNIEnv *env, jobject instance) {

    duk_context* ctx = JSVM_getDukContext(env, instance);

    if (ctx != NULL) {
        duk_destroy_heap(ctx);
    }

}

JNIEXPORT jstring JNICALL
Java_ntrrgc_me_jsvm_JSValue_asString(JNIEnv *env, jobject instance) {

    duk_context *ctx = JSValue_getDukContext(env, instance);

    duk_push_global_stash(ctx);
    duk_push_pointer(ctx, instance);
    duk_get_prop(ctx, -2);

    jstring returnValue = env->NewStringUTF(duk_get_string(ctx, -1));

    duk_pop_2(ctx); // pop value and stash

    return returnValue;
}

JNIEXPORT void JNICALL
Java_ntrrgc_me_jsvm_JSVM_nativeInit(JNIEnv *env, jobject instance) {
    duk_context * ctx = duk_create_heap(
            NULL, NULL, NULL, NULL, NULL
    );
    if (ctx == NULL) {
        env->ThrowNew(JSRuntimeException, "Failed to create JS heap");
        return;
    }

    duk_push_global_object(ctx);
    duk_push_string(ctx, "global");
    duk_push_global_object(ctx);
    duk_put_prop(ctx, -3);

    env->SetLongField(instance, JSVM_hDukContext, (jlong) ctx);

}

jclass hookClass(JNIEnv *env, const char *name) {
    jclass tmpClassRef;

    tmpClassRef = env->FindClass(name);
    jclass ret = (jclass) env->NewGlobalRef(tmpClassRef);
    env->DeleteLocalRef(tmpClassRef);

    return ret;
}

JNIEXPORT void JNICALL
Java_ntrrgc_me_jsvm_JSVM_initialize(JNIEnv *env, jclass type) {

    JSVM = hookClass(env, "ntrrgc/me/jsvm/JSVM");
    JSVM_hDukContext = env->GetFieldID(JSVM, "hDukContext", "J");

    JSRuntimeException = hookClass(env, "ntrrgc/me/jsvm/JSRuntimeException");

    JSValue = hookClass(env, "ntrrgc/me/jsvm/JSValue");
    JSValue_jsVM = env->GetFieldID(JSValue, "jsVM", "L" "ntrrgc/me/jsvm/JSVM;");
}

JNIEXPORT void JNICALL
Java_ntrrgc_me_jsvm_JSVM_destroy(JNIEnv *env, jclass type) {

    env->DeleteGlobalRef(JSVM);
    env->DeleteGlobalRef(JSValue);
    env->DeleteGlobalRef(JSRuntimeException);

}

jobject readStackTopIntoJSValue(JNIEnv *env, jobject jsVM) {
    // Create JSValue (get a pointer to the new instance, too!)
    jobject jsValue = env->AllocObject(JSValue);
    env->SetObjectField(jsValue, JSValue_jsVM, jsVM);

    duk_context *ctx = JSVM_getDukContext(env, jsVM);

    // Use the JSValue pointer as index in the global stash so that
    // the value is not GCed in JSland while JSValue is alive in Javaland.
    duk_push_global_stash(ctx); // stash
    duk_push_pointer(ctx, jsValue); // key
    duk_dup(ctx, -3); // value
    duk_put_prop(ctx, -3); // save in stash

    // Pop the stash
    duk_pop(ctx);

    return jsValue;
}

JNIEXPORT jobject JNICALL
Java_ntrrgc_me_jsvm_JSVM_evaluateScript(JNIEnv *env, jobject instance, jstring code_) {
    const char *code = env->GetStringUTFChars(code_, 0);
    duk_context *ctx = JSVM_getDukContext(env, instance);

    duk_eval_string(ctx, code);
    jobject returnValue = readStackTopIntoJSValue(env, instance);
    duk_pop(ctx);

    env->ReleaseStringUTFChars(code_, code);

    return returnValue;
}

jstring
Java_ntrrgc_me_jsvm_JSVM_stringFromJNI(
        JNIEnv *env,
        jobject /* this */) {
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}

}