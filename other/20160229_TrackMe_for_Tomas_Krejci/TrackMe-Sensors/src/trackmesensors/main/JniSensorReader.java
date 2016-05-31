package trackmesensors.main;

import java.util.concurrent.BlockingQueue;

import trackme.androidevent.EventType;
import android.content.Context;
import android.os.Handler;
import android.os.Looper;
import android.os.Message;
import android.util.Log;

// This sensor reader uses the JNI interface to read the data from sensors
// using native code. The implementation is in the jni/sensors-jni.cpp file.
public class JniSensorReader extends Thread implements SensorReader {
	static {
		System.loadLibrary("sensors-jni");
	}

	private String LOG_TAG = "TrackMe-Sensors";
	private BlockingQueue<trackme.androidevent.SensorEvent> eventBuffer;
	private Context context;
	private boolean running = false;
	private Handler mHandler;

	private native void jniStart();
	private native void jniStop();

	@Override
	public void startReading()
	{
		running = true;
		putStartEvent();
		start();
	}
	
	@Override
	public void run() {
		Looper.prepare();

		// Handler for receiving events from JNI.
		mHandler = new Handler() {
			public void handleMessage(Message msg) {
				if (msg.what == EventType.STOP) {
					jniStop();
					Looper.myLooper().quit();
				}
			}
		};

		jniStart();

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
		// no-op
	}
	
	private void put(int type, long timestamp, float x, float y, float z)
	{
		put(trackme.androidevent.SensorEvent.fromHardwareEvent(
		    type, timestamp, new float[]{x,y,z}));
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
	public void setContext(Context context) {
		this.context = context;
	}

	@Override
	public void setEventBuffer(
			BlockingQueue<trackme.androidevent.SensorEvent> eventBuffer) {
		this.eventBuffer = eventBuffer;
	}

}
