#include <iostream>
#include<cstdio>
using namespace std;
unsigned char* readBMP(char* filename)
{
    int i;
    FILE* f = fopen(filename, "rb");
    unsigned char info[54];
    fread(info, sizeof(unsigned char), 54, f); // read the 54-byte header

    // extract image height and width from header
    int width = *(int*)&info[18];
    int height = *(int*)&info[22];
    int size = 3 * width * height;
    unsigned char *data1 = new unsigned char[size]; // allocate 3 bytes per pixel
    fread(data1, sizeof(unsigned char), size, f); // read the rest of the data at once
    fclose(f);

    printf("static uint8_t imagen_tabla[]={\n");
    for(i = 0; i < size; i++)
    {
	printf("%u, \n", data1[i]);
    }
    printf("};");
    return data1;
}
int main(void){
  unsigned char *data = readBMP("girl.bmp"); 
 return 0;
}
