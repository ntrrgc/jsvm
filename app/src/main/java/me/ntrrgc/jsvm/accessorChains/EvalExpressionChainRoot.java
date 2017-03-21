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
