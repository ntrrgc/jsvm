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

#include <jni.h>
#include "duktape/duktape.h"

#include <stdexcept>
#include <memory>
#include <utility>
#include <string>

#include "jsvm/exceptions.h"
#include "jsvm/javaClasses.h"
#include "jsvm/stashIndices.h"
#include "jsvm/utils.h"