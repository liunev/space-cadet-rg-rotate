package com.fexed.spacecadetpinball;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import java.util.ArrayList;
import java.util.List;

/** High scores kept on the device only (replaces the upstream online leaderboard). */
public final class LocalScores {
    static final int MAX_ENTRIES = 20;

    static final class Entry {
        final String name;
        final int score;
        final long time; // epoch millis, 0 = unknown
        final boolean cheats;

        Entry(String name, int score, long time, boolean cheats) {
            this.name = name;
            this.score = score;
            this.time = time;
            this.cheats = cheats;
        }
    }

    private LocalScores() {
    }

    static List<Entry> load() {
        List<Entry> entries = new ArrayList<>();
        try {
            JSONArray array = new JSONArray(PrefsHelper.getLocalScores());
            for (int i = 0; i < array.length(); i++) {
                JSONObject o = array.getJSONObject(i);
                entries.add(new Entry(o.optString("name"), o.optInt("score"), o.optLong("time"), o.optBoolean("cheats")));
            }
        } catch (JSONException ignored) {
        }

        // Carry over the best score from before the local list existed.
        int best = PrefsHelper.getHighScore();
        if (entries.isEmpty() && best > 0) {
            entries.add(new Entry(PrefsHelper.getUsername("Player 1"), best, 0, false));
            save(entries);
        }
        return entries;
    }

    /** Records a finished game. Returns true when it is a new personal best (cheat-free games only). */
    static boolean add(int score) {
        if (score <= 0) return false;
        boolean cheats = PrefsHelper.getCheatsUsed();
        List<Entry> entries = load();
        Entry entry = new Entry(PrefsHelper.getUsername("Player 1"), score, System.currentTimeMillis(), cheats);

        int index = 0;
        while (index < entries.size() && entries.get(index).score >= score) index++;
        if (index < MAX_ENTRIES) {
            entries.add(index, entry);
            while (entries.size() > MAX_ENTRIES) entries.remove(entries.size() - 1);
            save(entries);
        }

        if (!cheats && score > PrefsHelper.getHighScore()) {
            PrefsHelper.setHighScore(score);
            return true;
        }
        return false;
    }

    static void clear() {
        PrefsHelper.setLocalScores("[]");
        PrefsHelper.setHighScore(0);
    }

    private static void save(List<Entry> entries) {
        JSONArray array = new JSONArray();
        try {
            for (Entry e : entries) {
                JSONObject o = new JSONObject();
                o.put("name", e.name);
                o.put("score", e.score);
                o.put("time", e.time);
                o.put("cheats", e.cheats);
                array.put(o);
            }
        } catch (JSONException ignored) {
        }
        PrefsHelper.setLocalScores(array.toString());
    }
}
