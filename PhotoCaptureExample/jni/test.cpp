/*
 * =====================================================================================
 *
 *       Filename:  test.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  01/07/2012 03:05:54 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *        Company:  
 *
 * =====================================================================================
 */
#include <stdlib.h>
#include <stdio.h>
#include <jni.h>
#include <android/log.h>
#include <android/bitmap.h>
#include <math.h>
#include "test.h"
uint32_t* Qwe::image=0;
uint32_t* Qwe::integral=0;
uint64_t* Qwe::intsquare=0;
uint8_t* Qwe::dev=0;

uint8_t* Qwe::mean=0;
uint32_t Qwe::maxgrey=0;
uint32_t Qwe::mingrey=0;
#define LOGV(...) __android_log_print(ANDROID_LOG_VERBOSE, "libnav",__VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG  , "libnav",__VA_ARGS__)
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO   , "libnav",__VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN   , "libnav",__VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR  , "libnav",__VA_ARGS__)

Qwe::Qwe()
{
       Qwe::image=0;
}

#ifdef __cplusplus
extern "C" {
#endif
    /*
  * Class:     makemachine_android_examples_Nati
   * Method:    fakulty
    * Signature: (I)I
     */
    JNIEXPORT jint JNICALL Java_makemachine_android_examples_Nati_fakulty(JNIEnv * env, jclass klass, jint i){
        if(i==1)return 1;
        else return i*Java_makemachine_android_examples_Nati_fakulty(env,klass,i-1);
    }

#ifdef __cplusplus
}
uint32_t* herp(){
	return Qwe::image;
}
void determine_contrast(uint32_t** pixels,int h,int w){
	Qwe::maxgrey=0;Qwe::mingrey=0xFFFFFFFF;
	int grey,ret,j,linecount,x,y,win=200;
	/*Determine Contrast range*/
	      for(j=0;j<h*w;j++){
	    	  double fgrey;
	    	  ret=*(*pixels+j);//Access RGB data
	    	  fgrey=( 0.212671*(ret&0x000000FF)+0.715160*((ret>>8)&0x000000FF)+0.072169*((ret>>16)&0x000000FF));//Calc Luma
	    	  if(fgrey>Qwe::maxgrey)Qwe::maxgrey=fgrey;
	    	  if(fgrey<Qwe::mingrey)Qwe::mingrey=fgrey;
	      }

}
void greyscale(uint32_t** pixels,int h,int w){
      int grey,ret,j,linecount,x,y,win=200;
      Qwe::image= (uint32_t*)malloc(sizeof(uint32_t)*h*w);
      Qwe::integral= (uint32_t*)malloc(sizeof(uint32_t)*h*w);
#ifdef USE_DEVIATION
      Qwe::intsquare= (uint64_t*)malloc(sizeof(uint64_t)*h*w);
      uint64_t linesquare;
#endif
      for(j=0;j<h*w;j++){
    	  x=j%w;
    	  y=j/w;
    	  if(!x){linecount=0;
#ifdef USE_DEVIATION
    	  linesquare=0;
#endif
    	  }
          ret=*(*pixels+j); //Access RGB data
          grey=(int)(-Qwe::mingrey+( 0.212671*(ret&0x000000FF)+0.715160*((ret>>8)&0x000000FF)+0.072169*((ret>>16)&0x000000FF)/(Qwe::maxgrey-Qwe::mingrey)*255));//Calc Luma
          //grey=(int)( 0.212671*(ret&0x000000FF)+0.715160*((ret>>8)&0x000000FF)+0.072169*((ret>>16)&0x000000FF));//Calc Luma
          Qwe::image[j]=grey; //Replicate Greyscale
                    /* Make Integral image for mean*/
          *(Qwe::integral+j)=linecount;
          linecount+=grey;
          if(j>=w){*(Qwe::integral+j)+=*(Qwe::integral+j-w);}
#ifdef USE_DEVIATION
          linesquare+=(grey*grey);
          *(Qwe::intsquare+j)=linesquare;
          if(j>=w){Qwe::intsquare[j]+=Qwe::intsquare[j-w];}
#endif
      }
}
void make_mean(int h,int w,int win){
	Qwe::mean= (uint8_t*)malloc(sizeof(uint8_t)*h*w);
	uint32_t register temp;
	  int grey,ret,j,linecount,x,y;
      for(j=4*w;j<w*h;j++){
    	  x=j%w;
    	  y=j/w;
    	  if(x>win/2&&y>win/2&&y<h-win/2&&x<w-win/2){
    			  temp=  Qwe::integral[x + win/2 +w*( y + win/2)];
				  temp+= Qwe::integral[x - win/2 +w*( y - win/2)];
				  temp-= Qwe::integral[x + win/2 +w*( y - win/2)];
				  temp-= Qwe::integral[x - win/2 +w*( y + win/2)];
				  temp/=(win*win);
				  Qwe::mean[j]=temp;
    	  }
      }
}
void dilate(int h,int w,int win){
	uint32_t tempx,tempy;
	  int grey,ret,j,linecount,x,y;
	  for(j=4*w;j<w*h;j++){
		  Qwe::integral[j]=0x00FFFFFF;
	  }
	  for(j=4*w;j<w*h;j++){
    	  x=j%w;
    	  y=j/w;
    	  if(x>win/2&&y>win/2&&y<h-win/2&&x<w-win/2&&!Qwe::image[j]){
    			  for(tempy = y - win/2;tempy< y + win/2;tempy++){
    				  for(tempx=x - win/2;tempx<x + win/2;tempx++){
    					  Qwe::integral[tempx +w*(tempy)]=0;
    				  }
    			  }
    	  }
      }
	  uint32_t* temp=Qwe::image;
	  Qwe::image=Qwe::integral;
	  Qwe::integral=temp;
}

