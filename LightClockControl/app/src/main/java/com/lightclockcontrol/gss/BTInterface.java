package com.lightclockcontrol.gss;

import android.content.Context;
import android.util.Log;
import android.widget.Toast;

import java.io.ByteArrayOutputStream;
import java.util.List;

import app.akexorcist.bluetotohspp.library.BluetoothSPP;
import app.akexorcist.bluetotohspp.library.BluetoothState;

public class BTInterface implements BluetoothSPP.OnDataReceivedListener, BTPacketFactory.IOnReceiveAction {

    private BluetoothSPP bt;
    private ByteArrayOutputStream buffer = new ByteArrayOutputStream();
    private BTPacketFactory packetFactory = new BTPacketFactory();
    private boolean isWaitingForAck = false;

    private int currentPacketID = 1;
    static private BTInterface instance=null;

    static public BTInterface GetInstance()
    {
        if(instance==null)
        {
           instance = new BTInterface();
        }
        return instance;
    }

    private BTInterface()
    {
        Begin(App.getContext());
    }

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

        bt.autoConnect("LightClock");
    }

    public void close() throws Exception {
        bt.stopService();
    }

    @Override
    public void onDataReceived(byte data) {
        buffer.write(data);
        byte[] bufferedData = buffer.toByteArray();

        // If buffered data does not start from preamble, look for it in the stream
        if(bufferedData.length>=BTPacketFactory.headerSize) {
            if (bufferedData[0] != (byte)0x5E || bufferedData[1] != (byte)0x11 || bufferedData[2] != (byte)0xEF || bufferedData[3] != (byte)0xBE) {
                int i = 0;
                for (i = 0; i < bufferedData.length - 4; i++) {
                    if (bufferedData[i] == (byte)0x5E && bufferedData[i + 1] == (byte)0x11 && bufferedData[i + 2] == (byte)0xEF && bufferedData[i + 3] == (byte)0xBE) {
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

            int bytesToRemove = packetFactory.ParsePacket(bufferedData, currentPacketID, this);

            if(bytesToRemove>0) {
                if (bytesToRemove < bufferedData.length) {
                    byte[] bufferedData2 = bufferedData.clone();
                    buffer.reset();
                    buffer.write(bufferedData2, bytesToRemove, bufferedData2.length - bytesToRemove);
                } else {
                    buffer.reset();
                }
            }
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

    public boolean SendPacket(byte[] data)
    {
        synchronized(this) {
            bt.send( data,false);
            isWaitingForAck=true;
        }
        return true;
    }

    public boolean SendScheduleItemUpdate(ScheduleViewAdapter.ScheduleItem item)
    {
        UpdatePacketID();
        byte[] packet = packetFactory.CreateScheduleUpdatePacket(currentPacketID,item);
        SendPacket(packet);

//        for(int i=0;i<10;i++)
//        {
//            synchronized (this)
//            {
//                if(!isWaitingForAck)
//                {
//                    return true;
//                }
//            }
//
//            try {
//                Thread.sleep(1000);
//            }
//            catch(Exception e)
//            {
//            }
//
//            SendPacket(packet);
//        }

//        isWaitingForAck=false;

        return false;
    }

    @Override
    public void OnScheduleUpdate(ScheduleViewAdapter.ScheduleItem[] scheduleItems) {
        synchronized (this) {
            MainActivity.uiHandler.obtainMessage(MainActivity.MSG_UPDATE_SCHEDULE,scheduleItems).sendToTarget();
        }
    }

    @Override
    public void OnAcknowledged(int packetID) {
        synchronized (this) {
            if(packetID==currentPacketID)
            {
                isWaitingForAck=false;
            }
        }
    }

    public BluetoothSPP GetSPP()
    {
        return bt;
    }
}
