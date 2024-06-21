#include <pickingPoint.hpp>
#include <cstdio>
#include <vector>
#include <cmath>
#include <limits>
#include <iostream>

void PickingPoint::Process(const std::string& path, const std::string& depth_path)
{
    m_Image = cv::imread(path, cv::IMREAD_COLOR);
    m_DepthMap = cv::imread(depth_path, cv::IMREAD_UNCHANGED);

    // float max_val = 0, min_val = std::numeric_limits<float>::max();

    // //find min and max
    // for(int i = 0; i < m_DepthMap.rows; i++)
    // {
    //     for(int j = 0; j < m_DepthMap.cols; j++)
    //     {
    //         //printf("Depth: %.2f\n", m_DepthCropped.at<cv::Vec3f>(i, j)[2]);
    //         max_val = std::max(max_val, m_DepthMap.at<cv::Vec3f>(i, j)[2]);
    //         //if(m_DepthCropped.at<cv::Vec3f>(i, j)[2] > 250)
    //         //{
    //         min_val = std::min(min_val, m_DepthMap.at<cv::Vec3f>(i, j)[2]);
    //         //}
    //     }
    // }
    
    // printf("Max: %.2f Min: %.2f\n", max_val, min_val);

    // //put normalized pixels in the new image
    // for(int i = 0; i < m_DepthMap.rows; i++)
    // {
    //     for(int j = 0; j < m_DepthMap.cols; j++)
    //     {
    //         float val = (m_DepthMap.at<cv::Vec3f>(i, j)[2] - min_val) / (max_val - min_val) * 255.0;
    //         m_DepthMap.at<cv::Vec3f>(i, j) = cv::Vec3f(val, val, val);
    //     }
    // }

    // cv::imwrite("output/depth.png", m_DepthMap);
    // cv::waitKey(0);

    if(m_Image.empty())
    {
        perror("No image data\n");
        return;
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

#ifdef DRAWDEBUG
        // Draw the rectangle and label on the image
        cv::Point2f pt[4];
        rect.points(pt);
        for (int j = 0; j < 4; j++)
            cv::line(m_Image, pt[j], pt[(j + 1) % 4], cv::Scalar(0, 0, 255), 2, cv::LINE_AA);
#endif

    cv::Mat M, rotated, rotated2;

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
    M = cv::getRotationMatrix2D(rect.center, angle, 1.0);

    // perform the affine transformation
    cv::warpAffine(m_Image, rotated, M, m_Image.size(), cv::INTER_CUBIC);

    // crop the resulting image
    cv::getRectSubPix(rotated, rect_size, rect.center, m_Cropped);

    cv::warpAffine(m_DepthMap, rotated2, M, m_DepthMap.size(), cv::INTER_NEAREST);

    // crop the resulting image
    cv::getRectSubPix(rotated2, rect_size, rect.center, m_DepthCropped);

    // for(int i = 0; i < rotated2.rows; i++)
    // {
    //     for(int j = 0; j < rotated2.cols; j++)
    //     {
    //         if(rotated2.at<cv::Vec3f>(i, j)[2] < 0)
    //         {
    //             printf("kjashdkljasoyhòfòasf");
    //         }
    //     }
    // }

    unsigned int min_depth = std::numeric_limits<unsigned int>::max(), max_depth = 0;

    for(int i = 0; i < m_DepthCropped.rows; i++)
    {
        for(int j = 0; j < m_DepthCropped.cols; j++)
        {
            if(m_DepthCropped.at<cv::Vec3f>(i, j)[2] == 0)
            {
                continue;
            }

            min_depth = std::min(min_depth, (unsigned int) m_DepthCropped.at<cv::Vec3f>(i, j)[2]);
            max_depth = std::max(max_depth, (unsigned int) m_DepthCropped.at<cv::Vec3f>(i, j)[2]);
        }
    }

    printf("Min Depth: %u Max Depth: %u\n", min_depth, max_depth);

    m_DepthCroppedNormalized = cv::Mat::zeros(m_DepthCropped.size(), m_DepthCropped.type());

    for(int i = 0; i < m_DepthCropped.rows; i++)
    {
        for(int j = 0; j < m_DepthCropped.cols; j++)
        {
            if(m_DepthCropped.at<cv::Vec3f>(i, j)[2] == 0)
            {
                continue;
            }

            float val = (m_DepthCropped.at<cv::Vec3f>(i, j)[2] - min_depth) / (max_depth - min_depth) * 255.0;
            m_DepthCroppedNormalized.at<cv::Vec3f>(i, j) = cv::Vec3f(val, val, val);
        }
    }

    cv::Mat Depth_Converted;
    m_DepthCroppedNormalized.convertTo(Depth_Converted, CV_8UC1);

    cv::Mat Output;
    cv::applyColorMap(Depth_Converted, Output, cv::COLORMAP_RAINBOW);

    cv::imwrite(std::string("output/") + std::string("depth_") + path.substr(path.find_last_of("/") + 1), Output);

#ifdef DEBUG
    fprintf(stderr, "Extracting Cells...\n");
#endif

if (m_Cropped.rows == 0 || m_Cropped.cols == 0)
    return;

    ExtractCells(5, m_Cropped);

#ifdef DEBUG
    fprintf(stderr, "Extracted Cells\n");
#endif

    for(int i = 0; i < m_Cells.size(); i++)
    {
        for(int j = 0; j < m_Cells[i].size(); j++)
        {
            HandleCell(m_Cells[i][j], i, j);
        }
    }

    std::vector<PickingPoint::Cell> min_cell_list = FindMinCell(10);
    
    std::sort(min_cell_list.begin(), min_cell_list.end(), [this](const struct Cell& a, const struct Cell& b) -> bool {
        return GetMinDepth(m_Cells[a.y][a.x].second, a.y, a.x) < GetMinDepth(m_Cells[b.y][b.x].second, b.y, b.x);
    });

    for (int i = 0; i < min_cell_list.size(); i++) {
        cv::Rect r = m_Cells[min_cell_list[i].y][min_cell_list[i].x].second;

        cv::circle(m_Cropped, cv::Point(r.x + r.width / 2, r.y + r.height / 2), 2, cv::Scalar(255, 112, 255), -1);
    }

    printf("Engine Starting %d %d %d %d...\n", min_cell_list[0].x, min_cell_list[0].y, m_Cells.size(), m_Cells[min_cell_list[0].y].size());

    printf("Kaboom %d %d\n", min_cell_list[0].y, min_cell_list[0].x);

    cv::Rect r = m_Cells[min_cell_list[min_cell_list.size() - 1].y][min_cell_list[min_cell_list.size() - 1].x].second;

    printf("Yahboom\n");

#ifdef DEBUG
    fprintf(stderr, "Writing Image...\n");
#endif

    DrawHeatMap(path); 

    printf("Bum Bum %d %d\n", r.y, r.height);

    cv::Point pickPoint = cv::Point(r.x + r.width / 2, r.y + r.height / 2);

    printf("Nah!\n");

    cv::Point y0 = Raycast(pickPoint, cv::Point(0, 1)); // Sopra
    cv::Point y1 = Raycast(pickPoint, cv::Point(0, -1)); //sotto
    cv::Point x0 = Raycast(pickPoint, cv::Point(1, 0)); // destra
    cv::Point x1 = Raycast(pickPoint, cv::Point(-1, 0));    // Sinis

    printf("Useless...\n");

    cv::circle(m_Cropped, y0, 2, cv::Scalar(0, 112, 255), -1);
    cv::circle(m_Cropped, y1, 2, cv::Scalar(0, 112, 255), -1);
    cv::circle(m_Cropped, x0, 2, cv::Scalar(0, 112, 255), -1);
    cv::circle(m_Cropped, x1, 2, cv::Scalar(0, 112, 255), -1);

    printf("Ciuf Ciuf!\n");

#ifdef DRAWDEBUG
    cv::circle(m_Cropped, y0, 2, cv::Scalar(255, 0, 0), -1);
    cv::circle(m_Cropped, y1, 2, cv::Scalar(255, 0, 0), -1);
    cv::circle(m_Cropped, x0, 2, cv::Scalar(255, 0, 0), -1);
    cv::circle(m_Cropped, x1, 2, cv::Scalar(255, 0, 0), -1);
#endif

    unsigned int requiredOpening1; // opening for the shortest side
    unsigned int requiredOpening2; // opening for the longest side
    float requiredAngle1 = requiredAngle; // angle for the shortest opening
    float requiredAngle2 = requiredAngle; // angle for the longest opening

    printf("Kapuf\n");

    /*if (std::abs(y0.y - y1.y) > std::abs(x0.x - x1.x)) {
        //pickPoint = cv::Point((x0.x + x1.x) / 2, pickPoint.y);

        cv::line(m_Cropped, x0, x1, cv::Scalar(0, 255, 0), 1);
        cv::line(m_Cropped, y0, y1, cv::Scalar(0, 255, 0), 1);

        std::vector<cv::Point> points;
        unsigned int min_depth = std::numeric_limits<unsigned int>::max();

        //find points with minimum depth
        for(int i = std::min(x0.x, x1.x); i < std::max(x0.x, x1.x); i++)
        {
            if(!(i > 0 && i < m_DepthCropped.cols))
                continue;

            printf("Horizontal: %d %d\n", x0.y, i);

            if(m_DepthCropped.at<cv::Vec3f>(x0.y, i)[2] < min_depth - 3 && m_DepthCropped.at<cv::Vec3f>(x0.y, i)[2] > 100)
            {
                printf("Deleting depth %u for depth %f\n", min_depth, m_DepthCropped.at<cv::Vec3f>(x0.y, i)[2]);
                min_depth = m_DepthCropped.at<cv::Vec3f>(x0.y, i)[2];
                points.clear();
                points.push_back(cv::Point(i, x0.y));
            }else if(std::abs(m_DepthCropped.at<cv::Vec3f>(x0.y, i)[2] - min_depth) < 3)
            {
                points.push_back(cv::Point(i, x0.y));
            }
        }

        printf("Points [");

        for(auto& p : points)
        {
            printf("[%d %d], ", p.x, p.y);
        }

        printf("]\n");

        if (points.size() > 0)
            pickPoint.x = (points[0].x + points[points.size() - 1].x) / 2;

        requiredOpening1 = std::abs(x0.x - x1.x) + 6;
        requiredOpening2 = std::abs(y0.y - y1.y) + 6;
        requiredAngle2 += 90;
    } else {
        //pickPoint = cv::Point(pickPoint.x, (y0.y + y1.y) / 2);

        cv::line(m_Cropped, x0, x1, cv::Scalar(0, 0, 255), 1);
        cv::line(m_Cropped, y0, y1, cv::Scalar(0, 0, 255), 1);

        std::vector<cv::Point> points;
        unsigned int min_depth = std::numeric_limits<unsigned int>::max();

        //find points with minimum depth
        for(int i = std::min(y0.y, y1.y); i < std::max(y0.y, y1.y); i++)
        {
            if(!(i > 0 && i < m_DepthCropped.rows))
                continue;

            printf("Vertical: %d %d\n", i, y0.x);

            if(m_DepthCropped.at<cv::Vec3f>(i, y0.x)[2] < min_depth - 3 && m_DepthCropped.at<cv::Vec3f>(i, y0.x)[2] > 100)
            {
                printf("Deleting depth %u for depth %f\n", min_depth, m_DepthCropped.at<cv::Vec3f>(i, y0.x)[2]);

                min_depth = m_DepthCropped.at<cv::Vec3f>(i, y0.x)[2];
                points.clear();
                points.push_back(cv::Point(y0.x, i));
            }else if(std::abs(m_DepthCropped.at<cv::Vec3f>(i, y0.x)[2] - min_depth) < 3)
            {
                points.push_back(cv::Point(y0.x, i));
            }
        }

        printf("Points [");

        for(auto& p : points)
        {
            printf("[%d %d], ", p.x, p.y);
        }

        printf("]\n");

        if (points.size() > 0)
            pickPoint.y = (points[0].y + points[points.size() - 1].y) / 2;

        requiredOpening1 = std::abs(y0.y - y1.y) + 6;
        requiredOpening2 = std::abs(x0.x - x1.x) + 6;
        requiredAngle1 += 90;
    }*/

    printf("Buff\n");
    
    printf("After PickingPoint");

    y0 = Raycast(pickPoint, cv::Point(0, 1)); // Sopra
    y1 = Raycast(pickPoint, cv::Point(0, -1)); //sotto
    x0 = Raycast(pickPoint, cv::Point(1, 0)); // destra
    x1 = Raycast(pickPoint, cv::Point(-1, 0));    // Sinis

    printf("After RayCast\n");

    cv::line(m_Cropped, y0, y1, cv::Scalar(255, 0, 0), 1);
    cv::line(m_Cropped, x0, x1, cv::Scalar(255, 0, 0), 1);

    cv::circle(m_Cropped, pickPoint, 1, cv::Scalar(0, 0, 255), -1);

    cv::Mat M_inv, reverted;
    cv::Size original_size = m_Image.size();

    // Step 1: Create a new image of the original size
    cv::Mat new_image = cv::Mat::zeros(original_size, m_Cropped.type());

    // Get the image size
    int imgWidth = new_image.cols;
    int imgHeight = new_image.rows;

    // Calculate the ROI position and size
    int yValue = std::max(std::min(static_cast<int>(rect.center.y - rect_size.height / 2.0f), imgHeight - rect_size.height), 0);
    int xValue = std::max(std::min(static_cast<int>(rect.center.x - rect_size.width / 2.0f), imgWidth - rect_size.width), 0);

    // Step 2: Place m_Cropped in this new image at the position corresponding to the original rect
    cv::Rect original_rect(xValue, yValue, rect_size.width, rect_size.height);

    printf("Other Things\n");

#ifdef DEBUG
    fprintf(stderr, "Original Rect: %d %d %d %d\n", original_rect.x, original_rect.y, original_rect.width, original_rect.height);
    fprintf(stderr, "Original Size: %d %d\n", original_size.width, original_size.height);
#endif

    m_Cropped.copyTo(new_image(original_rect));

    // Step 3: Compute the inverse of M
    cv::invertAffineTransform(M, M_inv);

    // Step 4: Apply cv::warpAffine with the inverted matrix
    cv::warpAffine(new_image, reverted, M_inv, original_size, cv::INTER_NEAREST, cv::BORDER_CONSTANT, cv::Scalar(0));

    // Step 5: If the width and height were swapped, swap them back
    if (rect.angle < -45.0f) {
        cv::swap(reverted.cols, reverted.rows);
    }

    cv::Point newPickingPoint = FindColor(cv::Scalar(0, 0, 255), reverted);

    //fprintf(stderr, "Picking Point: %d %d\n", newPickingPoint.x, newPickingPoint.y);
    
    cv::circle(reverted, newPickingPoint, 2, cv::Scalar(0, 255, 0), -1);
    cv::imwrite(std::string("output/") + std::string("result_") + path.substr(path.find_last_of("/") + 1), reverted);
    //cv::imshow("Image", reverted);
    cv::imwrite(std::string("output/") + path.substr(path.find_last_of("/")), m_Cropped);

    printf("Picking Point: %d %d\n", newPickingPoint.x, newPickingPoint.y);
    printf("First opening: %d with angle %.2f\n", requiredOpening1, requiredAngle1);
    printf("Second opening: %d with angle %.2f\n", requiredOpening2, requiredAngle2);

#ifdef DEBUG
    fprintf(stderr, "Writed Image, Destroying Windows... \n");
#endif

    //cv::waitKey(0); 
    //cv::destroyAllWindows();

#ifdef DEBUG
    fprintf(stderr, "Destroyed All Windows... \n");
#endif
}

cv::Point PickingPoint::FindColor(cv::Scalar color, cv::Mat& image) 
{
    for (int i = 0; i < image.rows; i++) {
        for (int j = 0; j < image.cols; j++) {
            cv::Vec3b pixel = image.at<cv::Vec3b>(i, j);
            
#ifdef DEBUG
            if(pixel[0] != 0 && pixel[1] != 0 && pixel[2] != 0)
                fprintf(stderr, "Color: %d %d %d\n", pixel[0], pixel[1], pixel[2]);
#endif
            
            if (pixel[0] == color[0] && pixel[1] == color[1] && pixel[2] == color[2]) {
                return cv::Point(j, i);
            }
        }
    }

    return cv::Point(-1, -1);
}

void PickingPoint::ExtractCells(size_t cell_size, cv::Mat img)
{
    int width = img.cols;
    int height = img.rows;

    if (cell_size > width) {
        cell_size = width/2;
    }

    if (cell_size > height) {
        cell_size = height/2;
    }

    if (cell_size <= 0) {
        cell_size = 1;
    }

    int y;
    for (y = 0; y < height - cell_size; y += cell_size) {
        int x;

        std::vector<std::pair<double, cv::Rect>> v;

        for (x = 0; x < width - cell_size; x += cell_size) {
            #ifdef DEBUG
                fprintf(stderr, "Looping 1 %d %d %d %d %d\n", height - cell_size, height, cell_size, x, y);
            #endif
            int k = x*y + x;
            cv::Rect grid_rect(x, y, cell_size, cell_size);
            v.push_back({0, grid_rect});
            #ifdef DRAWDEBUG
                cv::rectangle(img, grid_rect, cv::Scalar(0, 255, 0), 1);
            #endif
        }

        if (width - x > 0) {
            int k = x*y + x;
            cv::Rect grid_rect(x, y, width - x, cell_size);
            v.push_back({0, grid_rect});
            #ifdef DRAWDEBUG
                cv::rectangle(img, grid_rect, cv::Scalar(0, 255, 0), 1);
            #endif
        }

        m_Cells.push_back(v);
    }

    if (height - y > 0) {
        int x;
        std::vector<std::pair<double, cv::Rect>> v;

        for (x = 0; x < width - cell_size; x += cell_size) {
            #ifdef DEBUG
                fprintf(stderr, "Looping 2\n");
            #endif
            int k = x*y + x;
            cv::Rect grid_rect(x, y, cell_size, height - y);
            v.push_back({0, grid_rect});
            #ifdef DRAWDEBUG
                cv::rectangle(img, grid_rect, cv::Scalar(0, 255, 0), 1);
            #endif
        }

        if (width - x > 0) {
            int k = x*y + x;
            cv::Rect grid_rect(x, y, width - x, height - y);
            v.push_back({0, grid_rect});
#ifdef DRAWDEBUG
            cv::rectangle(img, grid_rect, cv::Scalar(0, 255, 0), 1);
#endif

        }

        m_Cells.push_back(v);
    }
    
    m_CellsCache.resize(m_Cells.size());
    m_DepthCache.resize(m_Cells.size());

    for(int i = 0; i < m_Cells.size(); i++)
    {
        m_CellsCache[i].assign(m_Cells[i].size(), std::numeric_limits<unsigned int>::max());
        m_DepthCache[i].assign(m_Cells[i].size(), std::numeric_limits<unsigned int>::max());
    }
}

std::pair<size_t, size_t> PickingPoint::FindMaxCell()
{
    double min_value = std::numeric_limits<double>::min();
    size_t x, y; 

    for(size_t i = 0; i < m_Cells.size(); i++)
    {
        for(size_t j = 0; j < m_Cells[i].size(); j++)
        {
            if(m_Cells[i][j].first > min_value && m_Cells[i][j].first != std::numeric_limits<double>::max())
            {
                min_value = m_Cells[i][j].first;
                x = j;
                y = i;
            }
        }
    }

    return {y, x};
}

cv::Point PickingPoint::Raycast(cv::Point startingPoint, cv::Point direction) {
    cv::Point currentPoint = startingPoint;
    cv::Point savedPoint = startingPoint;


    printf("Si Va Bene! %d %d\n", currentPoint.x, currentPoint.y);
    bool prev_color_black;
    cv::Vec3b color = m_Cropped.at<cv::Vec3b>(currentPoint);
    printf("CUrrentPointless\n");

    if(color[0] == 0 && color[1] == 0 && color[2] == 0)
    {
        prev_color_black = true;
    }
    else
    {
        prev_color_black = false;
    }

    while (currentPoint.x >= 0 && currentPoint.x < m_Cropped.cols && currentPoint.y >= 0 && currentPoint.y < m_Cropped.rows) {
        color = m_Cropped.at<cv::Vec3b>(currentPoint);

        if (color[0] == 0 && color[1] == 0 && color[2] == 0) {
            if(prev_color_black == false)
            {
                savedPoint = currentPoint;
            }
            prev_color_black = true;
        }
        else
        {
            prev_color_black = false;
        }

        currentPoint += direction;
    }

    if (!prev_color_black) {
        savedPoint = currentPoint;
    }
    
    return savedPoint;
}

void PickingPoint::DrawHeatMap(const std::string& path) {
    m_HeatMap = m_Cropped.clone();

    std::pair<size_t, size_t> max_cell = FindMaxCell();
    auto cell = m_Cells[max_cell.first][max_cell.second];
    double maxValue = cell.first;

    // pixelValue : maxValue = x : 255
    for(size_t i = 0; i < m_Cells.size(); i++)
    {
        for(size_t j = 0; j < m_Cells[i].size(); j++)
        {
            cv::rectangle(m_HeatMap, m_Cells[i][j].second, cv::Scalar(0, 0, (m_Cells[i][j].first * 255) / maxValue), -1);
        }
    }

#ifdef DEBUG
    fprintf(stderr, "Writing Heatmap... %s\n", (std::string("output/") + std::string("heatmap_") + path.substr(path.find_last_of("/") + 1)).c_str());
#endif

    cv::imwrite(std::string("output/") + std::string("heatmap_") + path.substr(path.find_last_of("/") + 1), m_HeatMap);
}


// Non Ottimizzato: 4.572s
// Ottimizzato: 2.514s
void PickingPoint::HandleCell(std::pair<double, cv::Rect>& cell, int row, int col)
{
    if(GetPixelCount(m_Cells[row][col].second, row, col) == 0)
    {
        cell.first = std::numeric_limits<double>::max();
        return;
    }
    
    for(int i = 0; i < m_Cells.size(); i++)
    {
        for(int j = 0; j < m_Cells[i].size(); j++)
        {
            cell.first += GetPixelCount(m_Cells[i][j].second, i, j) * GetDistance(row, col, i, j);
        }
    }

    //cell.first /= std::max((double) GetMinDepth(m_Cells[row][col].second, row, col), 1.0);
}

unsigned int PickingPoint::GetMinDepth(cv::Rect& rect, size_t row, size_t col) {

    #ifdef DEBUG
        fprintf(stderr, "GetMinDepth: Row: %d Col: %d\n", row, col);
    #endif

    if(m_DepthCache[row][col] != std::numeric_limits<unsigned int>::max())
    {
        return m_DepthCache[row][col];
    }

    #ifdef DEBUG
        fprintf(stderr, "Cache Checked!\n", row, col);
    #endif

    for(int i = rect.y; i < rect.y + rect.height; i++)
    {
        for(int j = rect.x; j < rect.x + rect.width; j++)
        {
            
            #ifdef DEBUG
                fprintf(stderr, "Working for %d %d!\n", i ,j);
            #endif

            unsigned int color = (unsigned int) m_DepthMap.at<cv::Vec3f>(i, j)[2];

            #ifdef DEBUG
                fprintf(stderr, "Working for %d %d done!\n", i ,j);
            #endif

            assert(m_DepthCache[row].size() > col);

            #ifdef DEBUG
                fprintf(stderr, "Working for %d %d done 2!\n", i ,j);
            #endif

            m_DepthCache[row][col] = std::min(m_DepthCache[row][col], color);

            #ifdef DEBUG
                fprintf(stderr, "Values Saved!\n");
            #endif
        }
    }

    #ifdef DEBUG
        fprintf(stderr, "Returning Value: %u!\n", m_DepthCache[row][col]);
    #endif

    return m_DepthCache[row][col];
}

double PickingPoint::GetDistance(int x1, int y1, int x2, int y2)
{
    if(x1 == x2)
        return std::abs(y1 - y2);
    if(y1 == y2)
        return std::abs(x1 - x2);

    return std::sqrt(std::pow(std::abs(x1 - x2), 2) + std::pow(std::abs(y1 - y2), 2)); //pitagora
}

unsigned int PickingPoint::GetPixelCount(cv::Rect& rect, size_t row, size_t col)
{
    if(m_CellsCache[row][col] != std::numeric_limits<unsigned int>::max())
    {
        return m_CellsCache[row][col];
    }

    unsigned int count = 0;

    for(int i = rect.y; i < rect.y + rect.height; i++)
    {
        for(int j = rect.x; j < rect.x + rect.width; j++)
        {
            cv::Vec3b color = m_Cropped.at<cv::Vec3b>(i, j);
            if(color[0] != 0 && color[1] != 0 && color[2] != 0)
            {
                count++;
            }
        }
    }

    m_CellsCache[row][col] = count;

    return count;
}

std::vector<PickingPoint::Cell> PickingPoint::FindMinCell(unsigned int n)
{

    if (n == 1)
        n = 2;
        
    double min_value = std::numeric_limits<double>::max();
    size_t x, y;
    std::vector<PickingPoint::Cell> cells;

    for(size_t i = 0; i < m_Cells.size(); i++)
    {
        for(size_t j = 0; j < m_Cells[i].size(); j++)
        {
            cells.push_back({m_Cells[i][j].first, j, i});
        }
    }

    std::sort(cells.begin(), cells.end(), [](const struct Cell& a, const struct Cell& b) -> bool {
        return a.value < b.value;
    });

    std::vector<PickingPoint::Cell> result;
    for (int i = 0; i < std::min((size_t) n, cells.size()); i++ ) {
        result.push_back(cells[i]);
    }

    return result;
}