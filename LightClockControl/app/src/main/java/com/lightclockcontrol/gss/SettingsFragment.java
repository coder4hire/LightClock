package com.lightclockcontrol.gss;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.support.v4.app.Fragment;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.CheckBox;
import android.widget.ImageButton;
import android.widget.SeekBar;
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
    Button btnSave;
    Button btnReset;
    ImageButton btnGetSensorsInfo;
    TextView txtClockTime;
    TextView txtConnectedDeviceName;
    TextView txtSensorsInfo;
    TextView txtBacklightThreshold;
    Timer timer = null;
    boolean isPlayBtnShown=true;
    SeekBar seekVolume;
    CheckBox chkBacklightDisablesRGB;
    CheckBox chkFrontLightStopsAlarm;
    CheckBox chkVisualConfirmation;

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
        txtSensorsInfo = (TextView)view.findViewById(R.id.txtSensorsInfo);
        txtBacklightThreshold = (TextView)view.findViewById(R.id.txtBackThresh);
        btnGetSensorsInfo = (ImageButton) view.findViewById(R.id.btnGetSensorsInfo);
        btnSave = (Button) view.findViewById(R.id.btnSave);
        btnReset = (Button) view.findViewById(R.id.btnReset);
        chkBacklightDisablesRGB = (CheckBox)view.findViewById(R.id.chkBacklightDisablesRGB);
        chkFrontLightStopsAlarm = (CheckBox)view.findViewById(R.id.chkFrontLightStopsAlarm);
        chkVisualConfirmation = (CheckBox)view.findViewById(R.id.chkVisualConfirmation);

        final ImageButton btnPlayStop = (ImageButton)view.findViewById(R.id.btnPlayStop);
        seekVolume = (SeekBar)view.findViewById(R.id.seekVolume);

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
                BTInterface.GetInstance().SendSetTime(localTime);
            }
        });

        isPlayBtnShown = true;
        btnPlayStop.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if(isPlayBtnShown)
                {
                    BTInterface.GetInstance().SendPlayMusicPacket();
                }
                else
                {
                    BTInterface.GetInstance().SendStopMusicPacket();
                }
                isPlayBtnShown=!isPlayBtnShown;
                int id = getResources().getIdentifier(
                        isPlayBtnShown? "@android:drawable/ic_media_play" : "@android:drawable/ic_media_stop",
                        null, null);
                btnPlayStop.setImageResource(id);
            }
        });

        btnGetSensorsInfo.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                txtSensorsInfo.setText("---Reading sensors---");
                BTInterface.GetInstance().SendGetSensorsInfoPacket();
            }
        });

        seekVolume.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {

            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {

            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {
                BTInterface.GetInstance().SendSetVolumePacket(seekBar.getProgress());
            }
        });

        btnSave.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                BoardConfig cfg = new BoardConfig();
                cfg.Volume = (byte)seekVolume.getProgress();
                try {
                    cfg.LightThresholdBack = Short.parseShort(txtBacklightThreshold.getText().toString());
                }
                catch(Exception e)
                {
                    cfg.LightThresholdBack = 800;
                }
                cfg.DoesBackligthDisableRGB = chkBacklightDisablesRGB.isChecked();
                cfg.IsStopLightEnabled = chkFrontLightStopsAlarm.isChecked();
                cfg.AreVisualConfirmationsEnabled = chkVisualConfirmation.isChecked();

                if(!BTInterface.GetInstance().SendSetConfigPacket(cfg))
                {
                    Toast.makeText(getActivity(),"Cannot save configuration. Please check connection.",Toast.LENGTH_LONG).show();
                }
                else
                {
                    Toast.makeText(getActivity(),"Configuration is saved.",Toast.LENGTH_SHORT).show();
                }
            }
        });

        btnReset.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if(!BTInterface.GetInstance().SendGetConfigPacket())
                {
                    Toast.makeText(getActivity(),"Cannot load configuration. Please check connection.",Toast.LENGTH_LONG).show();
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

        if(!BTInterface.GetInstance().SendGetConfigPacket())
        {
            Toast.makeText(getActivity(),"Cannot load configuration. Please check connection.",Toast.LENGTH_LONG).show();
        }

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

    public void ShowSensorsInfo(String info)
    {
        txtSensorsInfo.setText(info);
    }


    public void ShowBTDeviceName(String name)
    {
        if(txtConnectedDeviceName!=null) {
            txtConnectedDeviceName.setText(name);
        }
    }

    public void UpdateConfig(BoardConfig cfg) {
        seekVolume.setProgress(cfg.Volume <0 ? 256+cfg.Volume : cfg.Volume);
        txtBacklightThreshold.setText(Short.toString(cfg.LightThresholdBack));
        chkBacklightDisablesRGB.setChecked(cfg.DoesBackligthDisableRGB);
        chkFrontLightStopsAlarm.setChecked(cfg.IsStopLightEnabled);
        chkVisualConfirmation.setChecked(cfg.AreVisualConfirmationsEnabled);
    }
}
