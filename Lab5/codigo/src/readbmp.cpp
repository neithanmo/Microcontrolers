#include <fstream>
#include <iostream>
#include <string>
#include <array>
#include <vector>
#include <iterator>
#include<cstdio>

/*unsigned int RGB565(unsigned char R,unsigned char G, unsigned char B)///color
{                                              
  int prueba = ((R << 11) | (G << 5) | B);
  //printf("%u,\n",prueba);
  return ((R >> 3) << 11) | ((G >> 2) << 5) | (B >> 3);
  
}*/

/*unsigned short int RGB565(unsigned char R,unsigned char G, unsigned char B)
{
    unsigned short int r = (R & 0xF8) << 11;
    unsigned short int g = (G & 0xFC) << 5;
    unsigned short int b = (B & 0xF8);
    

    int prueba = (unsigned short int) (r | g | b);
    //printf("%u,\n",prueba);	
    return (unsigned short int) (r | g | b);
}*/

unsigned short int RGB565(unsigned char R,unsigned char G, unsigned char B)///color
{                                              
  int prueba = ((R << 11) | (G << 5) | B);
  //printf("%u,\n",prueba);
  return (unsigned short int)((R << 11) | (G << 5) | B);
  
}

std::vector<char> readBMP(const std::string &file)
{
    static constexpr size_t HEADER_SIZE = 54;

    std::ifstream bmp(file, std::ios::binary);

    std::array<char, HEADER_SIZE> header;
    bmp.read(header.data(), header.size());

    auto fileSize = *reinterpret_cast<uint32_t *>(&header[2]);
    auto dataOffset = *reinterpret_cast<uint32_t *>(&header[10]);
    auto width = *reinterpret_cast<uint32_t *>(&header[18]);
    auto height = *reinterpret_cast<uint32_t *>(&header[22]);
    auto depth = *reinterpret_cast<uint16_t *>(&header[28]);
    printf("/* Info de la imagen y caracteristicas\n");
    std::cout << "fileSize: " << fileSize << std::endl;
    std::cout << "dataOffset: " << dataOffset << std::endl;
    std::cout << "width: " << width << std::endl;
    std::cout << "height: " << height << std::endl;
    std::cout << "depth: " << depth << "-bit" << std::endl;
    printf("*/ \n");

    std::vector<char> img(dataOffset - HEADER_SIZE);
    bmp.read(img.data(), img.size());

    auto dataSize = ((width * 3 + 3) & (~3)) * height;
    img.resize(dataSize);
    bmp.read(img.data(), img.size());

    char temp = 0;
    int i;
    printf("static uint16_t imagen_tabla[]={\n");
   /* for(i = 0; i < img.size(); i++)
    {
	printf("%u, \n", (unsigned char)img.at(i));
    }*/
    //printf("};");
    //printf("%u\n", dataSize);
    for (auto i = dataSize; i > 0; i -= 3)
    {
        temp = img[i];
        img[i] = img[i+2];
        img[i+2] = temp;
	//RGB565(255,0,0);
        /*std::cout <<int(img[i] & 0xff) << ",\n";
        std::cout << int(img[i+1] & 0xff) << ",\n";
        std::cout << int(img[i+2] & 0xff) << ",\n";*/
        printf("%u,\n",RGB565(img[i] & 0xff,img[i+1] & 0xff, img[i+2] & 0xff));
        //printf("%u\t", i);
        
    }
    printf("};");

    return img;
}



int main(void)
{
	readBMP("imagen2.bmp");

}
