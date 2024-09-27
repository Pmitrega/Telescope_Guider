#include <opencv4/opencv2/core.hpp>
#include <opencv4/opencv2/imgcodecs.hpp>
#include <opencv4/opencv2/opencv.hpp>
#include <opencv4/opencv2/highgui.hpp>

#include <string>
#include <iostream>
#include <fstream>

int main(int argc, char* argv[])
{
    if(argc == 2){
        std::string s(argv[1]);
        if(s == "-h"){
            std::cout << "----HELP----" <<std::endl;
            std::cout << "{width} {height} {input} {output} - to convert RAW16 to tiff with the same name" <<std::endl;
        }
        return 0;
    }
    char *buffer;

    if(argc == 5){
        std::string width_s(argv[1]);
        std::string height_s(argv[2]);
        std::string filename_i(argv[3]);
        std::string filename_o(argv[4]);
        int width = std::stoi(width_s);
        int heigth = std::stoi(height_s);

        std::ifstream fin;
        fin.open(filename_i, std::ios::binary | std::ios::in);
        if(!fin.is_open()){
            std::cout << "Can't open file" << std::endl;
            return -1;
        }
        buffer = new char[width * heigth * 2];
        fin.read(buffer, width * heigth * 2);
        fin.close();

        cv::Mat grayscale(heigth, width, CV_16U, reinterpret_cast<uint16_t*>(buffer));

        cv::imwrite(filename_o, grayscale);

    }

    delete buffer;
    return 0;
}