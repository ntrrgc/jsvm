//
// Created by ntrrgc on 1/15/17.
//

#pragma once

#include <stdexcept>
#include <cassert>
using std::runtime_error;

#include <jni.h>
#include <duktape/duktape.h>

typedef int may_throw;
#define OK 0
#define THREW_EXCEPTION -1

#include <jsvm/javaClasses.h>
#include <jsvm/stashIndices.h>
#include <jsvm/utils.h>