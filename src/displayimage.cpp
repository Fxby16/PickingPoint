#include <displayimage.hpp>

#include <cstdio>
 
void DisplayImage::Open(const std::string& path)
{
    m_Image = cv::imread(path, cv::IMREAD_COLOR);

    if(!m_Image.data)
    {
        printf("No image data\n");
        return;
    }

    cv::namedWindow("Display Image", cv::WINDOW_AUTOSIZE);
    cv::imshow("Display Image", m_Image);

    cv::waitKey(0);
}