package com.lightclockcontrol.gss;

public enum EffectType {
    None(0,"None"),
    Sunrise(1,"Sunrise"),
    Red(2,"Red");

    private final int value;
    private final String description;

    private EffectType(int value, String description) {
        this.value = value;
        this.description = description;
    }

    public static EffectType fromValue(int val)
    {
        for (EffectType v:values())
        {
            if(v.value==val)
            {
                return v;
            }
        }
        return None;
    }

    public int getValue(){return value;}
}