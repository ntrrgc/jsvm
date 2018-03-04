#!/bin/bash
# Copyright (c) the JSVM authors
# https://github.com/ntrrgc/jsvm
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

find . \( \
        -name build -o -name .externalNativeBuild -o -name duktape \
    \) -prune -o \( \
        -name '*.java' -o \
        -name '*.cpp' -o \
        -name '*.h' -o \
        -name '*.c' -o \
        -name '*.sh' -o \
        -name 'CMakeLists.txt' -o \
        -name build.gradle \
    \) -exec python3 add-license-to-file.py '{}' +