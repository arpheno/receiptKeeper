package makemachine.android.examples;


import java.io.File;
import java.io.FileOutputStream;
import java.io.FileWriter;
import java.io.IOException;
import java.text.SimpleDateFormat;
import java.util.Calendar;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import android.app.Activity;
import android.content.Intent;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.media.ExifInterface;
import android.net.Uri;
import android.os.Bundle;
import android.os.Environment;
import android.provider.MediaStore;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.TextView;
import android.widget.Toast;

import com.googlecode.tesseract.android.TessBaseAPI;


public class PhotoCaptureExample extends Activity 
{
	protected Button _button;
	protected Button _button1;
	protected ImageView _image;
	protected TextView _field;
	protected String _path;
	protected boolean _taken;
	private NotesDbAdapter mDbHelper;

	protected static final String PHOTO_TAKEN	= "photo_taken";
	private Bitmap _bitmap;	
    @Override
    public void onCreate(Bundle savedInstanceState) 
    {
        super.onCreate(savedInstanceState);
        mDbHelper = new NotesDbAdapter(this);
        mDbHelper.open();
        setContentView(R.layout.main);
        _field = ( TextView ) findViewById( R.id.field );
        _button = ( Button ) findViewById( R.id.button );
        _button1 = ( Button ) findViewById( R.id.button1 );
        _path = Environment.getExternalStorageDirectory() + "/images/make_machine_example.jpg";
    	
        
    	
    }
    
   /* public class ButtonClickHandler implements View.OnClickListener 
    {
    	public void onClick( View view ){
    		Log.i("MakeMachine", "ButtonClickHandler.onClick()" );
    		int k=Nati.fakulty(3);
            Log.i( "MakeMachine", "hellowrold"+String.valueOf(k) );
    		startCameraActivity();
    	}
    }*/
    public void toGrayscale(int t)
    {        
    	ExifInterface ex;
        BitmapFactory.Options options = new BitmapFactory.Options();
        options.inSampleSize = 1;
        options.inPurgeable = true;
        for(int i=2;i<3;i++){
        	Log.i( "MakeMachine", "Trying window of "+String.valueOf(i*100) );
            //_bitmap.recycle();
            System.gc();
        	_bitmap = BitmapFactory.decodeFile("/mnt/sdcard/images/make_machine_example.jpg", options );
         try {
			ex= new ExifInterface(_path);
			Log.i("MakeMachine", "!!!!!"+ex.getAttribute(ExifInterface.TAG_ORIENTATION) );
		     int width, height;
		     height = _bitmap.getHeight();
		     width = _bitmap.getWidth();  
		     height = _bitmap.getHeight();
		     width = _bitmap.getWidth();  
		    Nati.greyscale(_bitmap,i*100);
		    _bitmap.recycle();
		    System.gc();
		    _bitmap=Bitmap.createBitmap(height, width, Bitmap.Config.ARGB_8888);
		    //if( ex.getAttribute(ExifInterface.TAG_ORIENTATION)=="6"){
		        Nati.getrot(_bitmap);
		    //}
 	       FileOutputStream out = new FileOutputStream("/mnt/sdcard/images/lol"+String.valueOf((int) (Math.random() * (100000)))+String.valueOf(i*100)+".jpg");
 	       _bitmap.compress(Bitmap.CompressFormat.JPEG, 90, out);
        }
         catch (IOException e) {e.printStackTrace();}
         }
        /*  
        Bitmap tempmap = Bitmap.createBitmap(width, height, Bitmap.Config.ARGB_8888);
        Canvas c = new Canvas(tempmap);
        Paint paint = new Paint();
        float [] thresh= {r_lum*256, g_lum*256, b_lum*256, 0,  -256*t, 
                r_lum*256 ,g_lum*256, b_lum*256, 0,  -256*t, 
                r_lum*256, g_lum*256, b_lum*256, 0,  -256*t, 
                0, 0, 0, 1, 0};
        ColorMatrix cm = new ColorMatrix(thresh);
        ColorMatrixColorFilter f = new ColorMatrixColorFilter(cm);
        paint.setColorFilter(f);
        c.drawBitmap(_bitmap, 0, 0, paint);
        _bitmap=tempmap;*/
    }
    public static final String DATE_FORMAT_NOW = "yyyy-MM-dd HH:mm:ss";

    public static String now() {
    Calendar cal = Calendar.getInstance();
    SimpleDateFormat sdf = new SimpleDateFormat(DATE_FORMAT_NOW);
    return sdf.format(cal.getTime());
    }

