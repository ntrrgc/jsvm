/* Copyright (c) the JSVM authors
 * https://github.com/ntrrgc/jsvm
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

//
// Created by ntrrgc on 1/15/17.
//

#pragma once

// The global stash is used to store internal information.
// It's indexed as an array, where each entry is used for a different purpose.

// Used by the ObjectBook
#define GLOBAL_STASH_INDEX_OBJECT_BOOK 0

// Used to create trampoline functions in Duktape that invoke JSCallable
#define GLOBAL_STASH_INDEX_TRAMPOLINE_FACTORY 1

// Used to generate the finalizer callbacks that are attached to Java functions exposed in JS
#define GLOBAL_STASH_INDEX_FINALIZE_FACTORY 2