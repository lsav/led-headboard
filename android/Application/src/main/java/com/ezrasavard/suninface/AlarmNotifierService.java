package com.ezrasavard.suninface;

import android.app.IntentService;
import android.app.NotificationManager;
import android.app.PendingIntent;
import android.content.Context;
import android.content.Intent;
import android.graphics.BitmapFactory;
import android.support.annotation.Nullable;
import android.support.v4.app.NotificationCompat;

public class AlarmNotifierService extends IntentService {

    public static final int NOTIFICATION_ID = 1;

    public AlarmNotifierService() {
        super(AlarmNotifierService.class.getSimpleName());
    }

    @Override
    protected void onHandleIntent(@Nullable Intent intent) {
        Context context = getApplicationContext();
        NotificationCompat.Builder builder = new NotificationCompat.Builder(context)
                .setSmallIcon(R.mipmap.ic_launcher)
                .setLargeIcon(BitmapFactory.decodeResource(context.getResources(),
                        R.mipmap.ic_launcher))
                .setContentTitle("SunInFace Reminder")
                .setContentText("Click here to turn off the light")
                .setAutoCancel(true);
        Intent bluetoothIntent = new Intent(context, BluetoothCommunicationService.class);
        bluetoothIntent.setAction(BluetoothCommunicationService.ACTION_SEND_COMMAND);
        bluetoothIntent.putExtra(BluetoothCommunicationService.KEY_COMMAND, HeadboardCommand.lightsOff());
        bluetoothIntent.putExtra(BluetoothCommunicationService.KEY_SILENT, true);
        PendingIntent pendingBluetoothIntent = PendingIntent.getService(context,
                0, bluetoothIntent, PendingIntent.FLAG_UPDATE_CURRENT);
        builder.setContentIntent(pendingBluetoothIntent);
        NotificationManager manager = (NotificationManager) getSystemService(NOTIFICATION_SERVICE);
        manager.notify(NOTIFICATION_ID, builder.build());
    }
}
