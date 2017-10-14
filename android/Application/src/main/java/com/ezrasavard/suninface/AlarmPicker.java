package com.ezrasavard.suninface;

import android.app.Activity;
import android.app.AlarmManager;
import android.app.Dialog;
import android.app.DialogFragment;
import android.app.PendingIntent;
import android.app.TimePickerDialog;
import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.widget.TimePicker;
import android.widget.Toast;

import java.util.Calendar;
import java.util.concurrent.TimeUnit;

public class AlarmPicker extends DialogFragment implements TimePickerDialog.OnTimeSetListener {

    private static final int SUNRISE_ALARM_ID = 12345;
    private static final int NOTIFY_ALARM_ID = 12346;

    @Override
    public Dialog onCreateDialog(Bundle savedInstanceState) {
        int hour = 6;
        int minute = 0;
        return new TimePickerDialog(getActivity(), this, hour, minute, true);
    }

    @Override
    public void onTimeSet(TimePicker view, int hourOfDay, int minute) {

        Calendar currentTime = Calendar.getInstance();
        Calendar alarmTime = Calendar.getInstance();
        alarmTime.set(Calendar.HOUR_OF_DAY, hourOfDay);
        alarmTime.set(Calendar.MINUTE, minute);
        alarmTime.set(Calendar.SECOND, 0);
        alarmTime.set(Calendar.MILLISECOND, 0);

        if (currentTime.after(alarmTime)) {
            alarmTime.add(Calendar.DAY_OF_YEAR, 1);
        }
        Calendar notificationTime = (Calendar) alarmTime.clone();

        // Adjust it back by the sunrise fade duration
        alarmTime.add(Calendar.MILLISECOND, -HeadboardCommand.DEFAULT_SUNRISE_FADE_MILLIS);

        Toast.makeText(getContext(), String.format("Sunrise will start at: %s", alarmTime.getTime().toString()),
                Toast.LENGTH_SHORT).show();

        if (currentTime.after(alarmTime)) {
            Toast.makeText(getContext(), String.format("Error - alarm must be at least %d minutes later than now",
                    TimeUnit.MILLISECONDS.toMinutes(HeadboardCommand.DEFAULT_SUNRISE_FADE_MILLIS)), Toast.LENGTH_LONG).show();
            return;
        }

        PendingIntent sunriseIntent = createSunriseIntent();
        PendingIntent notificationIntent = createNotificationIntent();

        AlarmManager am = (AlarmManager) getContext().getSystemService(Activity.ALARM_SERVICE);
        am.set(AlarmManager.RTC_WAKEUP, alarmTime.getTimeInMillis(),
                sunriseIntent);
        am.set(AlarmManager.RTC_WAKEUP, notificationTime.getTimeInMillis(),
                notificationIntent);
    }

    private PendingIntent createSunriseIntent() {
        // Set the sunrise alarm PendingIntent
        Intent intent = new Intent(getContext(), BluetoothCommunicationService.class);
        intent.setAction(BluetoothCommunicationService.ACTION_SEND_COMMAND);
        intent.putExtra(BluetoothCommunicationService.KEY_COMMAND, HeadboardCommand.sunrise());
        intent.putExtra(BluetoothCommunicationService.KEY_SILENT, true);
        return PendingIntent.getService(getContext(), SUNRISE_ALARM_ID, intent,
                PendingIntent.FLAG_CANCEL_CURRENT);
    }

    private PendingIntent createNotificationIntent() {
        Intent intent = new Intent(getContext(), AlarmNotifierService.class);
        return PendingIntent.getService(getContext(), NOTIFY_ALARM_ID, intent,
                PendingIntent.FLAG_CANCEL_CURRENT);
    }

    public void clearAlarm(Context context) {
        Intent intent = new Intent(context, BluetoothCommunicationService.class);
        intent.setAction(BluetoothCommunicationService.ACTION_SEND_COMMAND);
        PendingIntent pendingIntent = PendingIntent.getService(context,
                SUNRISE_ALARM_ID, intent, PendingIntent.FLAG_CANCEL_CURRENT);
        PendingIntent notificationIntent = PendingIntent.getService(context,
                NOTIFY_ALARM_ID, new Intent(context, AlarmNotifierService.class),
                PendingIntent.FLAG_CANCEL_CURRENT);
        AlarmManager am = (AlarmManager) context.getSystemService(Activity.ALARM_SERVICE);
        am.cancel(pendingIntent);
        am.cancel(notificationIntent);
        Toast.makeText(context, "Alarm cleared", Toast.LENGTH_SHORT).show();
    }
}
