#include "gnuplot-iostream.h"
#include <sys/time.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <vector>
#include <termios.h>
#include <stdio.h>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <cmath>

using namespace std;
void printBits(size_t const size, void const * const ptr);
Gnuplot gnp;
std::vector<std::pair<float, float> > xy_pts_A;
float y;
float tim;
int data;
clock_t t;

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
void serial_send(int serial_fd, char *data, int size)
{
  write(serial_fd, data, size);
}

int serial_read(int serial_fd, int timeout_usec)
{
      fd_set fds;
      struct timeval timeout;
      int count=0;
      int ret;
      int n;
      do {
        FD_ZERO(&fds);
        FD_SET (serial_fd, &fds);
        timeout.tv_sec = 1;
        timeout.tv_usec = timeout_usec;
        ret=select (FD_SETSIZE,&fds, NULL, NULL,&timeout);
        if (ret==1) {//control overflow del buffer
           // int  read(  int  handle,  void  *buffer,  int  nbyte );
          n=read (serial_fd, &data, sizeof(short int));//lee linea a linea el caracter de 16 bits
          t = clock()-t;
	  //printBits(sizeof(data[count]), &data[count]);
          y = (data-2047)>0 ? (data-2047)/2048.00 : (data-2047)/2047.00;
          tim = float(t)/CLOCKS_PER_SEC;
	  cout<<"leo un: "<<y<<endl;
	  xy_pts_A.push_back(std::make_pair(tim,y));
          gnp << "set xrange [0:"<<tim<<"]\n"; //pasando comandos al gnuplot object
          gnp << "plot" << gnp.file1d(xy_pts_A) << "with lines title 'signal'"<<std::endl;
   	}
 } while (ret==1);
 return count;
}


int main(int argc, char *argv[])
{
    
   int serial_fd, n, longitud;
   char *device="/dev/ttyACM0";
   gnp << "set xlabel 'Tiempo'\n";
   gnp << "set ylabel 'Volts (%)'\n";
   gnp << "set yrange [-1.5:1.5]\n";
   serial_fd = serial_open("/dev/ttyACM0",115200);
    if (serial_fd == -1) {
            printf ("Error opening the serial device: %s\n",argv[1]);
            perror("OPEN");
            return -1;
    }
    printf("SERIAL OPEN:%s\n", device);
    t = clock();
    n=serial_read(serial_fd, 1000000000);
    printf("Se ha recibido %s \n Tamaño: %d\n n:%d\nserial_fd:%d\n",data,longitud,n,serial_fd);
    serial_close(serial_fd);
    return 0;
}
