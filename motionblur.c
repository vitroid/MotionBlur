#include <stdio.h>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <opencv/cxcore.h>


void accumulate( IplImage* accum, IplImage* img, float weight )
{
  int width = accum->width;
  int height = accum->height;
  int i,j;
  for(i=0;i<height;i++){
    for(j=0;j<width;j++){
      ((float *)(accum->imageData + i*accum->widthStep))[j*accum->nChannels + 0] 
        += weight * ((uchar *)(img->imageData + i*img->widthStep))[j*img->nChannels + 0];
      ((float *)(accum->imageData + i*accum->widthStep))[j*accum->nChannels + 1] 
        += weight * ((uchar *)(img->imageData + i*img->widthStep))[j*img->nChannels + 1];
      ((float *)(accum->imageData + i*accum->widthStep))[j*accum->nChannels + 2] 
        += weight * ((uchar *)(img->imageData + i*img->widthStep))[j*img->nChannels + 2];
      ((float *)(accum->imageData + i*accum->widthStep))[j*accum->nChannels + 3]  
        += weight;
    }
  }
}



IplImage* average( IplImage* accum )
{
  int width = accum->width;
  int height = accum->height;
  IplImage* img=cvCreateImage(cvSize(width,height),IPL_DEPTH_8U,3);

  int i,j;
  for(i=0;i<height;i++){
    for(j=0;j<width;j++){
      float alpha = ((float *)(accum->imageData + i*accum->widthStep))[j*accum->nChannels + 3];
      if ( alpha > 0.0 ){
        ((uchar *)(img->imageData + i*img->widthStep))[j*img->nChannels + 0] = 
          ((float *)(accum->imageData + i*accum->widthStep))[j*accum->nChannels + 0] / alpha;
        ((uchar *)(img->imageData + i*img->widthStep))[j*img->nChannels + 1] = 
          ((float *)(accum->imageData + i*accum->widthStep))[j*accum->nChannels + 1] / alpha;
        ((uchar *)(img->imageData + i*img->widthStep))[j*img->nChannels + 2] = 
          ((float *)(accum->imageData + i*accum->widthStep))[j*accum->nChannels + 2] / alpha;
      }
    }
  }
  return img;
}




int main(int argc, char* argv[] )
{
  IplImage* imgs[argc];
  int base=2;
  int head=base;
  int tail=base;
  int out=0;
  if ( argc < 3 ){
    fprintf(stderr, "usage: %s window img1 img2 img3 ... \n", argv[0]);
    exit(1);
  }
  int window=atoi(argv[1]);
  char* fileName = argv[head];
  imgs[head]=cvLoadImage(fileName,1);
  IplImage* accum=cvCreateImage(cvSize(imgs[head]->width,imgs[head]->height),IPL_DEPTH_32F,4);
  accumulate( accum, imgs[head], 1.0 );
  while( tail < argc ){
    //Save averaged image
    IplImage* avg = average( accum );
    char buf[1000];
    sprintf(buf, "blur%05d.jpg", out);
    fprintf(stderr, "%s\n", buf);
    out ++;
    cvSaveImage(buf,avg,0);
    cvReleaseImage(&avg);
    //Move window
    if ( head < argc-1 ){
      head ++;
      //accumulate
      char* fileName = argv[head];
      imgs[head]=cvLoadImage(fileName,1);
      accumulate( accum, imgs[head], 1.0 );
    }
    if ( head >= base+window ){
      //de-accumulate
      accumulate( accum, imgs[tail], -1.0 );
      cvReleaseImage(&imgs[tail]);
      tail ++;
    }

  }
}

  
