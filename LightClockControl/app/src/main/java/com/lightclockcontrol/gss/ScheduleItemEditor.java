package com.lightclockcontrol.gss;

import android.content.Intent;
import android.os.Bundle;
import android.os.Parcelable;
import android.support.design.widget.FloatingActionButton;
import android.support.design.widget.Snackbar;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.widget.ArrayAdapter;
import android.widget.Spinner;
import android.widget.TimePicker;

import java.util.Date;

public class ScheduleItemEditor extends AppCompatActivity {

    Spinner spinner;
    TimePicker timePicker;
    ScheduleViewAdapter.ScheduleItem editingItem;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_schedule_item_editor);
        Toolbar toolbar = (Toolbar) findViewById(R.id.toolbar);
        setSupportActionBar(toolbar);

        // Filling in spinner values
        ArrayAdapter<String> spinnerAdapter;

        spinnerAdapter= new ArrayAdapter<String>(App.getContext(), android.R.layout.simple_spinner_item);
        for (EffectType t: EffectType.values()){
            spinnerAdapter.add(t.toString());
        }

        spinner = (Spinner) findViewById(R.id.spinLightEffect);
        timePicker = (TimePicker) findViewById(R.id.timePicker);
        spinner.setAdapter(spinnerAdapter);

        // Loading initial data
        Parcelable parcel = this.getIntent().getExtras().getParcelable("EditingItem");
        if(parcel!=null) {
            editingItem = (ScheduleViewAdapter.ScheduleItem) parcel;

            spinner.setSelection(editingItem.effectType.getValue());
            timePicker.setCurrentHour(editingItem.execTime.getHours());
            timePicker.setCurrentMinute(editingItem.execTime.getMinutes());
        }

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
        editingItem.effectType = EffectType.fromValue((int)spinner.getSelectedItemId());
        editingItem.execTime.setTime(((long)timePicker.getCurrentHour())*3600000l+((long)timePicker.getCurrentMinute())*60000l);

        Intent intent = new Intent();
        intent.putExtra("ResultItem",editingItem);
        setResult(1,intent);
        super.onBackPressed();
    }
}
