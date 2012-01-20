package makemachine.android.examples;

import android.graphics.Bitmap;

public class Nati{
public native static int fakulty(int a);
public native static void greyscale(Bitmap a,int wind);
public native static void getrot(Bitmap a);
static{
	System.loadLibrary("test");
}
}
