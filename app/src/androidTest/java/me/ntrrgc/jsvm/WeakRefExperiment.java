package me.ntrrgc.jsvm;

import android.support.test.runner.AndroidJUnit4;
import android.util.Log;

import org.junit.Test;
import org.junit.runner.RunWith;

import java.lang.ref.Reference;
import java.lang.ref.ReferenceQueue;
import java.lang.ref.WeakReference;
import java.util.ArrayList;
import java.util.Stack;

import static junit.framework.Assert.assertTrue;

@RunWith(AndroidJUnit4.class)
public class WeakRefExperiment {
    private class Potato {
        int handle;
    }

    private class PotatoWeakReference extends WeakReference<Potato> {
        int potatoHandle;

        PotatoWeakReference(Potato referent, ReferenceQueue<? super Potato> q) {
            super(referent, q);
            potatoHandle = referent.handle;
        }
    }

    @Test
    public void testReferences() throws Exception {
        ReferenceQueue<Potato> queue = new ReferenceQueue<>();
        Stack<Integer> freeList = new Stack<>();
        ArrayList<PotatoWeakReference> references = new ArrayList<>(); // just so references themselves are not collected!
        int handleCounter = 0;

        int nCreated = 0, nDeleted = 0;

        for (int i = 0; i < 1000000; i++) {
            Potato potato = new Potato();
            if (!freeList.isEmpty()) {
                potato.handle = freeList.pop();
            } else {
                potato.handle = handleCounter;
                references.add(null);
                handleCounter++;
            }
            references.set(potato.handle, new PotatoWeakReference(potato, queue));
            nCreated++;


            int nCollected = 0;
            Reference<? extends Potato> ref;
            while ((ref = queue.poll()) != null) {
                assertTrue(ref instanceof PotatoWeakReference);
                int handle = ((PotatoWeakReference)ref).potatoHandle;
                freeList.add(handle);
                references.set(handle, null);

                nCollected++;
                ref.clear();
                nDeleted++;
            }
            if (nCollected > 0) {
                Log.i("NCOL", "collected " + nCollected + ", " + (nCreated - nDeleted) + " remaining");
            }
        }

        assertTrue("Too many live objects: " + (nCreated - nDeleted),
                nCreated - nDeleted < 250000);
    }
}
