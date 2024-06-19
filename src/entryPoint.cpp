/*#include <pickingPoint.hpp>
#include <timer.hpp>
#include <filesystem>

int main(int argc, char** argv)
{
    std::string path = "assets/real_dataset/gt/masks";

    for (const auto& entry : std::filesystem::directory_iterator(path)) {
        printf("Path: %s\n", entry.path().c_str());

        Timer timer;

        PickingPoint pickingPoint;
        pickingPoint.Start(entry.path());
    }

    return 0;
}*/
#include <pcl/visualization/pcl_visualizer.h>
#include <pcl/surface/gp3.h>
#include <opencv2/opencv.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <pcl/point_types.h>
#include <pcl/io/pcd_io.h>
#include <pcl/visualization/cloud_viewer.h>
#include <string>
#include <iostream>

void generatePointCloud(const cv::Mat &depthImage, pcl::PointCloud<pcl::PointXYZ>::Ptr &cloud, const cv::Mat &rgbImage = cv::Mat())
{
    // Camera intrinsic parameters
    const float fx = 525.0f; // Focal length x
    const float fy = 525.0f; // Focal length y
    const float cx = 319.5f; // Principal point x
    const float cy = 239.5f; // Principal point y

    for (int v = 0; v < depthImage.rows; ++v)
    {
        for (int u = 0; u < depthImage.cols; ++u)
        {
            float depth = depthImage.at<float>(v, u);

            if (depth > 0)
            {
                pcl::PointXYZ point;
                point.z = depth;
                point.x = (u - cx) * depth / fx;
                point.y = (depthImage.rows - v - cy) * depth / fy; // Subtract v from the height of the image

                cloud->points.push_back(point);
            }
        }
    }
    
    cloud->width = cloud->points.size();
    cloud->height = 1;
    cloud->is_dense = true;
}

int main()
{
    std::string pathToExr = "assets/depth/0053.exr"; // Replace with your EXR file path

    // Enable the necessary flag to avoid OpenCV errors
    cv::setBreakOnError(false);

    // Read the EXR image
    cv::Mat depthImage = cv::imread(pathToExr, cv::IMREAD_ANYDEPTH);

    if (depthImage.empty())
    {
        std::cerr << "Failed to load EXR image." << std::endl;
        return -1;
    }

    // Create a Point Cloud object
    pcl::PointCloud<pcl::PointXYZ>::Ptr cloud(new pcl::PointCloud<pcl::PointXYZ>);

    // Generate the point cloud from the depth image
    generatePointCloud(depthImage, cloud);

    if (cloud->points.empty()) {
        perror("Cloud is empty");
        return -1;
    }

    // Save the point cloud as a PCD file
    pcl::io::savePCDFile("point_cloud.pcd", *cloud);

    return 0;
}
