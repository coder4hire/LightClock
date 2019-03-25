package com.lightclockcontrol.gss;

public enum EffectType {
    None(0,"None"),
    Sunrise(1,"Sunrise"),
    DarkRed(2,"Dark Red"),
    Red(3,"Red"),
    DarkGreen(4,"Dark Green"),
    Green(5,"Green"),
    DarkBlue(6,"Dark Blue"),
    Blue(7,"Blue");

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