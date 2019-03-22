package com.lightclockcontrol.gss;

import android.os.Trace;
import android.util.Log;

import java.util.ArrayList;
import java.util.Date;
import java.util.zip.CRC32;
import java.util.zip.Checksum;

enum PacketTypes {
    Unknown(0),
    ScheduleUpdate(1),
    GetSchedule(2),
    SetTime(3),
    GetTime(4),
    StopAlarm(5),
    EnableScheduleItem(6),
    SetVolume(7),
    GetConfig(8),
    SaveConfig(9),
    SetManualColor(0xA),
    GetSensorsInfo(0xB),
    PlayMusic(0xC),
    StopMusic(0xD),

    SimpleAck(0x40),
    ScheduleRecv(0x41),
    TimeRecv(0x44),
    ConfigRecv(0x48),
    SensorsInfoRecv(0x4B);

    private int value;
    PacketTypes(int value) {
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

    static public final int HeaderSize = 12;
    static public final int CRCSize = 4;
    private int currentPacketID = 1;

    private void UpdatePacketID() {
        currentPacketID++;
        if (currentPacketID == Integer.MAX_VALUE) {
            currentPacketID = 1; // 0 is reserved for broadcasts
        }
    }

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

    static public long ReadLongFromArray(byte[] arr, int offset)
    {
        return (arr[offset] & 0xFFl) | ((arr[offset + 1] << 8) & 0xFF00l) | ((arr[offset + 2] << 16) & 0xFF0000l) | ((arr[offset + 3] << 24) & 0xFF000000l) |
                ((arr[offset + 4] << 32) & 0xFF00000000l) | ((arr[offset + 5] << 40) & 0xFF0000000000l) | ((arr[offset + 6] << 48) & 0xFF000000000000l) | ((arr[offset + 7] << 56) & 0xFF00000000000000l);
    }

    static public int ReadIntFromArray(byte[] arr, int offset)
    {
        return (arr[offset]&0xFF) | ((arr[offset+1]<<8)&0xFF00) | ((arr[offset+2]<<16)&0xFF0000) | (( arr[offset+3]<<24)&0xFF000000);
    }

    static public short ReadShortFromArray(byte[] arr, int offset)
    {
        return (short)((arr[offset]&0xFF) | ((arr[offset+1]<<8)&0xFF00));
    }

    protected void WriteHeader(PacketTypes packetType) {
        bytesArray.clear();
        UpdatePacketID();
        WriteToArray(0xBEEF115E);// Preamble
        WriteToArray(currentPacketID);
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

    public byte[] CreateScheduleUpdatePacket(ScheduleViewAdapter.ScheduleItem item) {
        WriteHeader(PacketTypes.ScheduleUpdate);
        WriteToArray(item.id); // Schedule item ID
        WriteToArray(item.isEnabled);
        WriteToArray((int) item.execTime.getTime());
        WriteToArray((byte)item.effectType.getValue());
        WriteToArray(item.folderID);
        WriteToArray(item.songID);
        WriteToArray(item.lightEnabledTime);
        WriteToArray(item.soundEnabledTime);
        WriteToArray(item.dayOfWeekMask);
        bytesArray.set(10, (byte) (bytesArray.size() - HeaderSize)); // Setting size, payload only
        int crc = CalcCRC();
        WriteToArray(crc);
        return Obj2BytesArray(bytesArray.toArray());
    }

    public byte[] CreateSimplePacket(PacketTypes type) {
        WriteHeader(type);
        int crc = CalcCRC();
        WriteToArray(crc);
        return Obj2BytesArray(bytesArray.toArray());
    }

    public byte[] CreateSetTimePacket(int localTime) {
        WriteHeader(PacketTypes.SetTime);
        WriteToArray(localTime);
        bytesArray.set(10, (byte) (bytesArray.size() - HeaderSize)); // Setting size, payload only
        int crc = CalcCRC();
        WriteToArray(crc);
        return Obj2BytesArray(bytesArray.toArray());
    }

    public byte[] CreateEnableScheduleItemPacket(byte itemIndex,boolean isEnabled) {
        WriteHeader(PacketTypes.EnableScheduleItem);
        WriteToArray(itemIndex);
        WriteToArray(isEnabled);
        bytesArray.set(10, (byte) (bytesArray.size() - HeaderSize)); // Setting size, payload only
        int crc = CalcCRC();
        WriteToArray(crc);
        return Obj2BytesArray(bytesArray.toArray());
    }

    public byte[] CreateSetManualColorPacket(int rgbw)
    {
        WriteHeader(PacketTypes.SetManualColor);
        WriteToArray(rgbw);
        bytesArray.set(10, (byte) (bytesArray.size() - HeaderSize)); // Setting size, payload only
        int crc = CalcCRC();
        WriteToArray(crc);
        return Obj2BytesArray(bytesArray.toArray());
    }

    public byte[] CreateSetVolumePacket(int volume)
    {
        WriteHeader(PacketTypes.SetVolume);
        WriteToArray((byte)volume);
        bytesArray.set(10, (byte) (bytesArray.size() - HeaderSize)); // Setting size, payload only
        int crc = CalcCRC();
        WriteToArray(crc);
        return Obj2BytesArray(bytesArray.toArray());
    }

    public int ParsePacket(byte[] data,int awaitingID, IOnAckCallback ackCallback)
    {
        if(data.length<HeaderSize) {
            return 0;
        }
        // Check preamble
        if(ReadIntFromArray(data,0)!=(int)0xBEEF115E)
        {
            // If packet with wrong preamble was given at this stage - something has gone wrong, we have to clean up whole buffer
            return data.length;
        }

        int packetID = ReadIntFromArray(data,4);

        PacketTypes packetType = PacketTypes.fromValue((int)ReadShortFromArray(data,8));

        int payloadSize = ReadShortFromArray(data,10);
        if(payloadSize+HeaderSize+CRCSize>data.length)
        {
            return 0;
        }
        int calculatedCRC = CalcCRC(data,payloadSize+HeaderSize);

        if (calculatedCRC == ReadIntFromArray(data, HeaderSize + payloadSize)) {

            if(ackCallback!=null) {
                ackCallback.OnAcknowledged(packetID);
            }
            switch (packetType) {
                case ScheduleRecv:
                    if (awaitingID != packetID) {
                        return payloadSize + HeaderSize + CRCSize; // wrong packet ID for this type, skip the packet
                    }

                    int itemSize = 18;
                    int itemsCount = payloadSize / itemSize; // 21 is item size in bytes
                    ScheduleViewAdapter.ScheduleItem[] items = new ScheduleViewAdapter.ScheduleItem[itemsCount];

                    for(int i=0;i<itemsCount;i++)
                    {
                        int offset = HeaderSize+itemSize*i;
                        ScheduleViewAdapter.ScheduleItem item = new ScheduleViewAdapter.ScheduleItem(data[offset]);
                        item.isEnabled = data[offset+1]!=0;
                        item.execTime.setTime((long)ReadIntFromArray(data,offset+2)*1000l);
                        item.effectType = EffectType.fromValue((int)data[offset+6]);
                        item.folderID = data[offset+7];
                        item.songID = data[offset+8];
                        item.lightEnabledTime = ReadIntFromArray(data,offset+9);
                        item.soundEnabledTime = ReadIntFromArray(data,offset+13);
                        item.dayOfWeekMask = data[offset+17];

                        items[i]=item;
                    }

                    if(MainActivity.uiHandler!=null) {
                        MainActivity.uiHandler.obtainMessage(MainActivity.MSG_UPDATE_SCHEDULE, items).sendToTarget();
                    }
                    break;
                case TimeRecv:
                    if (awaitingID != packetID) {
                        return payloadSize + HeaderSize + CRCSize; // wrong packet ID for this type, skip the packet
                    }
                    int dateTime = ReadIntFromArray(data,HeaderSize);
                    if(MainActivity.uiHandler!=null) {
                        MainActivity.uiHandler.obtainMessage(MainActivity.MSG_UPDATE_CLOCK_TIME, dateTime, 0).sendToTarget();
                    }
                    break;
                case SensorsInfoRecv:
                    if (awaitingID != packetID) {
                        return payloadSize + HeaderSize + CRCSize; // wrong packet ID for this type, skip the packet
                    }
                    short frontSensor = ReadShortFromArray(data,HeaderSize);
                    short backSensor = ReadShortFromArray(data,HeaderSize+2);
                    if(MainActivity.uiHandler!=null) {
                        MainActivity.uiHandler.obtainMessage(MainActivity.MSG_SENSORS_INFO,
                                "Front:"+Short.toString(frontSensor)+ " Back:"+Short.toString(frontSensor)).sendToTarget();
                    }
                    break;
            }
        }

        return payloadSize + HeaderSize + CRCSize;
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

    public interface IOnAckCallback
    {
        void OnAcknowledged(int packetID);
    }
}

