#include <rotationFinder.hpp>
#include <cstdio>
#include <vector>
#include <cmath>
 
void RotationFinder::Start(const std::string& path)
{
    m_Image = cv::imread(path, cv::IMREAD_COLOR);

    if(m_Image.empty())
    {
        perror("No image data\n");
        return;
    }

    // Convert image to grayscale
    cv::cvtColor(m_Image, m_Gray, cv::COLOR_BGR2GRAY);

    // Convert image to binary
    cv::threshold(m_Gray, m_Binary, 50, 255, cv::THRESH_BINARY | cv::THRESH_OTSU);

    // Find all the contours in the thresholded image
    std::vector<std::vector<cv::Point>> contours;
    std::vector<cv::Vec4i> hierarchy;

    cv::findContours(m_Binary, contours, hierarchy, cv::RETR_LIST, cv::CHAIN_APPROX_NONE);

    for(size_t i = 0; i < contours.size(); i++)
    {
        // Calculate the area of each contour
        double area = contourArea(contours[i]);

        // Get the rotated bounding box
        cv::RotatedRect rect = cv::minAreaRect(contours[i]);
        cv::Point2f box[4];
        rect.points(box);

        // Retrieve the key parameters of the rotated bounding box
        cv::Point2f center = rect.center;
        int width = rect.size.width;
        int height = rect.size.height;
        float angle = rect.angle;

        if(width < height)
        {
            angle = 90 - angle;
        }
        else
        {
            angle = -angle;
        }

        // Draw the rectangle and label on the image
        cv::Point2f pt[4];
        rect.points(pt);
        for (int j = 0; j < 4; j++)
            cv::line(m_Image, pt[j], pt[(j + 1) % 4], cv::Scalar(0, 0, 255), 2, cv::LINE_AA);

        cv::circle(m_Image, center, 10, cv::Scalar(0, 255, 0), -1);

        angle += 90;

        cv::circle(m_Image, cv::Point(cv::Vec2i(center.x + 30 * std::cos(angle * CV_PI / 180.0f), center.y + 30 * std::sin(angle * CV_PI / 180.0f))), 10, cv::Scalar(255, 0, 0), -1);

        printf("Rotation angle %.2f\n", angle);
    }

    cv::imshow("Output Image", m_Image);
    cv::imwrite("min_area_rec_output.jpg", m_Image);

    cv::waitKey(0);
    cv::destroyAllWindows();
}