package trackmesensors.main;

import java.util.concurrent.BlockingQueue;
import java.util.concurrent.LinkedBlockingQueue;
import trackmesensors.main.R;
import android.app.Activity;
import android.app.AlertDialog;
import android.content.SharedPreferences;
import android.hardware.Sensor;
import android.os.Bundle;
import android.os.Handler;
import android.util.Log;
import android.view.Menu;
import android.view.View;
import android.view.WindowManager;
import android.widget.CheckBox;
import android.widget.EditText;
import android.widget.RadioButton;
import android.widget.ToggleButton;

// The main interface
public class MainActivity extends Activity {
	private String LOG_TAG = "TrackMe Sensors";

	private BlockingQueue<trackme.androidevent.SensorEvent> eventBuffer;

	private SensorReader sensorReader;
	private OutputThread outputThread;

	private Runnable runSensorReader;
	private Handler sensorReaderDelayer;

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);
		
		// Default values
		SharedPreferences prefs = getPreferences(MODE_PRIVATE);

		((EditText) findViewById(R.id.edittextFolderName))
		    .setText(prefs.getString("edittextFolderName", ""));
		((EditText) findViewById(R.id.edittextNetworkAddress))
		    .setText(prefs.getString("edittextNetworkAddress", ""));
		((EditText) findViewById(R.id.edittextNetworkPort))
		    .setText(prefs.getString("edittextNetworkPort", ""));
		
		// The sensor reader will be executed with a delay from the
		// moment of clicking the Start button. This is the object,
		// that manages the delay.
		sensorReaderDelayer = new Handler();
		runSensorReader = new Runnable() { public void run() {} };
	}

	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		// Inflate the menu; this adds items to the action bar if it is present.
		//getMenuInflater().inflate(R.menu.main, menu);
		return true;
	}

	@Override
	public void onStop() {
		stopRecording();
		super.onStop();
	}
	
	// When the application pauses, the recording will be stopped.
	@Override
	public void onPause() {
		stopRecording();

		SharedPreferences.Editor prefEdit = getPreferences(MODE_PRIVATE).edit();
		
		prefEdit.putString(
		    "edittextFolderName",
		    ((EditText) findViewById(R.id.edittextFolderName)).getText().toString());
		prefEdit.putString(
		    "edittextNetworkAddress",
		    ((EditText) findViewById(R.id.edittextNetworkAddress)).getText().toString());
		prefEdit.putString(
		    "edittextNetworkPort",
		    ((EditText) findViewById(R.id.edittextNetworkPort)).getText().toString());

		prefEdit.apply();
		super.onPause();
	}
	
	public void startRecording() {
		stopRecording();

		Log.i(LOG_TAG, "Starting recording");

		// Initialize the output thread
	    eventBuffer = new LinkedBlockingQueue<trackme.androidevent.SensorEvent>();
		outputThread = new OutputThread(eventBuffer);

		// File output settings
		if (((CheckBox) findViewById(R.id.checkboxFolderOutput)).isChecked()) {
			EditText folderName = (EditText) findViewById(R.id.edittextFolderName);
			outputThread.setFile(folderName.getText().toString(),
					System.currentTimeMillis() + ".sns");
		}
		// Network output settings
		if (((CheckBox) findViewById(R.id.checkboxNetworkOutput)).isChecked()) {
			EditText Address = (EditText) findViewById(R.id.edittextNetworkAddress);
			EditText Port = (EditText) findViewById(R.id.edittextNetworkPort);
			int port;
			try {
			    port = Integer.parseInt(Port.getText().toString());
			}
			catch (NumberFormatException e) {
				new AlertDialog.Builder(this)
				    .setMessage("Invalid port number").create().show();
				stopRecording();
				return;
			}
			outputThread.setTcpServer(Address.getText().toString(), port);
		}

		// Output thread exception handler
		Runnable onException = new Runnable() { public void run() {
            stopRecording();
        }};
		// Android AsyncTask
		new WorkerWithExceptionDialog(outputThread, onException, this).execute();

		// Initialize sensors
		RadioButton NativeAccelGyro =
				(RadioButton) findViewById(R.id.radioNativeAccelGyro);
		RadioButton AccelGyro =
				(RadioButton) findViewById(R.id.radioAccelGyro);
		RadioButton LinearAcceleration =
				(RadioButton) findViewById(R.id.radioLinearAcceleration);
		if (NativeAccelGyro.isChecked()) {
			sensorReader = new JniSensorReader();
		} else if (AccelGyro.isChecked()) {
			sensorReader = new JavaSensorReader();
			sensorReader.addSensor(Sensor.TYPE_ACCELEROMETER);
			sensorReader.addSensor(Sensor.TYPE_GYROSCOPE);
			sensorReader.addSensor(Sensor.TYPE_MAGNETIC_FIELD);
		} else if (LinearAcceleration.isChecked()) {
			sensorReader = new JavaSensorReader();
			sensorReader.addSensor(Sensor.TYPE_LINEAR_ACCELERATION);
		}
		sensorReader.setEventBuffer(eventBuffer);
		sensorReader.setContext(this);

		// Wait a little bit because of the physical pushing on a button
		runSensorReader = new Runnable() { public void run() {
			sensorReader.startReading();
		}};
		sensorReaderDelayer.postDelayed(runSensorReader, 300);
		
		// Check the button in case the function was manually called
		((ToggleButton) findViewById(R.id.toggleRecording)).setChecked(true);
		
		// Keep screen on
		getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
		Log.i(LOG_TAG, "Recording started");
	}
	
	public void stopRecording() {
		Log.i(LOG_TAG, "Stopping recording");
		sensorReaderDelayer.removeCallbacks(runSensorReader);

		if (sensorReader != null)
			sensorReader.stopReading();

		// Uncheck the button in case the function was manually called
		((ToggleButton) findViewById(R.id.toggleRecording)).setChecked(false);

		// do not keep screen on
		getWindow().clearFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
		Log.i(LOG_TAG, "Recording stopped");
	}

	// Turn the recording on and off
	public void onRecordToggleClicked(View view) {
		boolean on = ((ToggleButton)view).isChecked();
		if (on) {
			startRecording();
		}
		else {
			stopRecording();
		}
	}

}
