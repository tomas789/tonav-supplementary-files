package trackmesensors.main;

import java.io.BufferedOutputStream;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.OutputStream;
import java.net.InetSocketAddress;
import java.net.Socket;
import java.util.concurrent.BlockingQueue;
import java.util.concurrent.Callable;

import trackme.androidevent.EventType;
import trackme.androidevent.SensorEvent;
import android.os.Environment;
import android.util.Log;

// This class manages the output of the application.
public class OutputThread implements Callable<Void> {
	private String LOG_TAG = "TrackMe-Sensors";
	
	private boolean useTcpServer = false;
	private String address;
	private Integer port;
	
	private boolean useFile = false;
	private String folderName;
	private String fileName;

	// Received events are stored in a buffer.
	BlockingQueue<trackme.androidevent.SensorEvent> eventBuffer;
		
	public OutputThread(
	    BlockingQueue<trackme.androidevent.SensorEvent> eventBuffer) {
	    this.eventBuffer = eventBuffer;
	}
	
	/* Checks if external storage is available for read and write */
	private boolean isExternalStorageWritable() {
	    String state = Environment.getExternalStorageState();
	    if (Environment.MEDIA_MOUNTED.equals(state)) {
	        return true;
	    }
	    return false;
	}

	public void setTcpServer(String address, int port) {
		this.address = address;
		this.port = port;
		this.useTcpServer = true;
	}
	
	public void setFile(String folderName, String fileName) {
		this.folderName = folderName;
		this.fileName = fileName;
		this.useFile = true;
	}

	private File createFolderIfNotExists() throws IOException {
		File folderPath;
		if (isExternalStorageWritable()) {
			folderPath =
				new File(Environment.getExternalStorageDirectory(), folderName);
			if (!folderPath.exists() && !folderPath.mkdirs()) {
				Log.e(LOG_TAG, "Cannot create directory " + folderName);
				throw new IOException("Cannot create directory " + folderName);
			}
		} else {
			Log.e(LOG_TAG, "External storage is not writable.");
			throw new IOException("External storage is not writable.");
		}
		return folderPath;
	}

	public Void call() throws Exception {
		OutputStream outputFile = null;
		Socket tcpSocket = null;
		OutputStream tcpStream = null;
		try {
            if (useFile) {
            	File folderPath = createFolderIfNotExists();
                outputFile =
                    new BufferedOutputStream(
                	new FileOutputStream(
                	new File(folderPath, fileName)));
            }
            if (useTcpServer) {
            	tcpSocket = new Socket();
            	tcpSocket.connect(new InetSocketAddress(address, port), 5000);
            	tcpStream = tcpSocket.getOutputStream();
            }

            // The main writing loop
			SensorEvent event;
			do {
				event = eventBuffer.take();
				// File output
				if (useFile)
					outputFile.write(event.toByteArray());
				// Network output
				if (useTcpServer) {
					tcpStream.write(event.toByteArray());
				}
			} while (event.type != EventType.STOP);
		} catch (IOException e) {
			Log.e(LOG_TAG, e.getMessage());
			throw e;
		}
		finally {
			if (outputFile != null)
				outputFile.close();
			if (tcpStream != null)
				tcpStream.close();
			if (tcpSocket != null)
				tcpSocket.close();
		}
        return null;
	}

}
