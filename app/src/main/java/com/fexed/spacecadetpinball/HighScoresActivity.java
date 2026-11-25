package com.fexed.spacecadetpinball;

import android.os.Bundle;
import android.text.format.DateFormat;
import android.view.LayoutInflater;
import android.view.View;

import androidx.appcompat.app.AlertDialog;
import androidx.appcompat.app.AppCompatActivity;

import com.fexed.spacecadetpinball.databinding.ActivityHighscoresBinding;
import com.fexed.spacecadetpinball.databinding.HighscoreItemBinding;

import java.text.NumberFormat;
import java.util.Date;
import java.util.List;

/** Local high score table, stored on the device by {@link LocalScores}. */
public class HighScoresActivity extends AppCompatActivity {
    private ActivityHighscoresBinding mBinding;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        mBinding = ActivityHighscoresBinding.inflate(getLayoutInflater());
        setContentView(mBinding.getRoot());
        setTitle(R.string.highscores_title);

        mBinding.clearbtn.setOnClickListener(v -> new AlertDialog.Builder(this)
                .setTitle(R.string.highscores_clear)
                .setMessage(R.string.highscores_clear_confirm)
                .setPositiveButton(android.R.string.ok, (d, w) -> {
                    LocalScores.clear();
                    showScores();
                })
                .setNegativeButton(android.R.string.cancel, null)
                .show());

        showScores();
    }

    private void showScores() {
        List<LocalScores.Entry> entries = LocalScores.load();
        mBinding.list.removeAllViews();
        mBinding.emptytxt.setVisibility(entries.isEmpty() ? View.VISIBLE : View.GONE);
        mBinding.clearbtn.setEnabled(!entries.isEmpty());

        LayoutInflater inflater = getLayoutInflater();
        NumberFormat fmt = NumberFormat.getIntegerInstance();
        java.text.DateFormat dateFmt = DateFormat.getDateFormat(this);
        for (int i = 0; i < entries.size(); i++) {
            LocalScores.Entry e = entries.get(i);
            HighscoreItemBinding item = HighscoreItemBinding.inflate(inflater, mBinding.list, true);
            item.ranktxt.setText(getString(R.string.highscores_rank, i + 1));
            item.nametxt.setText(e.name);
            item.scoretxt.setText(fmt.format(e.score));
            String details = e.time > 0 ? dateFmt.format(new Date(e.time)) : "";
            if (e.cheats) details = details.isEmpty() ? getString(R.string.highscores_cheats)
                    : details + " · " + getString(R.string.highscores_cheats);
            item.detailtxt.setText(details);
        }
    }
}
