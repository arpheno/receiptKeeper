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
#include <jni.h>
#include <android/log.h>
#include <android/bitmap.h>
#include "test.h"
uint32_t* Qwe::image=0;
uint32_t* Qwe::integral=0;
uint32_t* Qwe::mean=0;
uint32_t Qwe::maxgrey=0;
uint32_t Qwe::mingrey=0;
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
 /*Determine Contrast range*/

      for(j=0;j<h*w;j++){
    	  x=j%w;
    	  y=j/w;
    	  if(!x)linecount=0;
          ret=*(*pixels+j); //Access RGB data
          grey=(int)(-Qwe::mingrey+( 0.212671*(ret&0x000000FF)+0.715160*((ret>>8)&0x000000FF)+0.072169*((ret>>16)&0x000000FF)/(Qwe::maxgrey-Qwe::mingrey)*255));//Calc Luma
          *(Qwe::image+j)=grey* 0x00010101; //Replicate Greyscale
          /* Make Integral image */
          linecount+=grey;
          *(Qwe::integral+j)=linecount;
          if(j>=w){*(Qwe::integral+j)+=*(Qwe::integral+j-w);}
      }

      for(j=4*w;j<w*h;j++){
    	  x=j%w;
    	  y=j/w;
    	  if(x>100&&y>100&&y<h-100&&x<w-100){
    	  *(Qwe::mean+j)=((*(Qwe::integral+x + win/2 +w*( y + win/2)) \
    	          		         + *(Qwe::integral+x - win/2 +w*( y - win/2)) \
    	          		         - *(Qwe::integral+x + win/2 +w*( y - win/2)) \
    	          		         - *(Qwe::integral+x - win/2 +w*( y + win/2)))\
    	          		         /(win*win));
      }}
      for(j=0;j<h*w;j++)
      *(Qwe::image+j)=*(Qwe::mean+j);
      /*for(j=0;j<h*w;j++){
    	  if(*(Qwe::image+j)>=(*Qwe::mean+j))*(Qwe::image+j)=0x00FFFFFFFF;//Replicate Greyscale
    	  else *(Qwe::image+j)=0;

      }*/
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

}
#ifdef __cplusplus
}
extern "C"{
#endif
JNIEXPORT void JNICALL Java_makemachine_android_examples_Nati_greyscale(JNIEnv * env, jclass klass, jobject bitmap){
      void * raw;
      uint32_t * pixels;
      int ret,j;
      int grey;
      AndroidBitmapInfo  info;
      if ((ret = AndroidBitmap_getInfo(env, bitmap, &info)) < 0) return;
      if ((ret = AndroidBitmap_lockPixels(env, bitmap, &raw)) < 0) return;
      pixels=(uint32_t*) raw;
      determine_contrast(&pixels,info.height,info.width);
      greyscale(&pixels,info.height,info.width);
      AndroidBitmap_unlockPixels(env, bitmap);
}

#ifdef __cplusplus
  }
#endif
