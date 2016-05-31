package trackme.androidevent;

import java.nio.ByteBuffer;
import java.util.Scanner;

// TrackMe representation of sensor event with various conversion
// from the Android sensor event.
public class SensorEvent {
	public int type;
	public long timestamp;
	public float[] values;
	
	public static SensorEvent fromHardwareEvent(
        android.hardware.SensorEvent event)
	{
		return fromHardwareEvent(
		    event.sensor.getType(),
		    event.timestamp,
		    event.values);
	}

	public static SensorEvent fromHardwareEvent(
	    int hwtype,
	    long timestamp,
	    float[] values)
	{
		int type;
		switch (hwtype) {
			case android.hardware.Sensor.TYPE_ACCELEROMETER:
				type = EventType.ACCELEROMETER;
				break;
			case android.hardware.Sensor.TYPE_GYROSCOPE_UNCALIBRATED:
			case android.hardware.Sensor.TYPE_GYROSCOPE:
				type = EventType.GYROSCOPE;
				break;
			case android.hardware.Sensor.TYPE_LINEAR_ACCELERATION:
				type = EventType.LINEAR_ACCELERATION;
				break;
			case android.hardware.Sensor.TYPE_MAGNETIC_FIELD_UNCALIBRATED:
			case android.hardware.Sensor.TYPE_MAGNETIC_FIELD:
				type = EventType.MAGNETIC_FIELD;
				break;
			default:
				type = EventType.UNKNOWN;
		}

		return new SensorEvent(type, timestamp, values);
	}
	
	public SensorEvent(int type, long timestamp, float[] values)
	{
		this.type = type;
		this.timestamp = timestamp;
		this.values = values.clone();
	}
	
	public String toString() {
		String str = type + "\t" + timestamp;
		for (int i = 0; i < 3; ++i)
			str += "\t" + values[i];
		return str;
	}
	
	public static SensorEvent fromString(String str) {
		Scanner scn = new Scanner(str);
		int type = scn.nextInt();
		long timestamp = scn.nextLong();
		float values[] = new float[3];
		for (int i = 0; i < values.length; ++i)
			values[i] = scn.nextFloat();
		return new SensorEvent(type, timestamp, values);
	}
	
	public byte[] toByteArray() {
		ByteBuffer bb = ByteBuffer.allocate(25);
		// magic number
		bb.putInt(0x56289740);
		bb.put(Integer.valueOf(type).byteValue());
		bb.putLong(timestamp);
		for (int i = 0; i < values.length; ++i)
			bb.putInt(Float.floatToIntBits(values[i]));
		return bb.array();
	}
	
	public static SensorEvent stopEvent()
	{
		return new SensorEvent(EventType.STOP, 0, new float[3]);
	}

	public static SensorEvent startEvent()
	{
		return new SensorEvent(EventType.START, 0, new float[3]);
	}
}
