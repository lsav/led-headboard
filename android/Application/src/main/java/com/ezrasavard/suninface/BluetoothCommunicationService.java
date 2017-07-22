package com.ezrasavard.suninface;

import android.app.IntentService;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothSocket;
import android.content.Intent;
import android.support.annotation.Nullable;
import android.support.v4.content.LocalBroadcastManager;

import java.io.IOException;
import java.io.OutputStream;
import java.lang.reflect.InvocationTargetException;
import java.util.UUID;

public class BluetoothCommunicationService extends IntentService {

    private static final UUID LOCAL_PORT_UUID = UUID.fromString("00001101-0000-1000-8000-00805f9b34fb"); // Common Serial Port Service ID
    public static final String DEVICE_ADDRESS="98:D3:31:20:53:BF";
    public static final String RESPONSE_ACTION =
            BluetoothCommunicationService.class.getSimpleName() + ".response";
    public static final String KEY_RESPONSE = "response";
    public static final String KEY_SILENT = "silent";
    public static final String KEY_COMMAND = "command";
    public static final String RESULT_CONNECT_FAIL = "failed - could not connect";
    public static final String ACTION_SEND_COMMAND = "send";
    public static final String RESULT_SEND_SUCCESS = "success - command sent";
    public static final String RESULT_SEND_FAIL = "failed - unknown command";
    public static final String RESULT_FAIL_OTHER = "failed";
    public static final String RESULT_BLUETOOTH_UNAVAILABLE = "failed - bluetooth not enabled";

    private OutputStream outputStream;
    private BluetoothSocket socket;

    public BluetoothCommunicationService() {
        super(BluetoothCommunicationService.class.getSimpleName());
    }

    @Override
    protected void onHandleIntent(@Nullable Intent intent) {
        Intent actionResponse = new Intent(RESPONSE_ACTION);
        if (intent == null) {
            actionResponse.putExtra(KEY_RESPONSE, RESULT_FAIL_OTHER);
            LocalBroadcastManager.getInstance(this).sendBroadcast(actionResponse);
            return;
        }

        BluetoothAdapter bluetoothAdapter = BluetoothAdapter.getDefaultAdapter();
        if (!bluetoothAdapter.isEnabled()) {
            actionResponse.putExtra(KEY_RESPONSE, RESULT_BLUETOOTH_UNAVAILABLE);
            LocalBroadcastManager.getInstance(this).sendBroadcast(actionResponse);
            return;
        }

        BluetoothDevice device = bluetoothAdapter.getRemoteDevice(DEVICE_ADDRESS);

        switch (intent.getAction()) {
            case ACTION_SEND_COMMAND: // currently this is the only command
                if (!intent.hasExtra(KEY_COMMAND)) {
                    actionResponse.putExtra(KEY_RESPONSE, RESULT_FAIL_OTHER);
                    break;
                }
                String data = (String) intent.getExtras().get(KEY_COMMAND);
                if (data == null) {
                    actionResponse.putExtra(KEY_RESPONSE, RESULT_SEND_FAIL);
                    break;
                }
                try {
                    createBluetoothSocket(device, LOCAL_PORT_UUID);
                    outputStream.write(data.getBytes());
                } catch (Exception e) {
                    e.printStackTrace();
                    actionResponse.putExtra(KEY_RESPONSE, RESULT_CONNECT_FAIL);
                    LocalBroadcastManager.getInstance(this).sendBroadcast(actionResponse);
                    break;
                } finally {
                    closeBluetoothConnection();
                }
                actionResponse.putExtra(KEY_RESPONSE, RESULT_SEND_SUCCESS);
                break;
            default:
                actionResponse.putExtra(KEY_RESPONSE, RESULT_FAIL_OTHER);
        }
        if (!intent.getBooleanExtra(KEY_SILENT, false)) {
            LocalBroadcastManager.getInstance(this).sendBroadcast(actionResponse);
        }
    }

    /**
     * Creates and opens a bluetooth socket to the target device
     * Handles fallback socket creation methods
     *
     * @param targetDevice Device to connect to
     * @param localDevice UUI for devices bluetooth adapter
     */
    private void createBluetoothSocket(BluetoothDevice targetDevice, UUID localDevice)
            throws IOException, NoSuchMethodException, IllegalAccessException, InvocationTargetException {

        // Hack to work around issues with bluetooth sockets in versions >= 4.2, apparently
        socket = (BluetoothSocket) targetDevice.getClass()
                .getMethod("createRfcommSocket", new Class[]{int.class})
                .invoke(targetDevice, 1);
        socket.connect();
        outputStream = socket.getOutputStream();
    }

    /**
     * Close bluetooth socket cleanly
     */
    private void closeBluetoothConnection() {
        if (socket != null) {
            try {
                socket.close();
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
    }
}
