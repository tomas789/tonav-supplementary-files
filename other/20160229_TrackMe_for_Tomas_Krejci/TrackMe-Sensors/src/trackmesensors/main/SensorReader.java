package trackmesensors.main;

import java.util.concurrent.BlockingQueue;
import android.content.Context;

// The interface for sensor readers.
public interface SensorReader {
	public void setContext(Context context);

	public void setEventBuffer(
        BlockingQueue<trackme.androidevent.SensorEvent> eventBuffer);

	public void startReading();
	
	public void stopReading();
	
	public void addSensor(Integer sensor);
}
