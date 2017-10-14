package com.ezrasavard.suninface;

import java.util.concurrent.TimeUnit;

public class HeadboardCommand {

    static final int DEFAULT_FAST_FADE_MILLIS = (int) TimeUnit.SECONDS.toMillis(1);
    static final int DEFAULT_SUNRISE_FADE_MILLIS = (int) TimeUnit.MINUTES.toMillis(15);

    public static String lightsOn() {
        return lightsOn(DEFAULT_FAST_FADE_MILLIS);
    }

    public static String lightsOn(int millisFade) {
        return String.format("lumos %d;", millisFade);
    }

    public static String lightsOff() {
        return lightsOff(DEFAULT_FAST_FADE_MILLIS);
    }

    public static String lightsOff(int millisFade) {
        return String.format("nox %d;", millisFade);
    }

    public static String sunrise() {
        return sunrise(DEFAULT_SUNRISE_FADE_MILLIS);
    }

    public static String sunrise(int millisFade) {
        return String.format("sunrise %d;", millisFade);
    }

    public static String setRGB(int r, int g, int b) {
        return setRGB(r, g, b, DEFAULT_FAST_FADE_MILLIS);
    }

    public static String setRGB(int r, int g, int b, int millisFade) {
        return String.format("set %d %d %d %d;", r, g, b, millisFade);
    }
}
