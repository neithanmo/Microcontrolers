#include <sys/time.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <cmath>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <fstream>

using namespace std;
using namespace cv;

const string filename;


unsigned int RGB565(int R,int G,int B)///color
{                                              
  return ((R >> 3) << 11) | ((G >> 2) << 5) | (B >> 3);
}


void open_image(const string filename){
    ofstream color("color_table.h");
    cout<<"abriendo imagen:"<<filename<<endl;
    Mat src1;
    src1 = imread(filename, CV_LOAD_IMAGE_COLOR); 
    color<<"static uint16_t image_tabla[]={" <<endl;
    for(int j = 0;j<src1.rows;j++){
    	for(int i = 0; i<src1.cols;i++){
    		Vec3b intensity2 = src1.at<Vec3b>(j,i);    
    		int blue = intensity2.val[0];
    		int green = intensity2.val[1];
    		int red = intensity2.val[2];
		color << RGB565(red,green,blue) << "," << endl;
    	}
    }
    color << "};" <<endl; 
    color.close();
    namedWindow( "Original image", CV_WINDOW_AUTOSIZE );
    imshow( "Original image", src1 );

}


int main(int argc, char *argv[])
{
    open_image(argv[1]);
    waitKey(0);
    printf("Bye.\n");
    return 0;
}
