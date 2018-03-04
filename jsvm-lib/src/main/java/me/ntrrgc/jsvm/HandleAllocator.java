/* Copyright (c) the JSVM authors
 * https://github.com/ntrrgc/jsvm
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

package me.ntrrgc.jsvm;

import org.jetbrains.annotations.NotNull;

import java.util.ArrayList;
import java.util.Stack;

/**
 * Created by ntrrgc on 3/23/17.
 */

/* package */ final class HandleAllocator<T> {
    private ArrayList<T> items = new ArrayList<>();
    private Stack<Integer> freeList = new Stack<>();
    private int nextHandle = 0;

    public int allocate(@NotNull T item) {
        if (freeList.isEmpty()) {
            if (items.size() != nextHandle) throw new AssertionError();
            items.add(item);
            return nextHandle++;
        } else {
            int handle = freeList.pop();
            if (items.get(handle) != null) throw new AssertionError();
            items.set(handle, item);
            return handle;
        }
    }

    @NotNull
    public T get(int handle) {
        return items.get(handle);
    }

    public void free(int handle) {
        if (items.get(handle) == null) throw new AssertionError();
        items.set(handle, null);
        freeList.push(handle);
    }

    /**
     * Used only in unit tests: returns how many non-null handles are there in `items`.
     */
    public int countAliveHandles() {
        return nextHandle - freeList.size();
    }
}
