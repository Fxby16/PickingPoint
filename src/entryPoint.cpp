#include <pickingPoint.hpp>
#include <timer.hpp>
#include <filesystem>
#include <vector>
#include <unordered_map>

inline const char* GetClassName(unsigned int class_id)
{
    switch(class_id){
        case 7:
            return "Chiave";
        case 3:
            return "Vite";
        case 2:
            return "Dado";
        case 1:
            return "Ugello";
        case 0:
            return "Tubo";
        default:
            return "Unknown";
    }
}

int main(int argc, char** argv)
{
    std::string mask_path = "assets/masks";

    std::map<unsigned int, std::vector<double>> object_class_times;

    for(const auto& mask_entry : std::filesystem::directory_iterator(mask_path))
    {
        //printf("Opening RGB Image: %s\n", (mask_entry.path().string().replace(mask_entry.path().string().find("masks"), 5, "depth") + ".jpg").c_str());
        cv::Mat colorImage = cv::imread(mask_entry.path().string().replace(mask_entry.path().string().find("masks"), 5, "depth") + ".jpg", cv::IMREAD_COLOR);
        cv::Mat rgbImage = cv::imread(mask_entry.path().string().replace(mask_entry.path().string().find("masks"), 5, "depth") + ".exr", cv::IMREAD_UNCHANGED);

        // Finding the min depth for normalization
        unsigned int min_depth = std::numeric_limits<unsigned int>::max(), max_depth = 0;

        for(int i = 0; i < rgbImage.rows; i++)
        {
            for(int j = 0; j < rgbImage.cols; j++)
            {
                if(rgbImage.at<cv::Vec3f>(i, j)[2] == 0)
                {
                    continue;
                }

                min_depth = std::min(min_depth, std::max((unsigned int) rgbImage.at<cv::Vec3f>(i, j)[2], (unsigned int) 300));
                max_depth = std::max(max_depth, (unsigned int) rgbImage.at<cv::Vec3f>(i, j)[2]);
            }
        }

        //printf("Min Depth: %u Max Depth: %u\n", min_depth, max_depth);
 
        // Normalizing the depth map
        cv::Mat rgbImageNormalized = cv::Mat::zeros(rgbImage.size(), rgbImage.type());

        for(int i = 0; i < rgbImage.rows; i++)
        {
            for(int j = 0; j < rgbImage.cols; j++)
            {
                if(rgbImage.at<cv::Vec3f>(i, j)[2] == 0)
                {
                    continue;
                }

                float val = std::max(0.0, (rgbImage.at<cv::Vec3f>(i, j)[2] - min_depth) / (max_depth - min_depth) * 255.0);
                rgbImageNormalized.at<cv::Vec3f>(i, j) = cv::Vec3f(val, val, val);
            }
        }

        // Converting the normalized map to a 8-bit image, and applying a rainbow colormap
        cv::Mat Depth_Converted;
        rgbImageNormalized.convertTo(Depth_Converted, CV_8UC1);

        cv::Mat Output;
        cv::applyColorMap(Depth_Converted, Output, cv::COLORMAP_JET);

        std::vector<std::pair<cv::Point, double>> points;

        for(const auto& entry : std::filesystem::directory_iterator(mask_entry.path()))
        {
            printf("Path: %s\n", entry.path().c_str());

            std::string tmp = entry.path().string().replace(entry.path().string().find("masks"), 5, "depth_masked");
            std::string tmp2 = tmp.replace(tmp.find_last_of("."), 4, ".exr");

            std::string output_folder = "output/" + mask_entry.path().filename().string();

            std::filesystem::create_directory(output_folder);

            Timer timer;

            PickingPoint pickingPoint;
            points.push_back(pickingPoint.Process(entry.path(), tmp2, output_folder));

            unsigned int object_class = std::stoi(tmp2.substr(tmp2.find_last_of('_') + 1, tmp2.find_last_of('_') - tmp2.find_last_of('.')));
        
            object_class_times[object_class].push_back(timer.ElapsedMillis());
        }

        std::sort(points.begin(), points.end(), [](const auto& p1, const auto& p2) {
            return p1.second < p2.second;
        });

        cv::circle(colorImage, points[0].first, 3, cv::Scalar(0, 255, 0), -1);
        cv::putText(colorImage, std::to_string((int) points[0].second) + " " + "0", cv::Point(points[0].first.x + 5, points[0].first.y + 5), cv::FONT_HERSHEY_SIMPLEX, 0.3, cv::Scalar(255, 255, 255), 1);

        //printf("Depths: %lf ", points[0].second);

        for(int k = 1; k < points.size(); k++)
        {
            cv::circle(colorImage, points[k].first, 3, cv::Scalar(0, 0, 255), -1);
            cv::putText(colorImage, std::to_string((int) points[k].second) + " " + std::to_string(k), cv::Point(points[k].first.x + 5, points[k].first.y + 5), cv::FONT_HERSHEY_SIMPLEX, 0.3, cv::Scalar(255, 255, 255), 1);
            //printf("%lf ", points[k].second);
        }

        //printf("\n");

        //printf("Saving image: output/%s\n", (mask_entry.path().filename().string() + ".png").c_str());

        cv::imwrite("output/" + mask_entry.path().filename().string() + ".png", colorImage);
    }

    for(const auto& [object_class, times] : object_class_times)
    {
        double sum = 0.0;
        double min_time = std::numeric_limits<double>::max();
        double max_time = 0.0;

        for(const auto& time : times)
        {
            sum += time;
            min_time = std::min(min_time, time);
            max_time = std::max(max_time, time);
        }

        double avgTime = sum / (double) times.size();
        std::string preStr = std::to_string(avgTime);
        std::string rstStr;
        if (avgTime > 90) {
            rstStr = "\033[0;91m" + preStr + "\033[0m";
        } else if (avgTime > 70) {
            rstStr = "\033[0;31m" + preStr + "\033[0m";
        } else {
            rstStr = "\033[0;32m" + preStr + "\033[0m";
        }

        std::string preStr2 = std::to_string(min_time);
        std::string rstStr2;
        if (min_time > 70) {
            rstStr2 = "\033[0;91m" + preStr2 + "\033[0m";
        } else if (min_time > 50) {
            rstStr2 = "\033[0;31m" + preStr2 + "\033[0m";
        } else {
            rstStr2 = "\033[0;32m" + preStr2 + "\033[0m";
        }

        std::string preStr3 = std::to_string(max_time);
        std::string rstStr3;
        if (max_time > 110) {
            rstStr3 = "\033[0;91m" + preStr3 + "\033[0m";
        } else if (max_time > 85) {
            rstStr3 = "\033[0;31m" + preStr3 + "\033[0m";
        } else {
            rstStr3 = "\033[0;32m" + preStr3 + "\033[0m";
        }

        printf("Object of class \033[0;34m%s\033[0m has an average time of %s ms, min time of %s ms, and max time of %s ms\n", GetClassName(object_class), rstStr.c_str(), rstStr2.c_str(), rstStr3.c_str());
    }

    // Reset terminal color
    printf("\033[0m");

    return 0;
}


