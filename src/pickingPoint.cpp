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
    cv::warpAffine(m_Image, rotated, M, m_Image.size(), cv::INTER_NEAREST);

    // crop the resulting image
    cv::getRectSubPix(rotated, rect_size, rect.center, m_Cropped);

    cv::warpAffine(m_DepthMap, rotated2, M, m_DepthMap.size(), cv::INTER_NEAREST);

    // crop the resulting image
    cv::getRectSubPix(rotated2, rect_size, rect.center, m_DepthCropped);



    /* Normalization:
 ________   ________  ________  _____ ______   ________  ___       ___  ________  ________  _________  ___  ________  ________      
|\   ___  \|\   __  \|\   __  \|\   _ \  _   \|\   __  \|\  \     |\  \|\_____  \|\   __  \|\___   ___\\  \|\   __  \|\   ___  \    
\ \  \\ \  \ \  \|\  \ \  \|\  \ \  \\\__\ \  \ \  \|\  \ \  \    \ \  \\|___/  /\ \  \|\  \|___ \  \_\ \  \ \  \|\  \ \  \\ \  \   
 \ \  \\ \  \ \  \\\  \ \   _  _\ \  \\|__| \  \ \   __  \ \  \    \ \  \   /  / /\ \   __  \   \ \  \ \ \  \ \  \\\  \ \  \\ \  \  
  \ \  \\ \  \ \  \\\  \ \  \\  \\ \  \    \ \  \ \  \ \  \ \  \____\ \  \ /  /_/__\ \  \ \  \   \ \  \ \ \  \ \  \\\  \ \  \\ \  \ 
   \ \__\\ \__\ \_______\ \__\\ _\\ \__\    \ \__\ \__\ \__\ \_______\ \__\\________\ \__\ \__\   \ \__\ \ \__\ \_______\ \__\\ \__\
    \|__| \|__|\|_______|\|__|\|__|\|__|     \|__|\|__|\|__|\|_______|\|__|\|_______|\|__|\|__|    \|__|  \|__|\|_______|\|__| \|__|                                                                                                     
*/

    // Finding the min depth for normalization
    unsigned int min_depth = std::numeric_limits<unsigned int>::max(), max_depth = 0;

    for(int i = 0; i < m_DepthCropped.rows; i++)
    {
        for(int j = 0; j < m_DepthCropped.cols; j++)
        {
            if(m_DepthCropped.at<cv::Vec3f>(i, j)[2] == 0)
            {
                continue;
            }

            min_depth = std::min(min_depth, std::max((unsigned int) m_DepthCropped.at<cv::Vec3f>(i, j)[2], (unsigned int) 100));
            max_depth = std::max(max_depth, (unsigned int) m_DepthCropped.at<cv::Vec3f>(i, j)[2]);
        }
    }

    // Normalizing the depth map
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

    // Converting the normalized map to a 8-bit image, and applying a rainbow colormap
    cv::Mat Depth_Converted;
    m_DepthCroppedNormalized.convertTo(Depth_Converted, CV_8UC1);

    cv::Mat Output;
    cv::applyColorMap(Depth_Converted, Output, cv::COLORMAP_RAINBOW);

    if (m_Cropped.rows == 0 || m_Cropped.cols == 0)
        return;

    // Extract cells
    ExtractCells(5, m_Cropped);

    // Iterate over all the cells to assign a score to each cell, in order to find the optimal cell
    for(int i = 0; i < m_Cells.size(); i++)
    {
        for(int j = 0; j < m_Cells[i].size(); j++)
        {
            HandleCell(m_Cells[i][j], i, j);
        }
    }

    // Finding the first 10 cells with the lowest score
    std::vector<PickingPoint::Cell> min_cell_list = FindMinCell(20);
    
    // Sorting the cells by the minimum depth, to find the highest point
    std::sort(min_cell_list.begin(), min_cell_list.end(), [this](const struct Cell& a, const struct Cell& b) -> bool {
        return GetMinDepth(m_Cells[a.y][a.x].second, a.y, a.x) < GetMinDepth(m_Cells[b.y][b.x].second, b.y, b.x);
    });

    DrawHeatMap(path); 

    cv::Rect r = m_Cells[min_cell_list[0].y][min_cell_list[0].x].second;

    cv::Point pickPoint = cv::Point(r.x + r.width / 2, r.y + r.height / 2);

    cv::Point y0 = Raycast(pickPoint, cv::Point(0, 1)); // Sopra
    cv::Point y1 = Raycast(pickPoint, cv::Point(0, -1)); //sotto
    cv::Point x0 = Raycast(pickPoint, cv::Point(1, 0)); // destra
    cv::Point x1 = Raycast(pickPoint, cv::Point(-1, 0));    // Sinis

    if(std::abs(y0.y - y1.y) > std::abs(x0.x - x1.x))
    {
        pickPoint.x = (x0.x + x1.x) / 2;
    }
    else
    {
        pickPoint.y = (y0.y + y1.y) / 2;
    }

    cv::circle(m_Cropped, y0, 2, cv::Scalar(0, 112, 255), -1);
    cv::circle(m_Cropped, y1, 2, cv::Scalar(0, 112, 255), -1);
    cv::circle(m_Cropped, x0, 2, cv::Scalar(0, 112, 255), -1);
    cv::circle(m_Cropped, x1, 2, cv::Scalar(0, 112, 255), -1);

    unsigned int requiredOpening1; // opening for the shortest side
    unsigned int requiredOpening2; // opening for the longest side
    float requiredAngle1 = requiredAngle; // angle for the shortest opening
    float requiredAngle2 = requiredAngle; // angle for the longest opening

    y0 = Raycast(pickPoint, cv::Point(0, 1)); // Sopra
    y1 = Raycast(pickPoint, cv::Point(0, -1)); //sotto
    x0 = Raycast(pickPoint, cv::Point(1, 0)); // destra
    x1 = Raycast(pickPoint, cv::Point(-1, 0));    // Sinis

    cv::line(m_Cropped, y0, y1, cv::Scalar(255, 0, 0), 1);
    cv::line(m_Cropped, x0, x1, cv::Scalar(255, 0, 0), 1);

    cv::circle(m_Cropped, pickPoint, 1, cv::Scalar(0, 0, 255), -1);
    cv::circle(Output, pickPoint, 1, cv::Scalar(0, 0, 255), -1);

    cv::Mat M_inv, reverted, depth_reverted;
    cv::Size original_size = m_Image.size();

    // Step 1: Create a new image of the original size
    cv::Mat new_image = cv::Mat::zeros(original_size, m_Cropped.type());
    cv::Mat new_image2 = cv::Mat::zeros(original_size, m_Cropped.type());

    // Get the image size
    int imgWidth = new_image.cols;
    int imgHeight = new_image.rows;

    // Calculate the ROI position and size
    int yValue = std::max(std::min(static_cast<int>(rect.center.y - rect_size.height / 2.0f), imgHeight - rect_size.height), 0);
    int xValue = std::max(std::min(static_cast<int>(rect.center.x - rect_size.width / 2.0f), imgWidth - rect_size.width), 0);

    // Step 2: Place m_Cropped in this new image at the position corresponding to the original rect
    cv::Rect original_rect(xValue, yValue, rect_size.width, rect_size.height);
    cv::Rect original_rect2(xValue, yValue, rect_size.width, rect_size.height);

    m_Cropped.copyTo(new_image(original_rect));
    Output.copyTo(new_image2(original_rect2));

    // Step 3: Compute the inverse of M
    cv::invertAffineTransform(M, M_inv);

    // Step 4: Apply cv::warpAffine with the inverted matrix
    cv::warpAffine(new_image, reverted, M_inv, original_size, cv::INTER_NEAREST, cv::BORDER_CONSTANT, cv::Scalar(0));
    cv::warpAffine(new_image2, depth_reverted, M_inv, original_size, cv::INTER_NEAREST, cv::BORDER_CONSTANT, cv::Scalar(0));

    // Step 5: If the width and height were swapped, swap them back
    if (rect.angle < -45.0f) {
        cv::swap(reverted.cols, reverted.rows);
        cv::swap(depth_reverted.cols, depth_reverted.rows);
    }

    cv::Point newPickingPoint = FindColor(cv::Scalar(0, 0, 255), reverted);

    cv::circle(reverted, newPickingPoint, 2, cv::Scalar(0, 255, 0), -1);
    cv::circle(depth_reverted, newPickingPoint, 2, cv::Scalar(0, 255, 0), -1);
    cv::imwrite(std::string("output/") + std::string("result_") + path.substr(path.find_last_of("/") + 1), reverted);
    cv::imwrite(std::string("output/") + path.substr(path.find_last_of("/")), m_Cropped);
    cv::imwrite(std::string("output/") + std::string("depth_") + path.substr(path.find_last_of("/") + 1), depth_reverted);
}

