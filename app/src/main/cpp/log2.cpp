//
// Created by ntrrgc on 1/15/17.
//
#include <math.h>

static double quotient = log(2.0);

double slow_log2(double x) {
    return x / quotient;
}