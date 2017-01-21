//
// Created by ntrrgc on 1/15/17.
//


#include <jni.h>
#include <duktape/duk_config.h>
#include "utils.h"
#include <sstream>
#include <iomanip>
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
static may_throw
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
        assert(writeChar < dstUtf16StringEnd);

        jchar newChar;

        if (*readChar < 0x80) { // 0xxx xxxx
            newChar = *readChar;
        } else if ((*readChar & 0xE0) == 0xC0) { // 110x xxxx
            newChar = static_cast<jchar>(*readChar & 0x1F) << 6;

            readChar++;
            assert(readChar < cesu8StringEnd);
            assert((*readChar & 0xC0) == 0x80); // 10xx xxxx
            newChar = newChar | static_cast<jchar>(*readChar & 0x3F);
        } else if ((*readChar & 0xF0) == 0xE0) { // 1110 xxxx
            newChar = static_cast<jchar>(*readChar & 0x0F) << 12;

            readChar++;
            assert(readChar < cesu8StringEnd);
            assert((*readChar & 0xC0) == 0x80); // 10xx xxxx
            newChar = newChar | (static_cast<jchar>(*readChar & 0x3F) << 6);

            readChar++;
            assert(readChar < cesu8StringEnd);
            assert((*readChar & 0xC0) == 0x80); // 10xx xxxx
            newChar = newChar | static_cast<jchar>(*readChar & 0x3F);
        } else {
            std::stringstream ss;
            ss << "Unexpected CESU-8 header: 0x" << std::hex
                << static_cast<int>(*readChar);
            env->ThrowNew(JSRuntimeException_Class, ss.str().c_str());
            return THREW_EXCEPTION;
        }

        *writeChar = newChar;
    }

    return OK;
}

Result<jstring>
jsvm::String_createFromStackTop(JNIEnv *env, duk_context *ctx) {
#ifdef ENABLE_EMBEDDED_NULL_INTEROP
    // Length of the script in UTF-16 code units (16 bits long),
    // without terminator character.
    const jsize codeUnitLength = (const jsize) duk_get_length(ctx, -1);
    duk_size_t dukStringByteLength;
    const char * dukString = duk_get_lstring(ctx, -1, &dukStringByteLength);

    Result<jstring> ret;

    if (codeUnitLength <= 256) {
        jchar utf16String[codeUnitLength];

        if (THREW_EXCEPTION == decodeCesu8IntoUtf16(env, utf16String, dukString,
                                                    dukStringByteLength, codeUnitLength)) {
            ret = Result<jstring>::createThrew();
        } else {
            ret = Result<jstring>::createOK(env->NewString(utf16String, codeUnitLength));
        }

    } else {
        jchar* utf16String = new jchar[codeUnitLength];

        if (THREW_EXCEPTION == decodeCesu8IntoUtf16(env, utf16String, dukString,
                                                    dukStringByteLength, codeUnitLength)) {
            ret = Result<jstring>::createThrew();
        } else {
            ret = Result<jstring>::createOK(env->NewString(utf16String, codeUnitLength));
        }

        delete[] utf16String;
    }

    return ret;
#else
    return Result::ok(env->NewStringUTF(duk_get_string(ctx, -1)));
#endif
}

