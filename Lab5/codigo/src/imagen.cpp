#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "iostream"
 
using namespace cv;
using namespace std;
 
int main( )
{
 
    Mat src1;
    src1 = imread("girl.bmp", CV_LOAD_IMAGE_COLOR); 
    namedWindow( "Original image", CV_WINDOW_AUTOSIZE ); 
    imshow( "Original image", src1 ); 
 
    Mat gray;
    cvtColor(src1, gray, CV_BGR2GRAY);
    namedWindow( "Gray image", CV_WINDOW_AUTOSIZE );  
    imshow( "Gray image", gray );
 
    // know the number of channels the image has
    //cout<<"original image channels: "<<src1.channels()<<"gray image channels: "<<gray.channels()<<endl; 
 
// ******************* READ the Pixel intensity *********************
    // single channel grey scale image (type 8UC1) and pixel coordinates x=5 and y=2
    // by convention, {row number = y} and {column number = x}
    // intensity.val[0] contains a value from 0 to 255
    Scalar intensity1 = gray.at<uchar>(2, 5);
    //cout << "Intensity = " << endl << " " << intensity1.val[0] << endl << endl;"static uint16_t imagen_tabla[]={\n"
    cout << "static uint8_t imagen2_tabla[]={" <<endl;
    // 3 channel image with BGR color (type 8UC3)
    // the values can be stored in "int" or in "uchar". Here int is used.
    for(int j = 0;j<160;j++){
    for(int i = 0; i<128;i++){
    Vec3b intensity2 = src1.at<Vec3b>(j,i);    
    int blue = intensity2.val[0];
    int green = intensity2.val[1];
    int red = intensity2.val[2];
    cout << red << "," << endl;
    cout << green << "," << endl;
    cout << blue << "," << endl;
    }
    }
    cout << "};" <<endl;
 
// ******************* WRITE to Pixel intensity **********************
    // This is an example in OpenCV 2.4.6.0 documentation 
    Mat H(10, 10, CV_64F);
    for(int i = 0; i < H.rows; i++)
        for(int j = 0; j < H.cols; j++)
            H.at<double>(i,j)=1./(i+j+1);
    //cout<<H<<endl<<endl;
 
    // Modify the pixels of the BGR image
    for (int i=100; i<src1.rows; i++)
    {
        for (int j=100; j<src1.cols; j++)
        {
                src1.at<Vec3b>(i,j)[0] = 0;
                src1.at<Vec3b>(i,j)[1] = 200;
                src1.at<Vec3b>(i,j)[2] = 0;            
        }
    }
    namedWindow( "Modify pixel", CV_WINDOW_AUTOSIZE );  
    imshow( "Modify pixel", src1 );
 
    waitKey(0);                                         
    return 0;
} 
