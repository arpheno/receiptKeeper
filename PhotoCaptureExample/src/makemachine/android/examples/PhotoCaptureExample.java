package makemachine.android.examples;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;


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

import com.googlecode.tesseract.android.TessBaseAPI;


public class PhotoCaptureExample extends Activity 
{
	protected Button _button;
	protected Button _button1;
	protected ImageView _image;
	protected TextView _field;
	protected String _path;
	protected boolean _taken;
	protected static final String PHOTO_TAKEN	= "photo_taken";
	private Bitmap _bitmap;	
    @Override
    public void onCreate(Bundle savedInstanceState) 
    {
        super.onCreate(savedInstanceState);
        
        setContentView(R.layout.main);
        _field = ( TextView ) findViewById( R.id.field );
        _button = ( Button ) findViewById( R.id.button );
        _button1 = ( Button ) findViewById( R.id.button1 );
        _button.setOnClickListener( new ButtonClickHandler() );
        _button1.setOnClickListener( new ButtonClickHandler() );
        _path = Environment.getExternalStorageDirectory() + "/images/make_machine_example.jpg";
        
    	
    }
    
    public class ButtonClickHandler implements View.OnClickListener 
    {
    	public void onClick( View view ){
    		Log.i("MakeMachine", "ButtonClickHandler.onClick()" );
    		int k=Nati.fakulty(3);
            Log.i( "MakeMachine", "hellowrold"+String.valueOf(k) );
    		startCameraActivity();
    	}
    }
    public void toGrayscale(int t)
    {        
    	ExifInterface ex;
        BitmapFactory.Options options = new BitmapFactory.Options();
        options.inSampleSize = 1;
        options.inPurgeable = true;
        for(int i=4;i<5;i++){
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
    protected void startCameraActivity()
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
    	//_image.setImageBitmap(bitmap);
    	_field.setText(recognizedText);
    	baseApi.end();
    	//_field.setVisibility( View.GONE );
    	
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