cv::Point PickingPoint::FindColor(cv::Scalar color, cv::Mat& image) 
{
    for (int i = 0; i < image.rows; i++) {
        for (int j = 0; j < image.cols; j++) {
            cv::Vec3b pixel = image.at<cv::Vec3b>(i, j);
            
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
            int k = x*y + x;
            cv::Rect grid_rect(x, y, cell_size, cell_size);
            v.push_back({0, grid_rect});
        }

        if (width - x > 0) {
            int k = x*y + x;
            cv::Rect grid_rect(x, y, width - x, cell_size);
            v.push_back({0, grid_rect});
        }

        m_Cells.push_back(v);
    }

    if (height - y > 0) {
        int x;
        std::vector<std::pair<double, cv::Rect>> v;

        for (x = 0; x < width - cell_size; x += cell_size) {
            int k = x*y + x;
            cv::Rect grid_rect(x, y, cell_size, height - y);
            v.push_back({0, grid_rect});
        }

        if (width - x > 0) {
            int k = x*y + x;
            cv::Rect grid_rect(x, y, width - x, height - y);
            v.push_back({0, grid_rect});
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


    bool prev_color_black;
    cv::Vec3b color = m_Cropped.at<cv::Vec3b>(currentPoint);

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
        savedPoint = currentPoint - direction;
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

    if(m_DepthCache[row][col] != std::numeric_limits<unsigned int>::max())
    {
        return m_DepthCache[row][col];
    }

    for(int i = rect.y; i < rect.y + rect.height; i++)
    {
        for(int j = rect.x; j < rect.x + rect.width; j++)
        {
            unsigned int color = (unsigned int) m_DepthCropped.at<cv::Vec3f>(i, j)[2];

            //printf("Color: %u\n", color);

            m_DepthCache[row][col] = std::min(m_DepthCache[row][col], color);
        }
    }

    //printf("Depth: %u\n", m_DepthCache[row][col]);

    if (m_DepthCache[row][col] < 220) {
        m_DepthCache[row][col] = std::numeric_limits<unsigned int>::max();
    }

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
    for (int i = 0; i < std::min((size_t) n, cells.size()); i++) {
        result.push_back(cells[i]);
    }

    return result;
}