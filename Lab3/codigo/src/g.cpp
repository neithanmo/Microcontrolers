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
#include <ctime>
#include <cmath>

using namespace std;
void printBits(size_t const size, void const * const ptr);
Gnuplot gnp;
std::vector<std::pair<int, int> > xy_pts_A;
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

int serial_read(int serial_fd, char *data, int size, int timeout_usec)
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
        if (ret==1 && count <= size) {//control overflow del buffer
           // int  read(  int  handle,  void  *buffer,  int  nbyte );
          n=read (serial_fd, &data[count], 1);//lee linea a linea el caracter de 8 bits
	  int x = (data[count] - '0');//resto 48 para obtener el entero original

	  printBits(sizeof(data[count]), &data[count]);
	  printf("char:%c  entero:%u \n", data[count], x);
	  printBits(sizeof(x), &x);
	  printf("\n");
          count+=n;
          data[count]=0;
	  xy_pts_A.push_back(std::make_pair(count,x));
          gnp << "set xlabel 'Tiempo'\n";
          gnp << "set ylabel 'Volts (%)'\n";
          gnp << "set xrange [0:"<<count<<"]\nset yrange [-5:5]\n"; //pasando comandos al gnuplot object
          gnp << "plot" << gnp.file1d(xy_pts_A) << "with lines title 'Volts'"<<std::endl;
   	}
	else
	  count=0;
 } while (ret==1);
 return count;
}

void serial_close(int fd)
{
   close(fd);
}


void printBits(size_t const size, void const * const ptr)
{
    unsigned char *b = (unsigned char*) ptr;
    unsigned char byte;
    int i, j;

    for (i=size-1;i>=0;i--)
    {
        for (j=7;j>=0;j--)
        {
            byte = (b[i] >> j) & 1;
            printf("%u", byte);
        }
    }
    puts("");
    
}

int main(int argc, char *argv[])
{
    
   //vetores para graficar resultados
   int serial_fd, n, longitud;
   char *device="/dev/ttyACM0";
   char data[256];//buffer de 256 bytes
   //longitud=strlen(device);
   // char dt = 2+'0';
    //int num = int(dt) - 48;
    //printf("caracter:%c y el entero es %d \n", dt, num);
    //cout<<"caracter"<<dt<< " el entero es"<<num<<"\n";
   serial_fd = serial_open("/dev/ttyACM0",115200);
    if (serial_fd == -1) {
            printf ("Error opening the serial device: %s\n",argv[1]);
            perror("OPEN");
            return -1;
    }
    printf("SERIAL OPEN:%s\n", device);
    //serial_send(serial_fd, device, longitud);
    //printf ("String sent------> %s\n",device);
    n=serial_read(serial_fd, data, sizeof(data), 10000000);
    printf("Se ha recibido %s \n Tamaño: %d\n n:%d \n serial_fd:%d\n",data,             longitud,n,serial_fd);
     puts(data);
    serial_close(serial_fd);
    return 0;
}
