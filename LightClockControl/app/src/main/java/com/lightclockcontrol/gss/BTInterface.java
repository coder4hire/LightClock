package com.lightclockcontrol.gss;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.Toast;

import java.io.ByteArrayOutputStream;
import java.util.List;
import java.util.zip.CRC32;
import java.util.zip.Checksum;

import app.akexorcist.bluetoothspp.ListenerActivity;
import app.akexorcist.bluetotohspp.library.BluetoothSPP;
import app.akexorcist.bluetotohspp.library.BluetoothState;
import app.akexorcist.bluetotohspp.library.DeviceList;

public class BTInterface implements BluetoothSPP.OnDataReceivedListener, BTPacketFactory.IOnReceiveAction {

    BluetoothSPP bt;
    ByteArrayOutputStream buffer = new ByteArrayOutputStream();
    BTPacketFactory packetFactory = new BTPacketFactory();

    int currentPacketID = 1;

    protected void Begin(Context ctx) {
        bt = new BluetoothSPP(ctx);

        if (!bt.isBluetoothAvailable()) {
            Toast.makeText(App.getContext()
                    , "Bluetooth is not available"
                    , Toast.LENGTH_SHORT).show();
        }

        bt.setBluetoothStateListener(new BluetoothSPP.BluetoothStateListener() {
            public void onServiceStateChanged(int state) {
                if (state == BluetoothState.STATE_CONNECTED)
                    Log.i("Check", "State : Connected");
                else if (state == BluetoothState.STATE_CONNECTING)
                    Log.i("Check", "State : Connecting");
                else if (state == BluetoothState.STATE_LISTEN)
                    Log.i("Check", "State : Listen");
                else if (state == BluetoothState.STATE_NONE)
                    Log.i("Check", "State : None");
            }
        });

        bt.setOnDataReceivedListener(this);

        bt.setBluetoothConnectionListener(new BluetoothSPP.BluetoothConnectionListener() {
            public void onDeviceConnected(String name, String address) {
                Log.i("Check", "Device Connected!!");
                Toast.makeText(App.getContext()
                        , name + " is connected."
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
        if (!bt.isBluetoothEnabled()) {
            bt.enable();
        } else {
            if (!bt.isServiceAvailable()) {
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
        try {
            buffer.write(data);
            byte[] bufferedData = buffer.toByteArray();

            // If buffered data does not start from preamble, look for it in the stream
            if (bufferedData[0] != 0x5E || bufferedData[1] != 0x11 || bufferedData[2] != 0xAF || bufferedData[3] != 0xBE) {
                int i = 0;
                for (i = 0; i < bufferedData.length - 4; i++) {
                    if (bufferedData[i] == 0x5E && bufferedData[i + 1] != 0x11 && bufferedData[i + 2] == 0xAF && bufferedData[3] == 0xBE) {
                        byte[] bufferedData2 = bufferedData.clone();
                        buffer.reset();
                        buffer.write(bufferedData2, i, bufferedData2.length - i);
                        break;
                    }
                }
                if (i == bufferedData.length - 4) {
                    buffer.reset();
                    return;
                }
            }

            int bytesToRemove = packetFactory.ParsePacket(bufferedData, currentPacketID,this);

            if (bytesToRemove < bufferedData.length) {
                byte[] bufferedData2 = bufferedData.clone();
                buffer.reset();
                buffer.write(bufferedData2, bytesToRemove, bufferedData2.length - bytesToRemove);
            } else {
                buffer.reset();
            }

        } catch (java.io.IOException ex) {
            buffer.reset();
        }
    }

    private void UpdatePacketID()
    {
        currentPacketID++;
        if(currentPacketID==Integer.MAX_VALUE)
        {
            currentPacketID=1; // 0 is reserved for broadcasts
        }
    }

    public boolean SendPacket(Byte[] data) {
        return true;
    }

    public boolean SendScheduleItemUpdate(ScheduleViewAdapter.ScheduleItem item)
    {
        UpdatePacketID();
        if(SendPacket(packetFactory.CreateScheduleUpdatePacket(currentPacketID,item)))
        {
            return false;
        }

        // TODO: Wait for response, repeat after timeout
        return true;
    }

    @Override
    public void OnScheduleUpdate(List<ScheduleViewAdapter.ScheduleItem> scheduleItems) {

    }
}
