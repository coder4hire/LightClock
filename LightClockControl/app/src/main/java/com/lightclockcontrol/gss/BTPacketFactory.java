package com.lightclockcontrol.gss;

import java.io.ByteArrayOutputStream;
import java.sql.Time;
import java.util.ArrayList;
import java.util.List;
import java.util.zip.CRC32;
import java.util.zip.Checksum;

enum PacketTypes {
    NotReady(-2),
    Error(-1),
    Unknown(0),
    ScheduleUpdate(1),
    ScheduleRecv(2);

    private int value;
    private PacketTypes(int value) {
        this.value = value;
    }

    public int getValue() {
        return value;
    }

    public static PacketTypes fromValue(int val)
    {
        for (PacketTypes v:values())
        {
            if(v.value==val)
            {
                return v;
            }
        }
        return PacketTypes.Unknown;
    }
}

public class BTPacketFactory {
    ArrayList<Byte> bytesArray = new ArrayList<Byte>();

    final int headerSize = 12;
    final int crcSize = 4;

    protected void WriteToArray(long data) {
        WriteToArray((int) (data & 0xFFFFFFFF));
        WriteToArray((int) (data >> 32));
    }

    protected void WriteToArray(int data) {
        bytesArray.add((byte) (data & 0xFF));
        bytesArray.add((byte) ((data >> 8) & 0xFF));
        bytesArray.add((byte) ((data >> 16) & 0xFF));
        bytesArray.add((byte) (data >> 24));
    }

    protected void WriteToArray(short data) {
        bytesArray.add((byte) (data & 0xFF));
        bytesArray.add((byte) ((data >> 8) & 0xFF));
    }
    protected void WriteToArray(byte data) {
        bytesArray.add(data);
    }

    protected void WriteToArray(boolean data) {
        bytesArray.add(data ? (byte) 1 : (byte) 0);
    }

    protected int ReadIntFromArray(byte[] arr, int offset)
    {
        return ((int)arr[offset]) | (((int)arr[offset+1])<<8) | (((int)arr[offset+2])<<16) | (((int)arr[offset+3])<<24);
    }

    protected short ReadShortFromArray(byte[] arr, int offset)
    {
        return (short)(((int)arr[offset]) | (((int)arr[offset+1])<<8));
    }


    protected void WriteHeader(int packetID,PacketTypes packetType) {
        bytesArray.clear();
        WriteToArray(0xBEAF115E);// Preamble
        WriteToArray(packetID);
        WriteToArray((short)packetType.getValue());
        WriteToArray((short) 0); // length
    }

    protected byte[] Obj2BytesArray(Object[] objArr)
    {
        byte[] arr = new byte[objArr.length];
        for(int i=0;i<objArr.length;i++)
        {
            arr[i]=(Byte)objArr[i];
        }
        return arr;
    }

    public byte[] CreateScheduleUpdatePacket(int packetID, ScheduleViewAdapter.ScheduleItem item) {
        WriteHeader(packetID,PacketTypes.ScheduleUpdate);
        WriteToArray(item.id); // Schedule item ID
        WriteToArray(item.isEnabled);
        WriteToArray((int) item.execTime.getTime());
        WriteToArray((byte)item.effectType.getValue());
        WriteToArray(item.folderID);
        WriteToArray(item.songID);
        WriteToArray(item.lightEnabledTime);
        WriteToArray(item.soundEnabledTime);
        WriteToArray(item.dayOfWeekMask);
        bytesArray.set(10, (byte) (bytesArray.size() - headerSize)); // Setting size, payload only
        WriteToArray(CalcCRC());
        return Obj2BytesArray(bytesArray.toArray());
    }

    public int ParsePacket(byte[] data,int awaitingID, IOnReceiveAction actionTarget)
    {
        if(data.length<headerSize) {
            return PacketTypes.NotReady.getValue();
        }
        // Check preamble
        if(data[0]!=0x5E || data[1]!=0x11 || data[2]!=0xAF || data[3]!=0xBE)
        {
            return PacketTypes.Error.getValue();
        }

        int packetID = ReadIntFromArray(data,4);

        PacketTypes packetType = PacketTypes.fromValue((int)ReadShortFromArray(data,8));

        int payloadSize = ReadShortFromArray(data,10);
        if(payloadSize+headerSize+crcSize<data.length)
        {
            return PacketTypes.NotReady.getValue();
        }
        int calculatedCRC = CalcCRC(data,payloadSize+headerSize);

        if (calculatedCRC == ReadIntFromArray(data, headerSize + payloadSize)) {

            if(actionTarget!=null) {
                actionTarget.OnAcknowledged(packetID);
            }
            switch (packetType) {
                case ScheduleRecv:
                    if (awaitingID != packetID) {
                        return payloadSize + headerSize + crcSize; // wrong packet ID for this type, skipt the packet
                    }

                    ArrayList<ScheduleViewAdapter.ScheduleItem> items = new ArrayList<ScheduleViewAdapter.ScheduleItem>();

                    int itemSize = 18;
                    int itemsCount = payloadSize / itemSize; // 21 is item size in bytes
                    for(int i=0;i<itemsCount;i++)
                    {
                        int offset = headerSize+itemSize*i;
                        ScheduleViewAdapter.ScheduleItem item = new ScheduleViewAdapter.ScheduleItem(data[offset]);
                        item.isEnabled = data[offset+1]!=0;
                        item.execTime.setTime((long)ReadIntFromArray(data,offset+2));
                        item.effectType.fromValue((int)data[offset+6]);
                        item.folderID = data[offset+7];
                        item.songID = data[offset+8];
                        item.lightEnabledTime = ReadIntFromArray(data,offset+9);
                        item.soundEnabledTime = ReadIntFromArray(data,offset+13);
                        item.dayOfWeekMask = data[offset+17];

                        items.add(item);
                    }

                    if(actionTarget!=null) {
                        actionTarget.OnScheduleUpdate(items);
                    }
                    break;
            }
        }

        return payloadSize + headerSize + crcSize;
    }

    private int CalcCRC() {
        Checksum checksum = new CRC32();

        // update the current checksum with the specified array of bytes
        for (byte val : bytesArray) {
            checksum.update(val);
        }
        // get the current checksum value
        return (int)checksum.getValue();
    }

    private int CalcCRC(byte[] bytes, int len) {
        Checksum checksum = new CRC32();

        // update the current checksum with the specified array of bytes
        for (int i=0;i<bytes.length && i<len; i++) {
            checksum.update(bytes[i]);
        }
        // get the current checksum value
        return (int)checksum.getValue();

    }


    public interface IOnReceiveAction
    {
        void OnScheduleUpdate(List<ScheduleViewAdapter.ScheduleItem> scheduleItems);
        void OnAcknowledged(int packetID);
    }
}

