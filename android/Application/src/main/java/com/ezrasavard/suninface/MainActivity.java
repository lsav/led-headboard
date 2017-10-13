package com.ezrasavard.suninface;

import android.app.Activity;
import android.bluetooth.BluetoothAdapter;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.Bundle;
import android.support.v4.content.LocalBroadcastManager;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;
import android.widget.Toast;

import java.util.concurrent.TimeUnit;

/**
 * States:
 * initialize to disconnected
 * terminal states are connected and disconnected
 * intermediate state is busy
 *
 */
public class MainActivity extends Activity {
    Button buttonLumos;
    Button buttonNox;
    Button buttonClear;
    Button buttonAlarm;
    Button buttonNoAlarm;
    TextView textView;
    Intent bluetoothIntent;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        buttonLumos = (Button) findViewById(R.id.buttonLumos);
        buttonNox = (Button) findViewById(R.id.buttonNox);
        buttonClear = (Button) findViewById(R.id.buttonClear);
        buttonAlarm = (Button) findViewById(R.id.buttonAlarm);
        buttonNoAlarm = (Button) findViewById(R.id.buttonNoAlarm);
        textView = (TextView) findViewById(R.id.textView);
        bluetoothIntent = new Intent(getApplicationContext(), BluetoothCommunicationService.class);
        setUILocked();

        IntentFilter bluetoothStatusIntentFilter =
                new IntentFilter(BluetoothCommunicationService.RESPONSE_ACTION);
        BluetoothStatusReceiver receiver = new BluetoothStatusReceiver();
        LocalBroadcastManager.getInstance(this)
                .registerReceiver(receiver, bluetoothStatusIntentFilter);
        configureBluetooth();
        setUIUnlocked();
    }

    public void setUIUnlocked() {
        buttonLumos.setEnabled(true);
        buttonNox.setEnabled(true);
        buttonAlarm.setEnabled(true);
        buttonNoAlarm.setEnabled(true);
        textView.setEnabled(true);
    }

    private void setUILocked() {
        buttonLumos.setEnabled(false);
        buttonNox.setEnabled(false);
        buttonAlarm.setEnabled(false);
        buttonNoAlarm.setEnabled(false);
        textView.setEnabled(false);
    }

    /**
     * Configure and enable the bluetooth adapter
     * Ensure that the remote device is bonded
     * @return success if true
     */
    public boolean configureBluetooth() {
        textView.append("\r\nConfiguring bluetooth...\r\n");
        BluetoothAdapter bluetoothAdapter = BluetoothAdapter.getDefaultAdapter();
        if (bluetoothAdapter == null) {
            Toast.makeText(getApplicationContext(),"Device does not support bluetooth",Toast.LENGTH_SHORT).show();
            return false;
        }

        if (!bluetoothAdapter.isEnabled()) {
            Intent enableAdapter = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
            startActivityForResult(enableAdapter, 0);
            try {
                Thread.sleep(1000);
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }

        if (!bluetoothAdapter.isEnabled()) {
            Toast.makeText(getApplicationContext(), "Please pair the device first", Toast.LENGTH_SHORT).show();
            textView.append("\r\nConnection configuration failed\r\n");
            return false;
        }
        textView.append("\r\nBluetooth is ready\r\n");
        return true;
    }

    public void onClickLumos(View view) {
        issueCommand(HeadboardCommand.lightsOn());
    }

    public void onClickNox(View view) {
        issueCommand(HeadboardCommand.lightsOff());
    }

    public void onClickClear(View view) {
        textView.setText("");
    }

    public void onClickAlarm(View view) {
        AlarmPicker alarmPicker = new AlarmPicker();
        alarmPicker.show(getFragmentManager(), "alarmPicker");
    }

    public void onClickNoAlarm(View view) {
        AlarmPicker alarmPicker = new AlarmPicker();
        alarmPicker.clearAlarm(getApplicationContext());
    }

    // useful for colour calibration testing
    public void onClickSunrise(View view) {
        issueCommand(HeadboardCommand.sunrise((int) TimeUnit.SECONDS.toMillis(30)));
    }

    private void issueCommand(String command) {
        setUILocked();
        bluetoothIntent.setAction(BluetoothCommunicationService.ACTION_SEND_COMMAND);
        bluetoothIntent.putExtra(BluetoothCommunicationService.KEY_COMMAND, command);
        getApplicationContext().startService(bluetoothIntent);
        textView.append("\r\nSent: " + command + "\r\n");
    }

    private class BluetoothStatusReceiver extends BroadcastReceiver {

        private BluetoothStatusReceiver() {
            // NOP
        }

        @Override
        public void onReceive(Context context, Intent intent) {
            String response = (String) intent
                    .getExtras()
                    .get(BluetoothCommunicationService.KEY_RESPONSE);

            if (response == null) { // this shouldn't happen
                Toast.makeText(getApplicationContext(), "Error!", Toast.LENGTH_SHORT).show();
                textView.append("\r\nUnknown bluetooth failure\r\n");
                setUIUnlocked();
                return;
            }

            switch (response) {
                case BluetoothCommunicationService.RESULT_CONNECT_FAIL:
                    setUIUnlocked();
                    textView.append("\r\nCould not connect\r\n");
                    break;
                case BluetoothCommunicationService.RESULT_BLUETOOTH_UNAVAILABLE:
                    setUIUnlocked();
                    textView.append("\r\nDevice not set\r\n");
                    break;
                case BluetoothCommunicationService.RESULT_SEND_SUCCESS:
                    setUIUnlocked();
                    textView.append("\r\nSend succeeded\r\n");
                    break;
                case BluetoothCommunicationService.RESULT_SEND_FAIL:
                    setUIUnlocked();
                    textView.append("\r\nSend failed\r\n");
                    break;
                case BluetoothCommunicationService.RESULT_FAIL_OTHER:
                default:
                    setUIUnlocked();
                    Toast.makeText(getApplicationContext(), "Error!", Toast.LENGTH_SHORT).show();
                    textView.append("\r\nUnknown bluetooth failure\r\n");
            }
        }
    }
}
