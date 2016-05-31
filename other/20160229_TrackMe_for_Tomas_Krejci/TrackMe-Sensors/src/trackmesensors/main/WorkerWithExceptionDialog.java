package trackmesensors.main;

import java.util.concurrent.Callable;

import android.app.AlertDialog;
import android.content.Context;
import android.os.AsyncTask;

// This worker is used for processing a task in separated thread. When
// the task fails, the worker propagates the exception to the GUI
// thread and shows an error dialog.
public class WorkerWithExceptionDialog extends AsyncTask<Void,Void,Void> {
	private Callable<Void> call;
	private Runnable onException; 
	private Context context;
	private Throwable exception;

	public WorkerWithExceptionDialog(Callable<Void> call,
									 Runnable onException,
									 Context context)
	{
		this.call = call;
		this.onException = onException;
		this.context = context;
	}

	protected Void doInBackground(Void... args)
	{
		try {
			call.call();
		}
		catch (Throwable t) {
			exception = t;
		}
		return null;
	}
	
	protected void onPostExecute(Void result)
	{
		if (exception != null) {
			onException.run();
			new AlertDialog.Builder(context)
				.setMessage(exception.toString()).create().show();
		}
	}

}
