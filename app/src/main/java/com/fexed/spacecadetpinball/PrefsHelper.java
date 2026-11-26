package com.fexed.spacecadetpinball;

import android.content.SharedPreferences;

public class PrefsHelper {
    static private SharedPreferences prefs;

    private static final String KEY_USERNAME = "username";
    private static final String KEY_HIGHSCORE = "highscore";
    private static final String KEY_CHEATHIGHSCORE = "cheathighscore";
    private static final String KEY_CHEATSUSED = "cheatsused";
    private static final String KEY_VOLUME = "volume";
    private static final String KEY_MUSIC = "music";
    private static final String KEY_FULLSCREENPLUNGER = "fullscreenplunger";
    private static final String KEY_PLUNGERPOPUP = "plungerPopup";
    private static final String KEY_REMAININGBALLS = "remainingballs";
    private static final String KEY_SHOULDSHOWBOTTOMPLUNGER = "shouldshowbottomplunger";
    private static final String KEY_TILTBUTTONS = "tiltbuttons";
    private static final String KEY_CUSTOMFONTS = "customfonts";
    private static final String KEY_CAMERAZOOM = "camerazoom";
    private static final String KEY_GBACONTROLS = "gbacontrols";
    private static final String KEY_LOCALSCORES = "localscores";

    public static SharedPreferences getPrefs() {
        return prefs;
    }

    public static void setPrefs(SharedPreferences prefs) {
        PrefsHelper.prefs = prefs;
    }

    public static boolean getCheatsUsed() {
        return prefs.getBoolean(KEY_CHEATSUSED, false);
    }

    public static void setCheatsUsed(boolean value) {
        prefs.edit().putBoolean(KEY_CHEATSUSED, value).apply();
    }

    public static int getVolume() {
        return prefs.getInt(KEY_VOLUME, 100);
    }

    public static void setVolume(int value) {
        prefs.edit().putInt(KEY_VOLUME, value).apply();
    }

    public static String getUsername(String defaultvalue) {
        return prefs.getString(KEY_USERNAME, defaultvalue);
    }

    public static void setUsername(String value) {
        prefs.edit().putString(KEY_USERNAME, value).apply();
    }

    public static boolean getFullScreenPlunger() {
        return prefs.getBoolean(KEY_FULLSCREENPLUNGER, true);
    }

    public static void setFullScreenPlunger(boolean value) {
        prefs.edit().putBoolean(KEY_FULLSCREENPLUNGER, value).apply();
    }

    public static boolean getPlungerPopup() {
        return prefs.getBoolean(KEY_PLUNGERPOPUP, true);
    }

    public static void setPlungerPopup(boolean value) {
        prefs.edit().putBoolean(KEY_PLUNGERPOPUP, value).apply();
    }

    public static boolean getRemainingBalls() {
        return prefs.getBoolean(KEY_REMAININGBALLS, false);
    }

    public static void setRemainingBalls(boolean value) {
        prefs.edit().putBoolean(KEY_REMAININGBALLS, value).apply();
    }

    public static boolean getShouldShowBottomPlunger() {
        return prefs.getBoolean(KEY_SHOULDSHOWBOTTOMPLUNGER, false);
    }

    public static void setShouldShowBottomPlunger(boolean value) {
        prefs.edit().putBoolean(KEY_SHOULDSHOWBOTTOMPLUNGER, value).apply();
    }

    public static boolean getTiltButtons() {
        return prefs.getBoolean(KEY_TILTBUTTONS, true);
    }

    public static void setTiltButtons(boolean value) {
        prefs.edit().putBoolean(KEY_TILTBUTTONS, value).apply();
    }

    public static boolean getCustomFonts() {
        return prefs.getBoolean(KEY_CUSTOMFONTS, true);
    }

    public static void setCustomFonts(boolean value) {
        prefs.edit().putBoolean(KEY_CUSTOMFONTS, value).apply();
    }

    public static int getHighScore() {
        return prefs.getInt(KEY_HIGHSCORE, 0);
    }

    public static void setHighScore(int value) {
        prefs.edit().putInt(KEY_HIGHSCORE, value).apply();
    }


    public static void setMusic(boolean value) {
        prefs.edit().putBoolean(KEY_MUSIC, value).apply();
    }

    public static boolean getMusic() {
        return prefs.getBoolean(KEY_MUSIC, true);
    }

    /** Camera zoom in percent: 100 = whole table (camera off), up to 300. */
    public static int getCameraZoom() {
        return prefs.getInt(KEY_CAMERAZOOM, 110);
    }

    public static void setCameraZoom(int value) {
        prefs.edit().putInt(KEY_CAMERAZOOM, value).apply();
    }

    /** Pokemon Pinball (GBA) controller layout: D-pad Left/A flippers, B launch, L/R nudge. */
    public static boolean getGbaControls() {
        return prefs.getBoolean(KEY_GBACONTROLS, true);
    }

    public static void setGbaControls(boolean value) {
        prefs.edit().putBoolean(KEY_GBACONTROLS, value).apply();
    }

    /** Local high score table as a JSON array, see LocalScores. */
    public static String getLocalScores() {
        return prefs.getString(KEY_LOCALSCORES, "[]");
    }

    public static void setLocalScores(String json) {
        prefs.edit().putString(KEY_LOCALSCORES, json).apply();
    }
}
