package com.lightclockcontrol.gss;

import android.content.Intent;
import android.os.Bundle;
import android.support.annotation.NonNull;
import android.support.design.widget.BottomNavigationView;
import android.support.v4.app.Fragment;
import android.support.v4.app.FragmentManager;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.LinearLayoutManager;
import android.support.v7.widget.RecyclerView;
import android.view.MenuItem;
import android.widget.TextView;

import com.lightclockcontrol.gss.dummy.DummyContent;

import java.util.ArrayList;
import app.akexorcist.bluetoothspp.ListenerActivity;

public class MainActivity extends AppCompatActivity {

    private Fragment alarmsFragment = new AlarmsFragment();
    private Fragment manualFragment = new ManualFragment();

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
                    Intent i = new Intent(getBaseContext(), ListenerActivity.class);
                    startActivity(i);
                    return true;
            }
            return false;
        }
    };

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        getSupportFragmentManager().beginTransaction().replace(R.id.fragmentContent,alarmsFragment).commit();

        BottomNavigationView navigation = (BottomNavigationView) findViewById(R.id.navigation);
        navigation.setOnNavigationItemSelectedListener(mOnNavigationItemSelectedListener);
    }
}
