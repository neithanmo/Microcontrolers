#include <sys/time.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <termios.h>
#include <stdio.h>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <cmath>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>

using namespace std;
using namespace cv;

int serial_fd;
bool new_image;
unsigned short int image_buffer[128*160];
const string filename;


unsigned int RGB565(int R,int G,int B)///color
{                                              
  return ((R >> 3) << 11) | ((G >> 2) << 5) | (B >> 3);
}


void open_image(const string filename){
    cout<<"abriendo imagen:"<<filename<<endl;
    Mat src1;
    src1 = imread(filename, CV_LOAD_IMAGE_COLOR); 
    for(int j = 0;j<src1.rows;j++){
    	for(int i = 0; i<src1.cols;i++){
    		Vec3b intensity2 = src1.at<Vec3b>(j,i);    
    		int blue = intensity2.val[0];
    		int green = intensity2.val[1];
    		int red = intensity2.val[2];
    		image_buffer[j*i]=RGB565(red,green,blue);
		//cout<<"image_buffer"<<image_buffer[i*j]<<endl;
    	}
    }
    namedWindow( "Original image", CV_WINDOW_AUTOSIZE );
    imshow( "Original image", src1 );

}

void image_send()
{
	 write(serial_fd, &image_buffer, sizeof(image_buffer));
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
   new_image = false;
    
   int n, longitud;
   char *device="/dev/ttyACM0";
   serial_fd = serial_open("/dev/ttyACM0", 115200);
    if (serial_fd == -1) {
            printf ("Error opening the serial device: %s\n",argv[1]);
            perror("OPEN");
           return -1;
    }
    open_image(argv[1]);
    image_send();
    waitKey(0);
    printf("Bye.\n");
    close(serial_fd);
    return 0;
}
