package com.ezrasavard.suninface;

import java.util.concurrent.TimeUnit;

public class HeadboardCommand {

    static final int DEFAULT_FAST_FADE = 1;
    static final int DEFAULT_SUNRISE_FADE = (int) TimeUnit.MINUTES.toSeconds(15);

    public static String lightsOn() {
        return lightsOn(DEFAULT_FAST_FADE);
    }

    public static String lightsOn(int fadeSeconds) {
        return String.format("lumos %d;", fadeSeconds);
    }

    public static String lightsOff() {
        return lightsOff(DEFAULT_FAST_FADE);
    }

    public static String lightsOff(int fadeSeconds) {
        return String.format("nox %d;", fadeSeconds);
    }

    public static String sunrise() {
        return sunrise(DEFAULT_SUNRISE_FADE);
    }

    public static String sunrise(int fadeSeconds) {
        return String.format("sunrise %d;", fadeSeconds);
    }

    public static String setRGB(int r, int g, int b) {
        return setRGB(r, g, b, DEFAULT_FAST_FADE);
    }

    public static String setRGB(int r, int g, int b, int fadeSeconds) {
        return String.format("set %d %d %d %d;", r, g, b, fadeSeconds);
    }

    public static String setParty() {
        return "party;";
    }
}
