package com.lightclockcontrol.gss;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.support.v4.app.Fragment;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.TextView;
import android.widget.Toast;

import java.text.DateFormat;
import java.text.SimpleDateFormat;
import java.util.Calendar;
import java.util.Date;
import java.util.TimeZone;
import java.util.Timer;
import java.util.TimerTask;

import app.akexorcist.bluetotohspp.library.BluetoothState;
import app.akexorcist.bluetotohspp.library.DeviceList;


/**
 * A simple {@link Fragment} subclass.
 * Use the {@link SettingsFragment#newInstance} factory method to
 * create an instance of this fragment.
 */
public class SettingsFragment extends Fragment {

    Button btnSync;
    TextView txtClockTime;
    TextView txtConnectedDeviceName;
    Timer timer = null;

    public SettingsFragment() {
        // Required empty public constructor
    }

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        // Inflate the layout for this fragment
        View view = inflater.inflate(R.layout.fragment_settings, container, false);

        Button btnConnect = (Button)view.findViewById(R.id.btnConnect);
        btnSync = (Button) view.findViewById(R.id.btnSyncClock);
        txtClockTime = (TextView) view.findViewById(R.id.txtClockTime);
        txtConnectedDeviceName = (TextView)view.findViewById(R.id.txtConnectedDeviceName);

        txtConnectedDeviceName.setText(BTInterface.GetInstance().GetConnectedDeviceName());

        btnConnect.setOnClickListener(new View.OnClickListener(){
            public void onClick(View v){
                if(BTInterface.GetInstance().GetSPP().getServiceState() == BluetoothState.STATE_CONNECTED) {
                    BTInterface.GetInstance().GetSPP().disconnect();
                } else {
                    Intent intent = new Intent(getActivity().getBaseContext(), DeviceList.class);
                    startActivityForResult(intent, BluetoothState.REQUEST_CONNECT_DEVICE);
                }
            }
        });

        btnSync.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Calendar cal = Calendar.getInstance();
                int localTime = (int)((cal.getTimeInMillis()+cal.get(Calendar.ZONE_OFFSET)+cal.get(Calendar.DST_OFFSET))/1000);
                if(!BTInterface.GetInstance().SendSetTime(localTime))
                {
                    Toast.makeText(getActivity(),R.string.toast_cannot_save,Toast.LENGTH_LONG).show();
                }
            }
        });

        return view;
    }

    @Override
    public void onPause()
    {
        super.onPause();
        if(timer!=null) {
            timer.cancel();
            timer = null;
        }
    }

    @Override
    public void onResume()
    {
        super.onResume();
        // Setting up timer
        if(timer==null)
        {
            timer= new Timer();
            timer.scheduleAtFixedRate(new TimerTask() {
                @Override
                public void run() {
                    BTInterface.GetInstance().SendGetTime();
                }
            },0,1000);
        }
    }

    public void onActivityResult(int requestCode, int resultCode, Intent data) {
        if(requestCode == BluetoothState.REQUEST_CONNECT_DEVICE) {
            if(resultCode == Activity.RESULT_OK)
                BTInterface.GetInstance().GetSPP().connect(data);
        } else if(requestCode == BluetoothState.REQUEST_ENABLE_BT) {
            if(resultCode == Activity.RESULT_OK) {
                BTInterface.GetInstance().GetSPP().setupService();
            } else {
                Toast.makeText(getActivity().getBaseContext()
                        , "Bluetooth was not enabled."
                        , Toast.LENGTH_SHORT).show();
            }
        }
    }

    public void ShowClockTime(Date dateTime)
    {
        DateFormat dateFormat = new SimpleDateFormat("HH:mm:ss\ndd.MM.yyyy");
        if(txtClockTime!=null) {
            txtClockTime.setText(dateFormat.format(dateTime));
        }
    }

    public void ShowBTDeviceName(String name)
    {
        if(txtConnectedDeviceName!=null) {
            txtConnectedDeviceName.setText(name);
        }
    }
}
