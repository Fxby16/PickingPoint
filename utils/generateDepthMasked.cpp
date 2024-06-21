#include <opencv2/opencv.hpp>
#include <filesystem>
#include <string>
#include <cassert>
#include <cstdio>

int main()
{
    cv::Mat depth = cv::imread("../assets/depth/0053.exr", cv::IMREAD_UNCHANGED);

    int type = depth.type();
    int depth_ = CV_MAT_DEPTH(type);
    int channels = CV_MAT_CN(type);

    printf("Type: %d Depth: %d, Channels: %d\n", type, depth_, channels);

    for(const auto& entry : std::filesystem::directory_iterator("../assets/masks"))
    {
        if(entry.path().extension() != ".png")
            continue;

        printf("Path: %s\n", entry.path().string().c_str());

        cv::Mat mask = cv::imread(entry.path().string(), cv::IMREAD_ANYCOLOR);

        cv::Mat depth_masked = cv::Mat::zeros(depth.size(), depth.type());

        printf("Output path: %s\n",(entry.path().string().substr(0, entry.path().string().find_last_of(".")) + ".txt").c_str());

        //FILE* file = fopen((entry.path().string().substr(0, entry.path().string().find_last_of(".")) + ".txt").c_str(), "w");

        for(int i = 0; i < depth.rows; i++)
        {
            for(int j = 0; j < depth.cols; j++)
            {
                if(mask.at<cv::Vec3b>(i, j) != cv::Vec3b(0, 0, 0))
                {
                    depth_masked.at<cv::Vec3f>(i, j) = depth.at<cv::Vec3f>(i, j);
                    assert(depth_masked.at<cv::Vec3f>(i, j) == depth.at<cv::Vec3f>(i, j));
                }
                else
                {
                    depth_masked.at<cv::Vec3f>(i, j) = cv::Vec3f(0, 0, 0);
                }
            }
        }

        cv::Mat Depth_Converted;
        depth_masked.convertTo(Depth_Converted, CV_8UC1);

        cv::Mat Output;
        cv::applyColorMap(Depth_Converted, Output, cv::COLORMAP_RAINBOW);

        cv::imwrite("../assets/depth_masked/" + (entry.path().filename().string().substr(0, entry.path().filename().string().find_last_of(".")) + ".png"), Output);
    }
}