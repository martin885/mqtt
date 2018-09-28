package com.example.mat88.mqtt;


import android.content.BroadcastReceiver;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.ServiceConnection;
import android.os.Bundle;
import android.os.IBinder;
import android.support.v4.content.LocalBroadcastManager;
import android.support.v7.app.AppCompatActivity;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;

public class MainActivity extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);


        Button button = findViewById(R.id.button);
        button.setOnClickListener(new View.OnClickListener() {

            @Override
            public void onClick(View v) {
                mqttService.publish(v);
            }
        });

        Button button2 = findViewById(R.id.button2);
        button2.setOnClickListener(new View.OnClickListener() {

            @Override
            public void onClick(View v) {

                mqttService.publish(v);
            }
        });

        textView = findViewById(R.id.textView);

        LocalBroadcastManager.getInstance(this).registerReceiver(
                new BroadcastReceiver(){

                    @Override
                    public void onReceive(Context context, Intent intent) {
                        String movimiento=intent.getStringExtra("movimiento");
                        textView.setText(movimiento);
                    }
                },new IntentFilter("mqtt_message")
        );

    }


    TextView textView;
    private mqttService mqttService = null;
    private boolean mqttBound = false;


    @Override
    protected void onStart() {
        super.onStart();
        Intent mqttServicio = new Intent(this, mqttService.class);
        bindService(mqttServicio, mqttConnection, Context.BIND_AUTO_CREATE);

    }


    private ServiceConnection mqttConnection = new ServiceConnection() {
        @Override
        public void onServiceConnected(ComponentName className, IBinder service) {
            mqttBinder binder = (mqttBinder) service;
            mqttService = binder.getService();
            mqttBound = true;
        }

        @Override
        public void onServiceDisconnected(ComponentName arg0) {
            mqttBound = false;
        }

    };
}