/*#include <opencv2/opencv.hpp>
#include <utils.hpp>

int main(int argc, char** argv) {
    std::string path = "assets/masks/paper_h_0/paper_h_0_mask_1.png"; 
    std::string tmp2 = "assets/depth_masked/paper_h_0/paper_h_0_mask_1.exr";

    cv::Mat m_Image = cv::imread(path, cv::IMREAD_COLOR);
    cv::Mat m_DepthMap = cv::imread(tmp2, cv::IMREAD_UNCHANGED);

    if(m_Image.empty())
    {
        perror("No image data\n");
        return -1;
    }

    cv::Mat gray, binary;

    // Convert image to grayscale
    cv::cvtColor(m_Image, gray, cv::COLOR_BGR2GRAY);

    // Convert image to binary
    cv::threshold(gray, binary, 50, 255, cv::THRESH_BINARY | cv::THRESH_OTSU);

    // Find all the contours in the thresholded image
    std::vector<std::vector<cv::Point>> contours;
    std::vector<cv::Vec4i> hierarchy;

    cv::findContours(binary, contours, hierarchy, cv::RETR_LIST, cv::CHAIN_APPROX_NONE);

    cv::RotatedRect rect;

    double area = contourArea(contours[0]);

    // Get the rotated bounding box
    rect = cv::minAreaRect(contours[0]);
    cv::Point2f box[4];
    rect.points(box);

    // get angle and size from the bounding box
    float angle = rect.angle;
    float requiredAngle = rect.angle;
    if (rect.size.width < rect.size.height) {
        requiredAngle += 90;
    }
    
    cv::Size rect_size = rect.size;
    
    // thanks to http://felix.abecassis.me/2011/10/opencv-rotation-deskewing/
    if (rect.angle < -45.0f) {
        angle += 90.0f;
        cv::swap(rect_size.width, rect_size.height);
    }
    // get the rotation matrix
    //M = cv::getRotationMatrix2D(rect.center, angle, 1.0);

    cv::Mat m_DepthCropped(rect_size, m_DepthMap.type());

    cv::Mat m = getRotRectImg(rect, m_DepthMap, m_DepthCropped);

    cv::Mat dst;
    revertRotation(m_DepthCropped, dst, m_Image.size(), rect, rect_size);

    cv::imwrite("test/depth.exr", m_DepthCropped);
    cv::imwrite("test/depth2.exr", dst);
}*/