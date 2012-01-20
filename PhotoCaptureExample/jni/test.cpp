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
#include "test.h"
uint32_t* Qwe::image=0;
uint32_t* Qwe::integral=0;
uint32_t* Qwe::mean=0;
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
      Qwe::mean= (uint32_t*)malloc(sizeof(uint32_t)*h*w);

      for(j=0;j<h*w;j++){
    	  x=j%w;
    	  y=j/w;
    	  if(!x)linecount=0;
          ret=*(*pixels+j); //Access RGB data
          grey=(int)(-Qwe::mingrey+( 0.212671*(ret&0x000000FF)+0.715160*((ret>>8)&0x000000FF)+0.072169*((ret>>16)&0x000000FF)/(Qwe::maxgrey-Qwe::mingrey)*255));//Calc Luma
          *(Qwe::image+j)=grey; //Replicate Greyscale
          /* Make Integral image */
          linecount+=grey;
          *(Qwe::integral+j)=linecount;
          if(j>=w){*(Qwe::integral+j)+=*(Qwe::integral+j-w);}
      }
}
void make_mean(int h,int w,int win){
	  int grey,ret,j,linecount,x,y;
      for(j=4*w;j<w*h;j++){
    	  x=j%w;
    	  y=j/w;
    	  if(x>win/2&&y>win/2&&y<h-win/2&&x<w-win/2){
    	  *(Qwe::mean+j)=((*(Qwe::integral+x + win/2 +w*( y + win/2)) \
					 + *(Qwe::integral+x - win/2 +w*( y - win/2)) \
					 - *(Qwe::integral+x + win/2 +w*( y - win/2)) \
					 - *(Qwe::integral+x - win/2 +w*( y + win/2)))\
					 /(win*win));
    	  }
      }
}

void imtoin(int h,int w){
	int j;
	for(j=0;j<h*w;j++)
	*(Qwe::integral+j)=*(Qwe::image+j);
}
void thresh(int h,int w){
	  int grey,ret=100,j;	  for(j=0;j<h*w;j++)
      /*(Qwe::image+j)=*(Qwe::mean+j);*/
      for(j=0;j<h*w;j++){
#ifdef DEBUG
    	  const char baseme[]="Mean is: ";
    	  const char baseim[]=" Image is: ";
    	  char filename [ 50 ];
    	  int numberme = (*Qwe::mean+j);
    	  int numberim = (*Qwe::integral+j);
    	  sprintf(filename, "%s%d%s%d", baseme, numberme,baseim,numberim);
    	  LOGI(filename);
#endif
    	  if(*(Qwe::integral+j)>=*(Qwe::mean+j))*(Qwe::image+j)=0x00FFFFFFFF;//Threshold it
    	  else *(Qwe::image+j)=0;
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
#define CONTRAST STRETCH
#ifdef CONTRAST_STRETCH
      determine_contrast(&pixels,info.height,info.width);
#else
      Qwe::mingrey=0;
      Qwe::maxgrey=255;
#endif
      greyscale(&pixels,info.height,info.width);
      make_mean(info.height,info.width,wind);
      imtoin(info.height,info.width);
      thresh(info.height,info.width);
      AndroidBitmap_unlockPixels(env, bitmap);

}

#ifdef __cplusplus
  }
#endif
