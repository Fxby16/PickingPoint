#pragma once

#include <opencv2/opencv.hpp>

class DisplayImage
{
public:
    DisplayImage() = default;
    ~DisplayImage() = default;

    void Open(const std::string& path);

private:
    cv::Mat image;
};