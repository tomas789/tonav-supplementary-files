package trackme.androidevent;

// Port of the EventType from the TrackMe library
public class EventType {
    public static final int UNKNOWN              = 0x0;

    public static final int SENSOR               = 0x0F; // below are sensor types
                                              // = 00001111
    public static final int ACCELEROMETER        = 0x01;
    public static final int GYROSCOPE            = 0x02;
    public static final int MAGNETIC_FIELD       = 0x03;
    public static final int ORIENTATION          = 0x04;
    public static final int VELOCITY             = 0x05;
    public static final int DISPLACEMENT         = 0x06;
    public static final int LINEAR_ACCELERATION  = 0x07;

    public static final int SYSTEM               = 0x70; // below are system events 
                                              // = 01110000
    public static final int START                = 0x10;
    public static final int STOP                 = 0x20;
}
