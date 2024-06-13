#pragma once

#include <opencv2/opencv.hpp>

class RotationFinder
{
public:
    RotationFinder() = default;
    ~RotationFinder() = default;

    void Start(const std::string& path);

private:
    cv::Mat m_Image;
    cv::Mat m_Gray;
    cv::Mat m_Binary;
};