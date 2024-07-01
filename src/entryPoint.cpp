#include <pickingPoint.hpp>
#include <timer.hpp>
#include <filesystem>

int main(int argc, char** argv)
{
    std::string mask_path = "assets/masks";

    for(const auto& mask_entry : std::filesystem::directory_iterator(mask_path))
    {
        printf("Opening RGB Image: %s\n", (mask_entry.path().string().replace(mask_entry.path().string().find("masks"), 5, "depth") + ".jpg").c_str());
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

        printf("Min Depth: %u Max Depth: %u\n", min_depth, max_depth);

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
        }

        std::sort(points.begin(), points.end(), [](const auto& p1, const auto& p2) {
            return p1.second < p2.second;
        });

        cv::circle(colorImage, points[0].first, 3, cv::Scalar(0, 255, 0), -1);
        cv::putText(colorImage, std::to_string((int) points[0].second) + " " + "0", cv::Point(points[0].first.x + 5, points[0].first.y + 5), cv::FONT_HERSHEY_SIMPLEX, 0.3, cv::Scalar(255, 255, 255), 1);

        printf("Depths: %lf ", points[0].second);

        for(int k = 1; k < points.size(); k++)
        {
            cv::circle(colorImage, points[k].first, 3, cv::Scalar(0, 0, 255), -1);
            cv::putText(colorImage, std::to_string((int) points[k].second) + " " + std::to_string(k), cv::Point(points[k].first.x + 5, points[k].first.y + 5), cv::FONT_HERSHEY_SIMPLEX, 0.3, cv::Scalar(255, 255, 255), 1);
            printf("%lf ", points[k].second);
        }

        printf("\n");

        printf("Saving image: output/%s\n", (mask_entry.path().filename().string() + ".png").c_str());

        cv::imwrite("output/" + mask_entry.path().filename().string() + ".png", colorImage);
    }

    return 0;
}