void erode(int h,int w,int win){
	uint32_t tempx,tempy;
	  int grey,ret,j,linecount,x,y;
	  for(j=4*w;j<w*h;j++){
		  Qwe::integral[j]=0;
	  }
	  for(j=4*w;j<w*h;j++){
    	  x=j%w;
    	  y=j/w;
    	  if(x>win/2&&y>win/2&&y<h-win/2&&x<w-win/2&&Qwe::image[j]){
    			  for(tempy = y - win/2;tempy< y + win/2;tempy++){
    				  for(tempx=x - win/2;tempx<x + win/2;tempx++){
    					  Qwe::integral[tempx +w*(tempy)]=0x00FFFFFF;
    				  }
    			  }
    	  }
      }
	  uint32_t* temp=Qwe::image;
	  Qwe::image=Qwe::integral;
	  Qwe::integral=temp;
}
void close(int h,int w,int win){
	dilate(h,w,win);
	erode(h,w,win);
}
void open(int h,int w,int win){
	erode(h,w,win);
	dilate(h,w,win);

}
void make_dev(int h,int w,int win){
    Qwe::dev= (uint8_t*)malloc(sizeof(uint8_t)*h*w);
	  int grey,ret,j,linecount,x,y;
	      for(j=4*w;j<w*h;j++){
	    	  x=j%w;
	    	  y=j/w;
	    	  if(x>win/2&&y>win/2&&y<h-win/2&&x<w-win/2){
	          uint64_t temp;
	    	  temp=  Qwe::intsquare[x + win/2 +w*( y + win/2)];
				char filename [ 90 ];
	    	  temp-= Qwe::intsquare[x + win/2 +w*( y - win/2)];
	    	  temp+= Qwe::intsquare[x - win/2 +w*( y - win/2)];
	    	  temp-= Qwe::intsquare[x - win/2 +w*( y + win/2)];

	    	  temp/= (win*win);
	    	  uint32_t temp2;
	    	  temp2= temp-Qwe::mean[j]*Qwe::mean[j];
	    	  temp2=sqrt(temp);
	    	  Qwe::dev[j]=temp;
	    	  }

	      }
}
void imtoin(int h,int w){
	int j;
	for(j=0;j<h*w;j++){
	Qwe::integral[j]=Qwe::image[j];
	}
}
void thresh(int h,int w){
	  int grey,ret=100,j;
      for(j=0;j<h*w;j++){
    	  /* Maybe use the deviation*/
#ifdef USE_DEVIATION
    	  const uint32_t k=0.34;
    	  uint_32t t=Qwe::mean[j]*(1+k*(Qwe::dev[j]/128-1));
    	  if(Qwe::integral[j]>=t-15)Qwe::image[j]=0x00FFFFFFFF;//Threshold it
		  else Qwe::image[j]=0;
#else
    	  if(Qwe::integral[j]>=Qwe::mean[j]-15)Qwe::image[j]=0x00FFFFFFFF;//Threshold it
		  else Qwe::image[j]=0;
#endif

      }
}

extern "C" {
#endif
    jint JNI_OnLoad(JavaVM* vm, void* reserved) {
              return JNI_VERSION_1_6;
    }
#ifdef __cplusplus
}
extern "C"{
#endif
JNIEXPORT void JNICALL Java_makemachine_android_examples_Nati_getrot(JNIEnv * env, jclass klass, jobject bitmap){
    int ret,j;
      uint32_t * pixels;
      void * raw;
      AndroidBitmapInfo  info;
      if ((ret = AndroidBitmap_getInfo(env, bitmap, &info)) < 0) return;
      if ((ret = AndroidBitmap_lockPixels(env, bitmap, &raw)) < 0) return;
      pixels=(uint32_t*)raw;
       for(j=0;j<info.height*info.width;j++){
          *(pixels+j)=*(Qwe::image+j);
      }
     int h,w,oH,oW,nW;
     oH=info.width;
     oW=info.height;
     nW=info.width;
      for(w=0;w<oW;w++){
	      for(h=0;h<oH;h++){
		      *(pixels+nW-h-1+nW*w)=*(Qwe::image+oW*h+w);
	      }
      }
      AndroidBitmap_unlockPixels(env, bitmap);
      free(Qwe::image);
      free(Qwe::integral);
      free(Qwe::mean);
      free(Qwe::dev);
#ifdef USE_DEVIATION
      free(Qwe::intsquare);
#endif
}
#ifdef __cplusplus
}
extern "C"{
#endif
JNIEXPORT void JNICALL Java_makemachine_android_examples_Nati_greyscale(JNIEnv * env, jclass klass, jobject bitmap,jint wind){
      void * raw;
      uint32_t * pixels;
      int ret,j;
      int grey;
      AndroidBitmapInfo  info;
      if ((ret = AndroidBitmap_getInfo(env, bitmap, &info)) < 0) return;
      if ((ret = AndroidBitmap_lockPixels(env, bitmap, &raw)) < 0) return;
      pixels=(uint32_t*) raw;
#define USE_CONTRAST STRETCH
#ifdef USE_CONTRAST_STRETCH
      determine_contrast(&pixels,info.height,info.width);
#else
      Qwe::mingrey=0;
      Qwe::maxgrey=255;
#endif
      greyscale(&pixels,info.height,info.width);
      make_mean(info.height,info.width,wind);
#ifdef USE_DEVIATION
      make_dev(info.height,info.width,wind);
#endif
      imtoin(info.height,info.width);
      thresh(info.height,info.width);
      close(info.height,info.width,5);
      open(info.height,info.width,3);

      AndroidBitmap_unlockPixels(env, bitmap);

}

#ifdef __cplusplus
  }
#endif
