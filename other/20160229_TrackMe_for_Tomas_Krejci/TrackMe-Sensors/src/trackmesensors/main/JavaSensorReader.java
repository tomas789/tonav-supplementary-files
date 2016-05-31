package trackmesensors.main;

import java.util.ArrayList;
import java.util.concurrent.BlockingQueue;

import trackme.androidevent.EventType;
import android.content.Context;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.os.Handler;
import android.os.Looper;
import android.os.Message;
import android.util.Log;

// This sensor reader entirely relies on the sensor reading
// provided by the Androi API. It runs in a separated thread.
public class JavaSensorReader extends Thread
                              implements SensorReader,SensorEventListener {
	private String LOG_TAG = "TrackMe-Sensors";
	private BlockingQueue<trackme.androidevent.SensorEvent> eventBuffer;
	private SensorManager mSensorManager;
	private ArrayList<Integer> sensors = new ArrayList<Integer>();
	private Context context;
	private boolean running = false;
	private Handler mHandler;

	@Override
	public void startReading()
	{
		running = true;
		putStartEvent();
		start();
	}
	
	public void run()
	{
		Looper.prepare();

		final SensorEventListener thisListener = this;
		mHandler = new Handler() {
			public void handleMessage(Message msg) {
				if (msg.what == EventType.STOP) {
					mSensorManager.unregisterListener(thisListener);
					Looper.myLooper().quit();
				}
			}
		};

		// Initialize sensor manager
		mSensorManager =
            (SensorManager) context.getSystemService(Context.SENSOR_SERVICE);
		for (int sensor : sensors)
            registerSensor(sensor);
		
		Looper.loop();
	}

	@Override
	public void stopReading()
	{
		if (running) {
			mHandler.sendEmptyMessage(EventType.STOP);
			try {
				this.join();
			} catch (InterruptedException e) {
			}
			running = false;
		}
		putStopEvent();
	}
	
	@Override
	public void addSensor(Integer sensor)
	{
		sensors.add(sensor);
	}

	private void registerSensor(Integer type) {
	    if (!mSensorManager.registerListener(
	        this, mSensorManager.getDefaultSensor(type),
            SensorManager.SENSOR_DELAY_FASTEST))
	    	Log.e(LOG_TAG, "Sensor " + type + " cannot be registered.");
	}
	
	private void put(trackme.androidevent.SensorEvent event)
	{
		try {
			eventBuffer.put(event);
		} catch (InterruptedException e) {
			Log.e(LOG_TAG, "Cannot put an event to the queue.");
		}
	}

	private void putStopEvent()
	{
	    put(trackme.androidevent.SensorEvent.stopEvent());
	}
	
	private void putStartEvent()
	{
	    put(trackme.androidevent.SensorEvent.startEvent());
	}

	@Override
	public void onSensorChanged(SensorEvent event) {
		put(trackme.androidevent.SensorEvent.fromHardwareEvent(event));
	}

	@Override
	public void onAccuracyChanged(Sensor sensor, int accuracy) {
		// TODO Auto-generated method stub
	}

	@Override
	public void setContext(Context context) {
		this.context = context;
	}

	@Override
	public void setEventBuffer(
			BlockingQueue<trackme.androidevent.SensorEvent> eventBuffer) {
		this.eventBuffer = eventBuffer;
	}

}
