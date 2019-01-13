package com.lightclockcontrol.gss;

import android.app.Application;
import android.content.Context;

public class App extends Application {
    private static Application myApplication;

    public static Application getApplication() {
        return myApplication;
    }

    public static Context getContext() {
        return getApplication().getApplicationContext();
    }

    @Override
    public void onCreate() {
        super.onCreate();
        myApplication = this;
    }
}
