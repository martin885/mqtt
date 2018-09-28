package com.example.mat88.mqtt;

import android.app.Service;
import android.content.Intent;
import android.os.IBinder;
import android.os.Looper;
import android.support.v4.content.LocalBroadcastManager;
import android.view.View;
import android.widget.Toast;

import org.eclipse.paho.android.service.MqttAndroidClient;
import org.eclipse.paho.client.mqttv3.IMqttActionListener;
import org.eclipse.paho.client.mqttv3.IMqttDeliveryToken;
import org.eclipse.paho.client.mqttv3.IMqttToken;
import org.eclipse.paho.client.mqttv3.MqttCallback;
import org.eclipse.paho.client.mqttv3.MqttClient;
import org.eclipse.paho.client.mqttv3.MqttConnectOptions;
import org.eclipse.paho.client.mqttv3.MqttException;
import org.eclipse.paho.client.mqttv3.MqttMessage;

import java.io.UnsupportedEncodingException;

public class mqttService extends Service {
    public mqttService() {
    }

    @Override
    public IBinder onBind(Intent intent) {
        setBinder(new mqttBinder(this));
        return getBinder();
    }


    @Override
    public void onCreate() {
        super.onCreate();
        conectarse();
    }


    private void conectarse() {
        MqttConnectOptions options = new MqttConnectOptions();
        options.setUserName("");
        options.setPassword("".toCharArray());
        String clientId = MqttClient.generateClientId();


        setClient(new MqttAndroidClient(this.getApplicationContext(), "tcp://mqtt.flespi.io:1883",
                clientId));


        try {
            IMqttToken token = getClient().connect(options);
            token.setActionCallback(new IMqttActionListener() {
                @Override
                public void onSuccess(IMqttToken asyncActionToken) {
                    // We are connected
                    System.out.println("onSuccess");
                    String topic1 = "probando/movimiento";
                    int qos = 0;
                    try {
                        IMqttToken subToken = getClient().subscribe(topic1, qos);
                        subToken.setActionCallback(new IMqttActionListener() {
                            @Override
                            public void onSuccess(IMqttToken asyncActionToken) {
                                // The message was published

                            }

                            @Override
                            public void onFailure(IMqttToken asyncActionToken,
                                                  Throwable exception) {
                                // The subscription could not be performed, maybe the user was not
                                // authorized to subscribe on the specified topic e.g. using wildcards

                            }
                        });
                    } catch (MqttException e) {
                        e.printStackTrace();
                    }
                }

                @Override
                public void onFailure(IMqttToken asyncActionToken, Throwable exception) {
                    // Something went wrong e.g. connection timeout or firewall problems
                    System.out.println("onFailure");

                }
            });
        } catch (MqttException e) {
            e.printStackTrace();
        }


        getClient().setCallback(new MqttCallback() {
            @Override
            public void connectionLost(Throwable cause) {
                Toast.makeText(getApplicationContext(), "CONEXION PERDIDA", Toast.LENGTH_LONG).show();
                conectarse();
            }

            @Override
            public void messageArrived(String topic, MqttMessage message) throws Exception {
                System.out.println(topic + " " + new String(message.getPayload()));

                Intent intent = new Intent("mqtt_message");
                intent.putExtra("movimiento", new String(message.getPayload()));
                LocalBroadcastManager.getInstance(getApplicationContext()).sendBroadcast(intent);

            }

            @Override
            public void deliveryComplete(IMqttDeliveryToken token) {

            }
        });
    }

    public void publish(View v) {

        if (v.getId() == R.id.button) {
            String topic = "probando/digital";
            String payload = "N";
            byte[] encodedPayload = new byte[0];
            try {
                encodedPayload = payload.getBytes("UTF-8");
                MqttMessage message = new MqttMessage(encodedPayload);
                getClient().publish(topic, message);
            } catch (UnsupportedEncodingException | MqttException e) {
                e.printStackTrace();
            }
        } else if (v.getId() == R.id.button2) {

            String topic = "probando/digital";
            String payload = "F";
            byte[] encodedPayload = new byte[0];
            try {
                encodedPayload = payload.getBytes("UTF-8");
                MqttMessage message = new MqttMessage(encodedPayload);
                getClient().publish(topic, message);
            } catch (UnsupportedEncodingException | MqttException e) {
                e.printStackTrace();
            }
        }

    }


    private MqttAndroidClient client;

    private IBinder Binder;


    public IBinder getBinder() {
        return Binder;
    }

    public void setBinder(IBinder binder) {
        Binder = binder;
    }

    public MqttAndroidClient getClient() {
        return client;
    }

    public void setClient(MqttAndroidClient client) {
        this.client = client;
    }


}
