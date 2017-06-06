package me.ntrrgc.jsvm;

import org.jetbrains.annotations.NotNull;

import java.util.ArrayList;
import java.util.Stack;

/**
 * Created by ntrrgc on 3/23/17.
 */

/* package */ class HandleAllocator<T> {
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
}
