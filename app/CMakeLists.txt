# Sets the minimum version of CMake required to build the native
# library. You should either keep the default value or only pass a
# value of 3.4.0 or lower.

cmake_minimum_required(VERSION 3.4.1)

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds it for you.
# Gradle automatically packages shared libraries with your APK.

SET( CMAKE_CXX_FLAGS_RELEASE "-O2" )
SET( CMAKE_C_FLAGS_RELEASE "-O2" )
SET( CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -fstrict-aliasing -fomit-frame-pointer -ffunction-sections -fdata-sections -fvisibility=hidden " )
SET( CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} -fstrict-aliasing -fomit-frame-pointer -ffunction-sections -fdata-sections -fvisibility=hidden " )
SET( CMAKE_SHARED_LINKER_FLAGS  "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--gc-sections " )
add_definitions(-DENABLE_EMBEDDED_NULL_INTEROP)
SET(CMAKE_VERBOSE_MAKEFILE "ON")

add_library( # Sets the name of the library.
             native-lib

             # Sets the library as a shared library.
             SHARED

             # Provides a relative path to your source file(s).
             # Associated headers in the same location as their source
             # file are automatically included.
             src/main/cpp/duktape/duktape.c
             src/main/cpp/duktape/duktape.h
             src/main/cpp/duktape/duk_config.h
             src/main/cpp/stdafx.h
             src/main/cpp/jsvm/javaClasses.h
             src/main/cpp/jsvm/javaClasses.cpp
             src/main/cpp/jsvm/stashIndices.h
             src/main/cpp/jsvm/ObjectBook.h
             src/main/cpp/jsvm/JSVM.h
             src/main/cpp/jsvm/JSVM.cpp
             src/main/cpp/jsvm/JSValue.h
             src/main/cpp/jsvm/JSValue.cpp
             src/main/cpp/jsvm/JSObject.h
             src/main/cpp/jsvm/JSObject.cpp
             src/main/cpp/jsvm/utils.h
             src/main/cpp/jsvm/utils.cpp
             )

 include_directories(src/main/cpp)

# Searches for a specified prebuilt library and stores the path as a
# variable. Because system libraries are included in the search path by
# default, you only need to specify the name of the public NDK library
# you want to add. CMake verifies that the library exists before
# completing its build.

find_library( # Sets the name of the path variable.
              log-lib

              # Specifies the name of the NDK library that
              # you want CMake to locate.
              log )

# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in the
# build script, prebuilt third-party libraries, or system libraries.

target_link_libraries( # Specifies the target library.
                       native-lib

                       # Links the target library to the log library
                       # included in the NDK.
                       ${log-lib} )
