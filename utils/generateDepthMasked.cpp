#include <opencv2/opencv.hpp>
#include <filesystem>
#include <string>
#include <cassert>
#include <cstdio>

int main()
{
    std::string depthPath = "../assets/depth";
    for(const auto& entry : std::filesystem::directory_iterator(depthPath))
    {
        if(entry.path().extension() != ".exr")
            continue;

        printf("Path: %s\n", entry.path().string().c_str());

        cv::Mat depth = cv::imread(entry.path().string(), cv::IMREAD_UNCHANGED);

        //FILE* file = fopen((entry.path().string().substr(0, entry.path().string().find_last_of(".")) + ".txt").c_str(), "w");

        // val : 255 = x : 25 
        // (val * 10)

        std::string masksPath = "../assets/masks/" + entry.path().filename().string().substr(0, entry.path().filename().string().find_last_of(".")) + "/";

        for(const auto& maskEntry : std::filesystem::directory_iterator(masksPath))
        {

            cv::Mat mask = cv::imread(maskEntry.path().string(), cv::IMREAD_ANYCOLOR);
            cv::Mat depth_masked = cv::Mat::zeros(depth.size(), depth.type());

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

            std::filesystem::create_directory("../assets/depth_masked/" + entry.path().filename().string().substr(0, entry.path().filename().string().find_last_of(".")));

            cv::imwrite("../assets/depth_masked/" + entry.path().filename().string().substr(0, entry.path().filename().string().find_last_of(".")) + "/" + (maskEntry.path().filename().string().substr(0, maskEntry.path().filename().string().find_last_of(".")) + ".exr"), depth_masked);
        }
    }
}