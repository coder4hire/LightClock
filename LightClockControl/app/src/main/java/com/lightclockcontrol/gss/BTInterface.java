package com.lightclockcontrol.gss;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.Toast;

import java.util.zip.CRC32;
import java.util.zip.Checksum;

import app.akexorcist.bluetoothspp.ListenerActivity;
import app.akexorcist.bluetotohspp.library.BluetoothSPP;
import app.akexorcist.bluetotohspp.library.BluetoothState;
import app.akexorcist.bluetotohspp.library.DeviceList;

public class BTInterface implements BluetoothSPP.OnDataReceivedListener{

    BluetoothSPP bt;

    protected void Begin(Context ctx) {
        bt = new BluetoothSPP(ctx);

        if(!bt.isBluetoothAvailable()) {
            Toast.makeText(App.getContext()
                    , "Bluetooth is not available"
                    , Toast.LENGTH_SHORT).show();
        }

        bt.setBluetoothStateListener(new BluetoothSPP.BluetoothStateListener() {
            public void onServiceStateChanged(int state) {
                if(state == BluetoothState.STATE_CONNECTED)
                    Log.i("Check", "State : Connected");
                else if(state == BluetoothState.STATE_CONNECTING)
                    Log.i("Check", "State : Connecting");
                else if(state == BluetoothState.STATE_LISTEN)
                    Log.i("Check", "State : Listen");
                else if(state == BluetoothState.STATE_NONE)
                    Log.i("Check", "State : None");
            }
        });

        bt.setOnDataReceivedListener(this);

        bt.setBluetoothConnectionListener(new BluetoothSPP.BluetoothConnectionListener() {
            public void onDeviceConnected(String name, String address) {
                Log.i("Check", "Device Connected!!");
                Toast.makeText(App.getContext()
                        , name+ " is connected."
                        , Toast.LENGTH_SHORT).show();

            }

            public void onDeviceDisconnected() {
                Log.i("Check", "Device Disconnected!!");
            }

            public void onDeviceConnectionFailed() {
                Log.i("Check", "Unable to Connected!!");
            }
        });

        bt.setAutoConnectionListener(new BluetoothSPP.AutoConnectionListener() {
            public void onNewConnection(String name, String address) {
                Log.i("Check", "New Connection - " + name + " - " + address);
            }

            public void onAutoConnectionStarted() {
                Log.i("Check", "Auto menu_connection started");
            }
        });

        // Enable Bluetooth
        if(!bt.isBluetoothEnabled()) {
            bt.enable();
        } else {
            if(!bt.isServiceAvailable()) {
                bt.setupService();
                bt.startService(BluetoothState.DEVICE_OTHER);
            }
        }
    }

    public void close() throws Exception {
        bt.stopService();
    }

    @Override
    public void onDataReceived(byte[] data, String message) {

    }

    public boolean SendPacket(Byte[] data)
    {
        return true;
    }
}
