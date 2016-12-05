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

using namespace std;

int serial_fd;
bool new;
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


void nueva_imagen( void ) 
{
  char ch = '\0';
  int set_point;
  printf("para configurar un nuevo valor presione n \n");
  printf("una vez ingresado el nuevo valor y desea enviarlo a la tarjeta presione s \n");
  printf("para salir presione c \n");
  scanf("%c",&ch);
  while(ch != 'c')
  {
	if(ch == 'n'){
           printf("Desea enviar un nuevo mapa de bits a la tarjeta?(esto sobreescribira la imagen actual)\n");
           scanf("%d",&set_point);
           printf("valor deseado es %dC° \n", set_point);
        }
	else if(ch == 's'){
           printf("enviando nuevo valor deseado de %d C° al controlador ttyACM0\n", set_point);
           write(serial_fd, &set_point, 1);
        }
        else if(ch == 'c'){
           printf("Hasta luego \n");
        }
        ch = getchar();
  }
}


int main(int argc, char *argv[])
{
   new = false;
    
   int n, longitud;
   char *device="/dev/ttyACM0";
   serial_fd = serial_open("/dev/ttyACM0", 115200);
    if (serial_fd == -1) {
            printf ("Error opening the serial device: %s\n",argv[1]);
            perror("OPEN");
            return -1;
    }
    nuevo_set_point();
    printf("Bye.\n");
    close(serial_fd);
    return 0;
}
