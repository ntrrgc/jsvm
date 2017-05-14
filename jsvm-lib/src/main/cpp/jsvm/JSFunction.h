//
// Created by ntrrgc on 1/25/17.
//

#ifndef JSVM_JSFUNCTION_H
#define JSVM_JSFUNCTION_H

#include <stdafx.h>
#include <jsvm/JSObject.h>

namespace jsvm {

    class _JSFunction : public _JSObject {
    };

    typedef _JSFunction *JSFunction;

    JSFunction JSFunction_createFromStack(JNIEnv *env, JSVM jsVM, int stackPosition);

}

#endif //JSVM_JSFUNCTION_H
