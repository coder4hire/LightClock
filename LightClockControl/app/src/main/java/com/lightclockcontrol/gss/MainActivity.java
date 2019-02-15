package com.lightclockcontrol.gss;

import android.annotation.SuppressLint;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.support.annotation.NonNull;
import android.support.design.widget.BottomNavigationView;
import android.support.v4.app.Fragment;
import android.support.v4.app.FragmentManager;
import android.support.v7.app.AppCompatActivity;
import android.view.MenuItem;

public class MainActivity extends AppCompatActivity {

    private AlarmsFragment alarmsFragment = new AlarmsFragment();
    private ManualFragment manualFragment = new ManualFragment();
    private SettingsFragment settingsFragment = new SettingsFragment();

    public static final int MSG_UPDATE_SCHEDULE = 100;

    static public Handler uiHandler;

    private BottomNavigationView.OnNavigationItemSelectedListener mOnNavigationItemSelectedListener
            = new BottomNavigationView.OnNavigationItemSelectedListener() {

        @Override
        public boolean onNavigationItemSelected(@NonNull MenuItem item) {
            FragmentManager mgr = getSupportFragmentManager();
            switch (item.getItemId()) {
                case R.id.navigation_alarms:
                    mgr.beginTransaction().replace(R.id.fragmentContent,alarmsFragment).commit();
                    return true;
                case R.id.navigation_manual:
                    mgr.beginTransaction().replace(R.id.fragmentContent,manualFragment).commit();
                    return true;
                case R.id.navigation_settings:
                    mgr.beginTransaction().replace(R.id.fragmentContent,settingsFragment).commit();
                    return true;
            }
            return false;
        }
    };

    @SuppressLint("HandlerLeak")
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        getSupportFragmentManager().beginTransaction().replace(R.id.fragmentContent,alarmsFragment).commit();

        BottomNavigationView navigation = (BottomNavigationView) findViewById(R.id.navigation);
        navigation.setOnNavigationItemSelectedListener(mOnNavigationItemSelectedListener);
    }

    @SuppressLint("HandlerLeak")
    @Override
    protected void onResume() {
        super.onResume();

        uiHandler = new Handler() {
            @Override
            public void handleMessage(Message msg) {
                super.handleMessage(msg);
                if (msg.what == MSG_UPDATE_SCHEDULE) {
                    alarmsFragment.UpdateSchedule((ScheduleViewAdapter.ScheduleItem[])msg.obj);
                }
            }
        };
    }
}
