package com.kitsprout.main;

import androidx.appcompat.app.AppCompatActivity;

import android.graphics.Color;
import android.os.Bundle;
import android.os.Handler;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;
import android.widget.Toast;

import com.kitsprout.ks.KBluetooth;
import com.kitsprout.ks.KSerial;
import com.kitsprout.ks.KSerial.KPacket;

import java.util.Locale;

public class MainActivity extends AppCompatActivity {

    private Menu bluetoothDeviceMenu = null;

    private TextView bluetoothRecvText;
    private TextView bluetoothRecvBufferText;
    private EditText bluetoothSendText;
    private Button bluetoothSendButton;
    private Button bluetoothConnectStatus;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        // layout
        bluetoothRecvText = findViewById(R.id.textViewBluetoothRecv);
        bluetoothRecvBufferText = findViewById(R.id.textViewBluetoothRecvBuffer);
        bluetoothSendText = findViewById(R.id.editTextBluetoothSend);
        bluetoothSendButton = findViewById(R.id.buttonBluetoothSend);
        bluetoothConnectStatus = findViewById(R.id.buttonBluetoothConnectStatus);
        changeConnectStatusColor(false);

        // bluetooth startup
        if (!KBluetooth.startup(this)) {
            Log.e("KS_DBG", "bluetoothStartup ... error");
        }
        bluetoothRecvText.setText(getPacketString(0, 0, 0, 0, 0, new KPacket[0]));

        // keep screen on
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        bluetoothDeviceMenu = menu;
        return super.onCreateOptionsMenu(bluetoothDeviceMenu);
    }

    @Override
    public boolean onPrepareOptionsMenu(Menu menu) {
        bluetoothDeviceMenu = KBluetooth.updateDeviceMenu(menu);
        return super.onPrepareOptionsMenu(bluetoothDeviceMenu);
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        int id = item.getItemId();
        int state = KBluetooth.select(id);
        if (state == KBluetooth.DISCONNECTED) {
//            bluetoothRecvBufferText.setText("");
            changeConnectStatusColor(false);
            Toast.makeText(this, "DISCONNECT SUCCESS", Toast.LENGTH_SHORT).show();
        } else if (state == KBluetooth.DISCONNECT_FAILED) {
            Toast.makeText(this, "DISCONNECT FAILED", Toast.LENGTH_SHORT).show();
        } else if (state == KBluetooth.CONNECT_FAILED) {
            Toast.makeText(this, "CONNECT FAILED", Toast.LENGTH_SHORT).show();
        } else if (state == KBluetooth.CONNECTED) {
            bluetoothBeginListening();
            Toast.makeText(this, "CONNECT SUCCESS", Toast.LENGTH_SHORT).show();
        }
        return super.onOptionsItemSelected(item);
    }

    private void changeConnectStatusColor(boolean enable) {
        if (enable) {
            bluetoothSendButton.setTextColor(Color.BLACK);
            bluetoothConnectStatus.setBackgroundColor(Color.RED);
        } else {
            bluetoothSendButton.setTextColor(Color.GRAY);
            bluetoothConnectStatus.setBackgroundColor(Color.parseColor("#DCDCDC"));
        }
    }

    public void OnClickBluetoothSendData(View view) {
        if (KBluetooth.isConnected()) {
            int lens = KBluetooth.send(bluetoothSendText.getText().toString().getBytes());
            Log.d("KS_DBG", String.format("OnClickBluetoothSendData() ... lens = %d", lens));
        } else {
            Log.d("KS_DBG", "OnClickBluetoothSendData() ... without connect");
        }
    }

    KSerial ks;
    Thread bluetoothRecvThread;
    void bluetoothBeginListening() {
        Log.d("KS_DBG", "bluetoothBeginListening()");
        final Handler handler = new Handler();
        ks = new KSerial(32*1024, 0.001);
        ks.enableLostRateDetection(true);
        bluetoothRecvThread = new Thread(new Runnable() {
            public void run() {
                changeConnectStatusColor(true);
                while (!Thread.currentThread().isInterrupted() && KBluetooth.isConnected()) {
                    byte[] receiveBytes = KBluetooth.receive();
                    if (receiveBytes != null) {
                        int bytesAvailable = receiveBytes.length;
                        if (bytesAvailable > 0) {
                            KPacket[] pk = ks.getPacket(receiveBytes);
                            for (KSerial.KPacket KPacket : pk) {
                                Log.d("KSERIAL", String.format("%6d,%.0f,%d,%d",
                                        ks.getPacketParameterU16(KPacket), ks.getFrequency(0), bytesAvailable, pk.length));
                            }
                            if (pk.length > 0) {
                                // show information
                                final String recvByteString = getPacketHexString(ks.setPacket(pk[pk.length-1]));
                                final String recvBufferString = getPacketString(ks.getFrequency(0), ks.getTimes(), bytesAvailable, ks.getLostCount(), ks.getPacketTotalCount(), pk);
                                handler.post(new Runnable() {
                                    public void run() {
                                        bluetoothRecvText.setText(recvBufferString);
                                        bluetoothRecvBufferText.setText(recvByteString);
                                    }
                                });
                            }
                        }
                    } else {
                        KBluetooth.disconnect();
                        break;
                    }
                }
                changeConnectStatusColor(false);
            }
        });
        bluetoothRecvThread.start();
    }

    private String getPacketHexString(byte[] buf) {
        StringBuilder logString;
        logString = new StringBuilder("");
        for (byte b : buf) {
            logString.append(String.format(Locale.ENGLISH, " %02X", b));
        }
        return logString.toString();
    }

    private String getPacketString(double freq, double time, int bytesAvailable, long lostCount, long bytesTotal, KPacket[] packet) {
        int idx = packet.length - 1;
        StringBuilder logString;
        logString = new StringBuilder(String.format(Locale.ENGLISH, "Freq:  %.2f Hz\n", freq));
        logString.append(String.format(Locale.ENGLISH, "Time:  %.3f sec\n", time));
        logString.append(String.format(Locale.ENGLISH, "Recv:  %d\n", bytesAvailable));
        logString.append(String.format(Locale.ENGLISH, "Lost:  %d\n", lostCount));
        if (idx < 0) {
            return logString.toString();
        }
        logString.append("\n");
        logString.append(String.format(Locale.ENGLISH, "Total: %d (%d)\n", bytesTotal, packet.length));
        logString.append(String.format(Locale.ENGLISH, "Type:  %s\n", KSerial.typeConvert(packet[idx].type)));
        logString.append(String.format(Locale.ENGLISH, "Lens:  %d (%d bytes)\n", packet[idx].data.length, packet[idx].nbyte));
        logString.append(String.format(Locale.ENGLISH, "Param: %02X, %02X\n", packet[idx].param[0], packet[idx].param[1]));
        logString.append("\n");
        for (int i = 0; i < packet[idx].data.length; i++) {
            logString.append(String.format(Locale.ENGLISH, "Data[%d]: %.0f\n", i, packet[idx].data[i]));
        }
        return logString.toString();
    }

}
