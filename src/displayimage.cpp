#include <displayimage.hpp>

#include <cstdio>
 
void DisplayImage::Open(const std::string& path)
{
    cv::Mat image = cv::imread(path, cv::IMREAD_COLOR);

    if(!image.data)
    {
        printf("No image data\n");
        return;
    }

    cv::namedWindow("Display Image", cv::WINDOW_AUTOSIZE);
    cv::imshow("Display Image", image);

    cv::waitKey(0);
}