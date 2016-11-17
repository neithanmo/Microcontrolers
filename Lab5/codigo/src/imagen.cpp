#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "iostream"
 
using namespace cv;
using namespace std;

unsigned int RGB565(int R,int G,int B)///color
{                                              
  return ((R >> 3) << 11) | ((G >> 2) << 5) | (B >> 3);
}

int main( )
{
 
    Mat src1;
    src1 = imread("girl.bmp", CV_LOAD_IMAGE_COLOR); 
    namedWindow( "Original image", CV_WINDOW_AUTOSIZE ); 
    imshow( "Original image", src1 ); 
 
    cout << "static uint16_t imagen2_tabla[]={" <<endl;
    // 3 channel image with BGR color (type 8UC3)
    // the values can be stored in "int" or in "uchar". Here int is used.
    for(int j = 0;j<160;j++){
    for(int i = 0; i<128;i++){
    Vec3b intensity2 = src1.at<Vec3b>(j,i);    
    int blue = intensity2.val[0];
    int green = intensity2.val[1];
    int red = intensity2.val[2];
    //cout << red << "," << endl;
    //cout << green << "," << endl;
    //cout << blue << "," << endl;
    cout << RGB565(red,green,blue) << "," << endl;
    }
    }
    cout << "};" <<endl;
   
    waitKey(0);                                         
    return 0;
} 
