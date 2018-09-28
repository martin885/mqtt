package com.example.mat88.mqtt;

import android.os.Binder;

public class mqttBinder extends Binder {





    public mqttBinder(mqttService service){
        this.setService(service);
    }

    private mqttService Service;

    public void setService(mqttService service) {
        Service = service;
    }

    public mqttService getService() {
        return Service;
    }
}
