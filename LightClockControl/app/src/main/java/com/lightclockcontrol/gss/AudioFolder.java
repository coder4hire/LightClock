package com.lightclockcontrol.gss;

public enum AudioFolder {
    System(0,"System"),
    Songs1(1,"Songs0"),
    Songs2(2,"Songs1"),
    Songs3(3,"Songs3"),
    Songs4(4,"Songs4");

    private final int value;
    private final String description;

    private AudioFolder(int value, String description) {
        this.value = value;
        this.description = description;
    }

    public static AudioFolder fromValue(int val)
    {
        for (AudioFolder v:values())
        {
            if(v.value==val)
            {
                return v;
            }
        }
        return System;
    }

    public int getValue(){return value;}
}