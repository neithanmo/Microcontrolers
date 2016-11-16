#include <fstream>
#include <iostream>
#include <string>
#include <array>
#include <vector>
#include <iterator>
#include<cstdio>

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
    printf("static uint8_t imagen_tabla[]={\n");
    for(i = 0; i < img.size(); i++)
    {
	printf("%u, \n", (unsigned char)img.at(i));
    }
    printf("};");
/*
    for (auto i = dataSize - 4; i >= 0; i -= 3)
    {
        temp = img[i];
        img[i] = img[i+2];
        img[i+2] = temp;

        std::cout << "R: " << int(img[i] & 0xff) << " G: " << int(img[i+1] & 0xff) << " B: " << int(img[i+2] & 0xff) << std::endl;
    }*/

    return img;
}

int main(void)
{
	readBMP("imagen2.bmp");

}
