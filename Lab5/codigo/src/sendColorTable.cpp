#include <sys/time.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <termios.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <cmath>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <fstream>

using namespace std;
using namespace cv;

int serial_fd;
bool new_image;
short image_buffer[128*160];
const string filename;


unsigned int RGB565(int R,int G,int B)///color
{                                              
  return ((R >> 3) << 11) | ((G >> 2) << 5) | (B >> 3);
}


void open_image(const string filename){
    cout<<"abriendo imagen:"<<filename<<endl;
    Mat src1;
    src1 = imread(filename, CV_LOAD_IMAGE_COLOR);
    cout<<"width "<<src1.cols<<"heigth "<<src1.rows<<endl; 
    for(int j = 0;j<src1.rows;j++){
    	for(int i = 0; i<src1.cols;i++){
    		Vec3b intensity2 = src1.at<Vec3b>(j,i);    
    		int blue = intensity2.val[0];
    		int green = intensity2.val[1];
    		int red = intensity2.val[2];
		image_buffer[i*j]= RGB565(red,green,blue);
    	}
    }
    namedWindow( "Original image", CV_WINDOW_AUTOSIZE );
    imshow( "Original image", src1 );

}

void image_send()
{
	 short color;
	 int j;
	 for(j=0;j<20480;j++){
	  color = image_buffer[j];
	  write(serial_fd, &color, sizeof(short));
	}
}


int serial_open(char *serial_name, speed_t baud)
{
      struct termios newtermios;
      int fd;
      fd = open(serial_name,O_RDWR | O_NOCTTY);
      newtermios.c_cflag= CBAUD | CS8 | CLOCAL | CREAD;
      newtermios.c_iflag=IGNPAR;
      newtermios.c_oflag=0;
      newtermios.c_lflag=0;
      newtermios.c_cc[VMIN]=1;
      newtermios.c_cc[VTIME]=0;
      cfsetospeed(&newtermios,baud);
      cfsetispeed(&newtermios,baud);
      if (tcflush(fd,TCIFLUSH)==-1) return -1;
      if (tcflush(fd,TCOFLUSH)==-1) return -1;
      if (tcsetattr(fd,TCSANOW,&newtermios)==-1) return -1;
      return fd;
}


int main(int argc, char *argv[])
{

    int n, longitud;
    open_image(argv[1]);
    serial_fd=serial_open("/dev/ttyACM0", 9600);
    if(serial_fd == -1) return -1;
    image_send();
    //waitKey(0);
    printf("Bye.\n");
    close(serial_fd);
    return 0;
}