    public void startCameraActivity(View view)
    {
    	Log.i("MakeMachine", "startCameraActivity()" );
    	File file = new File( _path );
    	Uri outputFileUri = Uri.fromFile( file );
    	
    	Intent intent = new Intent(android.provider.MediaStore.ACTION_IMAGE_CAPTURE );
    	intent.putExtra( MediaStore.EXTRA_OUTPUT, outputFileUri );
    	
    	startActivityForResult( intent, 0 );
    }
    
    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) 
    {	
    	Log.i( "MakeMachine", "resultCode: " + resultCode );
    	switch( resultCode )
    	{
    		case 0:
    			Log.i( "MakeMachine", "User cancelled" );
    			break;
    			
    		case -1:
    			onPhotoTaken();
    			break;
    	}
    }
    public void generateNoteOnSD(String sFileName, String sBody){
        try
        {
            File root = new File(Environment.getExternalStorageDirectory(), "Notes");
            if (!root.exists()) {
                root.mkdirs();
            }
            File gpxfile = new File(root, sFileName);
            FileWriter writer = new FileWriter(gpxfile);
            writer.append(sBody);
            writer.flush();
            writer.close();
            Toast.makeText(this, "Saved", Toast.LENGTH_SHORT).show();
        }
        catch(IOException e)
        {
             e.printStackTrace();
        }
       }    
    protected void onPhotoTaken()
    {
    	Log.i( "MakeMachine", "onPhotoTaken" );
    	
    	_taken = true;
    	
    	toGrayscale(80);
    	/*BitmapFactory.Options options = new BitmapFactory.Options();
        options.inSampleSize = 2;
        options.inPurgeable = true;
         _bitmap = BitmapFactory.decodeFile("/mnt/sdcard/images/make_machine_example.jpg", options );
        */
    	TessBaseAPI baseApi = new TessBaseAPI();
    	baseApi.init("/mnt/sdcard/tesseract/", "deu");// myDir + "/tessdata/eng.traineddata" must be present
    	
    	//baseApi.setImage(new File("/mnt/sdcard/images/testthreshhold.jpg"));
    	baseApi.setImage(_bitmap);
    	String recognizedText = baseApi.getUTF8Text(); // Log or otherwise display this string...
    	//_image.setImageBitmap(_bitmap);
    	
    	_field.setText("dkdp");
    	generateNoteOnSD("/rec.txt",recognizedText);
    	recognizedText=alph(recognizedText);
    	long id=mDbHelper.createNote(now(), prices(recognizedText));
    	baseApi.end();
    	//_field.setVisibility( View.GONE );
    	
    }
    public void showpurchases(View view){
    	Intent i = new Intent(this, Notepadv3.class);
        startActivityForResult(i, 0);
    }
    public String alph(String sample){
    	String result="";
    	Pattern s = Pattern.compile("([a-zA-Z])5");
    	Pattern o = Pattern.compile("([a-zA-Z])0");
    	Pattern l = Pattern.compile("([a-zA-Z])1");
    	Pattern b = Pattern.compile("([a-zA-Z])8");
    	Pattern trim=Pattern.compile("(.),(.)");
    	Matcher Trim=trim.matcher(sample);
    	sample=Trim.replaceAll("$1.|>$2");
    	
    	Matcher S=s.matcher(sample);
    	sample=S.replaceAll("$1s");
    	Matcher O=o.matcher(sample);
    	sample=O.replaceAll("$1o");
    	Matcher B=b.matcher(sample);
    	sample=B.replaceAll("$1B");
    	Matcher L=l.matcher(sample);
    	sample=L.replaceAll("$1l");
    	return sample;
    }
        public String prices(String sample){
    	String result="";
    	Pattern trim;
    	Matcher Trim;
    	String[] lines=sample.split("\n");
    	for(String line : lines){
    		if(line.matches(".*\\s+([1-9][0-9]*|0)\\D([0-9]{2})\\s+.*")){
    			trim=Pattern.compile("^.*?([A-Z].*\\d).*");
    	    	Trim=trim.matcher(line);
    	    	line=Trim.replaceAll("$1");
    	    	trim=Pattern.compile("(.*\\s+)([1-9]?[0-9][.,<][0-9]{2})(.*)");
    	    	Trim=trim.matcher(line);
    	    	line=Trim.replaceAll("<product>$1</product><price>$2</price>$3");
    	    	
    	    	
    			result+=line+"\n";
    		}
    	}
      	return result;
    }
    public String date(String sample){
    	String result="";
    	String[] lines=sample.split("\n");
    	for(String line : lines){
    		if(line.contains("Datum")){
    			String[] lines2=line.split("\\s");
    			for(String line2 : lines){
    				if(line2.contains("Datum"))
    				return line2;
    			}
    		}
    	}
      	return result;
    }
    @Override 
    protected void onRestoreInstanceState( Bundle savedInstanceState){
    	Log.i( "MakeMachine", "onRestoreInstanceState()");
    	if( savedInstanceState.getBoolean( PhotoCaptureExample.PHOTO_TAKEN ) ) {
    		onPhotoTaken();
    	}
    }
    
    @Override
    protected void onSaveInstanceState( Bundle outState ) {
    	outState.putBoolean( PhotoCaptureExample.PHOTO_TAKEN, _taken );
    }
}