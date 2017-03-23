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