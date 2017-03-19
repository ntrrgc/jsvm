//
// Created by ntrrgc on 1/15/17.
//


#include <jni.h>
#include <duktape/duk_config.h>
#include "utils.h"
#include <sstream>
#include <iomanip>
#include <vector>

using namespace jsvm;

/**
 * Decode a CESU-8 string as used in Duktape into a UTF-16 string as
 * used in Java.
 *
 * @param env Used to throw exceptions.
 * @param dstUtf16String Destination string, must have enough code
 * points to fit the source string, so its length must be known
 * beforehand.
 * @param cesu8StringSigned Source string in CESU-8 with or without
 * null terminator.
 * @param cesu8StringByteLength Length of the source string in bytes.
 * Null terminator, if any, is not counted here.
 * @param dstUtf16StringCodeUnitLength Length of the destination
 * string in 16 bit code units. Only used in debug mode: an assertion
 * error will be throw if exceeded.
 */
static void
decodeCesu8IntoUtf16(JNIEnv *env,
                     jchar* dstUtf16String, const char* cesu8StringSigned,
                     duk_size_t cesu8StringByteLength,
                     jsize dstUtf16StringCodeUnitLength)
{
    // Bit operations are already unintuitive enough unsigned, don't
    // make them unnecessarily harder!
    const unsigned char *cesu8String =
            (const unsigned char *) cesu8StringSigned;
    const unsigned char* const cesu8StringEnd =
            cesu8String + cesu8StringByteLength;
    jchar* const dstUtf16StringEnd =
            dstUtf16String + dstUtf16StringCodeUnitLength;

    const unsigned char *readChar;
    jchar *writeChar;
    for (readChar = cesu8String, writeChar = dstUtf16String;
         readChar < cesu8StringEnd;
         readChar++, writeChar++)
    {
        jsvm_assert(writeChar < dstUtf16StringEnd);

        jchar newChar;

        if (*readChar < 0x80) { // 0xxx xxxx
            newChar = *readChar;
        } else if ((*readChar & 0xE0) == 0xC0) { // 110x xxxx
            newChar = static_cast<jchar>(*readChar & 0x1F) << 6;

            readChar++;
            jsvm_assert(readChar < cesu8StringEnd);
            jsvm_assert((*readChar & 0xC0) == 0x80); // 10xx xxxx
            newChar = newChar | static_cast<jchar>(*readChar & 0x3F);
        } else if ((*readChar & 0xF0) == 0xE0) { // 1110 xxxx
            newChar = static_cast<jchar>(*readChar & 0x0F) << 12;

            readChar++;
            jsvm_assert(readChar < cesu8StringEnd);
            jsvm_assert((*readChar & 0xC0) == 0x80); // 10xx xxxx
            newChar = newChar | (static_cast<jchar>(*readChar & 0x3F) << 6);

            readChar++;
            jsvm_assert(readChar < cesu8StringEnd);
            jsvm_assert((*readChar & 0xC0) == 0x80); // 10xx xxxx
            newChar = newChar | static_cast<jchar>(*readChar & 0x3F);
        } else {
            std::stringstream ss;
            ss << "Unexpected CESU-8 header: 0x" << std::hex
                << static_cast<int>(*readChar);
            throw JSVMInternalError(ss.str());
        }

        *writeChar = newChar;
    }
}

jstring
jsvm::String_createFromStack(JNIEnv *env, duk_context *ctx, int stackPosition) {
#ifdef ENABLE_EMBEDDED_NULL_INTEROP
    // Length of the script in UTF-16 code units (16 bits long),
    // without terminator character.
    const jsize codeUnitLength = (const jsize) duk_get_length(ctx, stackPosition);
    duk_size_t dukStringByteLength;
    const char * dukString = duk_get_lstring(ctx, stackPosition, &dukStringByteLength);

    if (codeUnitLength <= 256) {
        jchar utf16String[codeUnitLength];

        decodeCesu8IntoUtf16(env, utf16String, dukString, dukStringByteLength, codeUnitLength);
        return env->NewString(utf16String, codeUnitLength);
    } else {
        std::vector<jchar> utf16String(codeUnitLength);

        decodeCesu8IntoUtf16(env, utf16String.data(), dukString,
                             dukStringByteLength, codeUnitLength);

        return env->NewString(utf16String.data(), codeUnitLength);
    }
#else
    return env->NewStringUTF(duk_get_string(ctx, stackPosition));
#endif
}

void
jsvm::String_pushUTF16(duk_context *ctx,
                       const jchar *utf16String,
                       jsize codePointLength)
{
    const jchar *utf16StringEnd = utf16String + codePointLength;
    
    std::string cesu8str;
    cesu8str.reserve((unsigned int) (codePointLength + 1));

    const jchar* readCharPos;
    for (readCharPos = utf16String; readCharPos < utf16StringEnd; readCharPos++) {
        // Note: jchar is unsigned, so there is no weird arithmetic shift.
        jchar readChar = *readCharPos;

        if (readChar < 0x80) { // 7 bits or less
            cesu8str.push_back((char) readChar); // 0xxx xxxx
        } else if (*readCharPos < 0x800) { // 11 bits or less
            cesu8str.push_back((char) (0xC0 | (readChar >> 6))); // 110x xxxx
            cesu8str.push_back((char) (0x80 | (readChar & 0x3F))); // 10xx xxxx
        } else {
            cesu8str.push_back((char) (0xE0 | (readChar >> 12))); // 1110 xxxx
            cesu8str.push_back((char) (0x80 | ((readChar >> 6) & 0x3F))); // 10xx xxxx
            cesu8str.push_back((char) (0x80 | (readChar & 0x3F))); // 10xx xxxx
        }
    }

    duk_push_lstring(ctx, cesu8str.data(), cesu8str.size());
}

void
jsvm::String_pushJString(JNIEnv *env, jstring string, duk_context *ctx) {
#ifdef ENABLE_EMBEDDED_NULL_INTEROP
    jsize codePointLength = env->GetStringLength(string);
    const jchar *utf16String = env->GetStringCritical(string, NULL);

    String_pushUTF16(ctx, utf16String, codePointLength);

    env->ReleaseStringCritical(string, utf16String);
#else
    const char *modifiedUtf8String = env->GetStringUTFChars(string, NULL);

    duk_push_lstring(ctx, modifiedUtf8String, (duk_size_t) env->GetStringLength(string));

    env->ReleaseStringUTFChars(string, modifiedUtf8String);
#endif
}

bool ::jsvm::checkLocalRefIsNotNullAndRelease(JNIEnv* env, jobject localRef) {
    if (localRef) {
        env->DeleteLocalRef(localRef);
    }
    return localRef != NULL;
}
