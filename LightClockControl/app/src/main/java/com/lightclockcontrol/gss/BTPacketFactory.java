package com.lightclockcontrol.gss;

import java.io.ByteArrayOutputStream;
import java.sql.Time;
import java.util.ArrayList;
import java.util.zip.CRC32;
import java.util.zip.Checksum;

enum PacketsType {
    ScheduleUpdate,
    ScheduleRecv,
}

public class BTPacketFactory {
    ArrayList<Byte> bytesArray = new ArrayList<Byte>();

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

    protected void WriteHeader(int packetID) {
        bytesArray.clear();
        WriteToArray(0xBEAF115E);// Preamble
        WriteToArray(packetID);
    }

    public Byte[] CreatePacket(int packetID, ScheduleViewAdapter.ScheduleItem item) {
        WriteHeader(packetID);
        WriteToArray((int) 0); // length
        WriteToArray(item.id); // Schedule item ID
        WriteToArray(item.isEnabled);
        WriteToArray((int) item.execTime.getTime());
        WriteToArray(item.effectType.getValue());
        WriteToArray(item.folderID);
        WriteToArray(item.songID);
        WriteToArray(item.effectEnabledTime);
        WriteToArray(item.musicEnabledTime);
        WriteToArray(item.dayOfWeekMask);
        bytesArray.set(8, (byte) (bytesArray.size() - 12)); // Setting size, payload only
        WriteToArray(CalcCRC());
        return (Byte[]) bytesArray.toArray();
    }

    public boolean ParsePacket(byte[] data) {
        return true;
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

}

