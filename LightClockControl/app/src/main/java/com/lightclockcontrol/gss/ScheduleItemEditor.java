package com.lightclockcontrol.gss;

import android.content.Intent;
import android.os.Bundle;
import android.os.Parcelable;
import android.support.design.widget.FloatingActionButton;
import android.support.design.widget.Snackbar;
import android.support.v4.app.NavUtils;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.view.Menu;
import android.view.MenuItem;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.CheckBox;
import android.widget.CompoundButton;
import android.widget.EditText;
import android.widget.Spinner;
import android.widget.TimePicker;
import android.widget.Toast;

import java.util.Calendar;
import java.util.Date;
import java.util.TimeZone;

public class ScheduleItemEditor extends AppCompatActivity {

    Spinner spinVisualEffect;
    TimePicker timePicker;
    ScheduleViewAdapter.ScheduleItem editingItem;
    CheckBox chkRnd;
    EditText edtSong;
    Spinner spinFolder;
    EditText edtLightLength;
    EditText edtSoundLength;
    CheckBox[] chkWeekdays = new CheckBox[7];

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_schedule_item_editor);
        Toolbar toolbar = (Toolbar) findViewById(R.id.toolbar);
        setSupportActionBar(toolbar);

        // Getting GUI objects
        spinVisualEffect = (Spinner) findViewById(R.id.spinVisualEffect);
        timePicker = (TimePicker) findViewById(R.id.timePicker);
        spinFolder = (Spinner) findViewById(R.id.spinAudioFolder);
        chkRnd = (CheckBox) findViewById(R.id.chkRnd);
        edtSong = (EditText) findViewById(R.id.txtSong);
        edtLightLength = (EditText) findViewById(R.id.txtLightLength);
        edtSoundLength = (EditText) findViewById(R.id.txtSoundLength);

        chkWeekdays[0] = (CheckBox) findViewById(R.id.chkSun);
        chkWeekdays[1] = (CheckBox) findViewById(R.id.chkMon);
        chkWeekdays[2] = (CheckBox) findViewById(R.id.chkTue);
        chkWeekdays[3] = (CheckBox) findViewById(R.id.chkWed);
        chkWeekdays[4] = (CheckBox) findViewById(R.id.chkThu);
        chkWeekdays[5] = (CheckBox) findViewById(R.id.chkFri);
        chkWeekdays[6] = (CheckBox) findViewById(R.id.chkSat);

        // Filling in spinner values
        ArrayAdapter<String> spinnerAdapterEffects= new ArrayAdapter<String>(App.getContext(), android.R.layout.simple_spinner_dropdown_item);
        for (EffectType t: EffectType.values()){
            spinnerAdapterEffects.add(t.toString());
        }
        spinVisualEffect.setAdapter(spinnerAdapterEffects);

        ArrayAdapter<String> spinnerAdapterFolders= new ArrayAdapter<String>(App.getContext(), android.R.layout.simple_spinner_dropdown_item);
        for (AudioFolder t: AudioFolder.values()){
            spinnerAdapterFolders.add(t.toString());
        }

        spinFolder.setAdapter(spinnerAdapterFolders);

        // Configuring TimePicker
        timePicker.setIs24HourView(true);

        // Loading initial data
        Parcelable parcel = this.getIntent().getExtras().getParcelable("EditingItem");
        if(parcel!=null) {
            editingItem = (ScheduleViewAdapter.ScheduleItem) parcel;

            spinVisualEffect.setSelection(editingItem.effectType.getValue());

            Calendar cal = Calendar.getInstance(TimeZone.getTimeZone("UTC"));
            cal.setTime(editingItem.execTime);
            timePicker.setCurrentHour(cal.get(Calendar.HOUR_OF_DAY));
            timePicker.setCurrentMinute(cal.get(Calendar.MINUTE));

            spinFolder.setSelection(editingItem.folderID);
            if(editingItem.songID!=-1) {
                chkRnd.setChecked(false);
                edtSong.setText(Integer.toString(editingItem.songID));
            }
            else
            {
                chkRnd.setChecked(true);
                edtSong.setText("0");
            }
            edtLightLength.setText(Integer.toString(editingItem.lightEnabledTime));
            edtSoundLength.setText(Integer.toString(editingItem.soundEnabledTime));

            for(int i=0;i<7;i++)
            {
                chkWeekdays[i].setChecked((editingItem.dayOfWeekMask&(1<<i))!=0);
            }
        }

        // Adding event handlers
        chkRnd.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
                edtSong.setEnabled(!isChecked);
            }
        });

//        FloatingActionButton fab = (FloatingActionButton) findViewById(R.id.fab);
//        fab.setOnClickListener(new View.OnClickListener() {
//            @Override
//            public void onClick(View view) {
//                Snackbar.make(view, "Replace with your own action", Snackbar.LENGTH_LONG)
//                        .setAction("Action", null).show();
//            }
//        });
    }

    @Override
    public void onBackPressed()
    {
        setResult(-1,null);
        super.onBackPressed();
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        getMenuInflater().inflate(R.menu.schedule_item_editor_menu, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        switch(item.getItemId())
        {
            case R.id.action_cancel:
                setResult(-1,null);
                finish();
                break;
            case R.id.action_save:
                if(SaveDataToClock()) {
                    setResult(0,null);
                    finish();
                    onBackPressed();
                }
                else{
                    Toast.makeText(this,R.string.toast_cannot_save,Toast.LENGTH_LONG).show();
                }
                break;

        }
        return super.onOptionsItemSelected(item);
    }

    private boolean SaveDataToClock() {
        editingItem.effectType = EffectType.fromValue((int)spinVisualEffect.getSelectedItemId());
        editingItem.execTime.setTime(((long)timePicker.getCurrentHour())*3600l+((long)timePicker.getCurrentMinute())*60l);
        if(chkRnd.isChecked()) {
            editingItem.folderID=-1;
        }
        else {
            editingItem.folderID = (byte) (spinFolder.getSelectedItemId());
        }
        editingItem.songID = (byte)Integer.parseInt(edtSong.getText().toString());
        editingItem.lightEnabledTime = Integer.parseInt(edtLightLength.getText().toString());
        editingItem.soundEnabledTime = Integer.parseInt(edtSoundLength.getText().toString());
        editingItem.dayOfWeekMask=0;
        for(int i=0;i<7;i++)
        {
            editingItem.dayOfWeekMask|=(chkWeekdays[i].isChecked() ? 1<<i : 0);
        }

        return BTInterface.GetInstance().SendScheduleItemUpdate(editingItem);

/*        editingItem.effectType = EffectType.fromValue((int)spinVisualEffect.getSelectedItemId());
        editingItem.execTime.setTime(((long)timePicker.getCurrentHour())*3600000l+((long)timePicker.getCurrentMinute())*60000l);

        Intent intent = new Intent();
        intent.putExtra("ResultItem",editingItem);
        setResult(1,intent);*/
    }
}
