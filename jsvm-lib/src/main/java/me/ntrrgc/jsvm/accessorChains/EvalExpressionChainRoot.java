/* Copyright (c) the JSVM authors
 * https://github.com/ntrrgc/jsvm
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

package me.ntrrgc.jsvm.accessorChains;

import org.jetbrains.annotations.NotNull;
import org.jetbrains.annotations.Nullable;

/**
 * Created by ntrrgc on 3/21/17.
 */

public final class EvalExpressionChainRoot extends AccessorChainRoot {
    @Nullable
    private final String evaluatedExpression;

    public EvalExpressionChainRoot(@NotNull String evaluatedExpression) {
        if (evaluatedExpression.length() < 64) {
            this.evaluatedExpression = evaluatedExpression;
        } else {
            this.evaluatedExpression = null;
        }
    }

    private static boolean isComplexExpression(@NotNull String evaluatedExpression) {
        return evaluatedExpression.contains(";") || evaluatedExpression.contains("\n");
    }

    @Override
    public String getFullPath() {
        if (evaluatedExpression != null && !isComplexExpression(evaluatedExpression)) {
            return evaluatedExpression;
        } else {
            return "[eval expression]";
        }
    }
}
