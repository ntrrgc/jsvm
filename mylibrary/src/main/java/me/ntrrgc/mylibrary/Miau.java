package me.ntrrgc.mylibrary;

/**
 * Created by ntrrgc on 5/13/17.
 */

public class Miau {
    static {
        System.loadLibrary("mylibrary");
    }

    public static native int meow();
}
