#include <pickingPoint.hpp>
#include <utils.hpp>
#include <timer.hpp>

#include <cstdio>
#include <vector>
#include <cmath>
#include <limits>
#include <map>
#include <filesystem>

void PickingPoint::Start()
{
    std::string mask_path = "assets/masks";

    std::map<unsigned int, std::vector<double>> object_class_times; // map to store the times for each object class (to get the average time for each class)

    // Create output directory if it doesn't exist
    if(!std::filesystem::exists("output")){
        std::filesystem::create_directory("output");
    }

    //iterate over all the masks directories
    for(const auto& mask_entry : std::filesystem::directory_iterator(mask_path))
    {
        //open the rgb image that contains all the objects
        cv::Mat colorImage = cv::imread(mask_entry.path().string().replace(mask_entry.path().string().find("masks"), 5, "depth") + ".jpg", cv::IMREAD_COLOR);

        // Save the picking point for each object
        std::vector<std::pair<cv::Point, double>> points;

        // Iterate over all the objects in the mask directory
        for(const auto& entry : std::filesystem::directory_iterator(mask_entry.path()))
        {
            printf("Path: %s\n", entry.path().c_str());

            std::string tmp = entry.path().string().replace(entry.path().string().find("masks"), 5, "depth_masked");
            std::string tmp2 = tmp.replace(tmp.find_last_of("."), 4, ".exr");

            std::string output_folder = "output/" + mask_entry.path().filename().string();

            // Create output directory if it doesn't exist
            if(!std::filesystem::exists(output_folder)){
                std::filesystem::create_directory(output_folder);
            }

            Timer timer; // Timer to measure the time taken to process each object

            PickingPoint pickingPoint;
            points.push_back(pickingPoint.Process(entry.path(), tmp2, output_folder)); // Process the object and save the picking point

            //extract the object class from the filename
            unsigned int object_class = std::stoi(tmp2.substr(tmp2.find_last_of('_') + 1, tmp2.find_last_of('_') - tmp2.find_last_of('.')));
        
            //store the time taken to process the object in the respective object class vector
            object_class_times[object_class].push_back(timer.ElapsedMillis());
        }

        // Sort the points based on the depth (lower depth first)
        std::sort(points.begin(), points.end(), [](const auto& p1, const auto& p2) {
            return p1.second < p2.second;
        });

        // Draw a green point for the first object to take
        cv::circle(colorImage, points[0].first, 3, cv::Scalar(0, 255, 0), -1);
        cv::putText(colorImage, std::to_string((int) points[0].second) + " " + "0", cv::Point(points[0].first.x + 5, points[0].first.y + 5), cv::FONT_HERSHEY_SIMPLEX, 0.3, cv::Scalar(255, 255, 255), 1);

        // Draw a red point for the rest of the objects
        for(int k = 1; k < points.size(); k++)
        {
            cv::circle(colorImage, points[k].first, 3, cv::Scalar(0, 0, 255), -1);
            cv::putText(colorImage, std::to_string((int) points[k].second) + " " + std::to_string(k), cv::Point(points[k].first.x + 5, points[k].first.y + 5), cv::FONT_HERSHEY_SIMPLEX, 0.3, cv::Scalar(255, 255, 255), 1);
        }

        // Save the image with the picking points
        cv::imwrite("output/" + mask_entry.path().filename().string() + ".png", colorImage);
    }

    // Calculate the average time for each object class
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

        printf("Class \033[0;34m%s\033[0m\nAverage time: %s ms\nMin time: %s ms\nMax time: %s ms\n", GetClassName(object_class), rstStr.c_str(), rstStr2.c_str(), rstStr3.c_str());
    }

    // Reset terminal color
    printf("\033[0m");
}

std::pair<cv::Point, double> PickingPoint::Process(const std::string& path, const std::string& depth_path, const std::string& output_folder)
{
    m_Image = cv::imread(path, cv::IMREAD_COLOR); //read the mask
    m_DepthMap = cv::imread(depth_path, cv::IMREAD_UNCHANGED); //read the masked depth map

    if(m_Image.empty())
    {
        perror("No image data\n");
        return {cv::Point(-1, -1), -1.0};
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

    // rotate and crop the mask
    m_Cropped = cv::Mat(rect_size, m_Image.type());
    GetRotRectImg(rect, m_Image, m_Cropped);

    // rotate and crop the depth map
    m_DepthCropped = cv::Mat(rect_size, m_DepthMap.type());
    GetRotRectImg(rect, m_DepthMap, m_DepthCropped);


    // NORMALIZATION

    // Finding the min and max depth for normalization
    unsigned int min_depth = std::numeric_limits<unsigned int>::max(), max_depth = 0;

    for(int i = 0; i < m_DepthCropped.rows; i++)
    {
        for(int j = 0; j < m_DepthCropped.cols; j++)
        {
            if(m_DepthCropped.at<cv::Vec3f>(i, j)[2] == 0)
            {
                continue;
            }

            min_depth = std::min(min_depth, std::max((unsigned int) m_DepthCropped.at<cv::Vec3f>(i, j)[2], (unsigned int) 300));
            max_depth = std::max(max_depth, (unsigned int) m_DepthCropped.at<cv::Vec3f>(i, j)[2]);
        }
    }

    // Normalize the depth map
    m_DepthCroppedNormalized = cv::Mat::zeros(m_DepthCropped.size(), m_DepthCropped.type());

    for(int i = 0; i < m_DepthCropped.rows; i++)
    {
        for(int j = 0; j < m_DepthCropped.cols; j++)
        {
            if(m_DepthCropped.at<cv::Vec3f>(i, j)[2] == 0)
            {
                continue;
            }

            float val = std::max(0.0, (m_DepthCropped.at<cv::Vec3f>(i, j)[2] - min_depth) / (max_depth - min_depth) * 255.0);
            m_DepthCroppedNormalized.at<cv::Vec3f>(i, j) = cv::Vec3f(val, val, val);
        }
    }

    // Convert the normalized map to a 8-bit image, and apply a colormap
    cv::Mat Depth_Converted;
    m_DepthCroppedNormalized.convertTo(Depth_Converted, CV_8UC1);

    cv::Mat Output;
    cv::applyColorMap(Depth_Converted, Output, cv::COLORMAP_JET);

    cv::imwrite(output_folder + std::string("/depth_colored_") + path.substr(path.find_last_of("/") + 1), Output);

    if (m_Cropped.rows == 0 || m_Cropped.cols == 0)
        return {cv::Point(-1, -1), -1.0};

    // Extract cells
    int cell_size = std::max((int) std::ceil(std::log10(m_Cropped.rows * m_Cropped.cols) * 2), 1);

    ExtractCells(cell_size, m_Cropped);

    // Iterate over all the cells to assign a score to each cell, in order to find the optimal cell
    for(int i = 0; i < m_Cells.size(); i++)
    {
        for(int j = 0; j < m_Cells[i].size(); j++)
        {
            HandleCell(m_Cells[i][j], i, j);
        }
    }

    // Draw the heatmap
    DrawHeatMap(path, output_folder);

    cv::imwrite(output_folder + std::string("/cropped_") + path.substr(path.find_last_of("/") + 1), m_Cropped);

    //10 : 15 = cell_size : x
    size_t cells_to_get = std::max((8 * cell_size) / 10, 1);

    // Finding the cell with the lowest score
    std::vector<PickingPoint::Cell> min_cell_list = FindMinCell(1);
    cv::Rect r = m_Cells[min_cell_list[0].y][min_cell_list[0].x].second;
    
    //center the point in the cell
    cv::Point pickPoint = cv::Point(r.x + r.width / 2, r.y + r.height / 2);

    unsigned int requiredOpening1; // opening for the shortest side
    unsigned int requiredOpening2; // opening for the longest side
    float requiredAngle1 = requiredAngle; // angle for the shortest opening
    float requiredAngle2 = requiredAngle; // angle for the longest opening

    //raycast considering the depth
    cv::Point y0 = Raycast(pickPoint, cv::Point(0, 1), true); // Sopra
    cv::Point y1 = Raycast(pickPoint, cv::Point(0, -1), true); //sotto
    cv::Point x0 = Raycast(pickPoint, cv::Point(1, 0), true); // destra
    cv::Point x1 = Raycast(pickPoint, cv::Point(-1, 0), true); // sinistra

    //center the point on the shortest side, find the required opening and angle
    if(std::abs(y0.y - y1.y) > std::abs(x0.x - x1.x)){
        pickPoint = cv::Point((x0.x + x1.x) / 2, pickPoint.y);
    }else{
        pickPoint = cv::Point(pickPoint.x, (y0.y + y1.y) / 2);
    }

    //raycast considering the depth
    y0 = Raycast(pickPoint, cv::Point(0, 1), true); // Sopra
    y1 = Raycast(pickPoint, cv::Point(0, -1), true); //sotto
    x0 = Raycast(pickPoint, cv::Point(1, 0), true); // destra
    x1 = Raycast(pickPoint, cv::Point(-1, 0), true); // sinistra

    //center the point on the shortest side, find the required opening and angle
    if(std::abs(y0.y - y1.y) > std::abs(x0.x - x1.x)){
        requiredOpening1 = std::abs(x0.x - x1.x) + 6;
        requiredOpening2 = std::abs(y0.y - y1.y) + 6;
        requiredAngle2 += 90;
        pickPoint = cv::Point((x0.x + x1.x) / 2, pickPoint.y);
    }else{
        requiredOpening1 = std::abs(y0.y - y1.y) + 6;
        requiredOpening2 = std::abs(x0.x - x1.x) + 6;
        requiredAngle1 += 90;
        pickPoint = cv::Point(pickPoint.x, (y0.y + y1.y) / 2);
    }

    printf("Required Opening 1: %u Angle1: %f\nRequired Opening 2: %u Angle2: %f\n", requiredOpening1, requiredAngle1, requiredOpening2, requiredAngle2);

    //draw the point on the image
    cv::circle(m_Cropped, pickPoint, 1, cv::Scalar(0, 0, 255), -1);
    cv::circle(Output, pickPoint, 1, cv::Scalar(0, 0, 255), -1);

    //revert the changes made at the beginning
    cv::Mat dstDepth;
    RevertRotation(m_DepthCropped, dstDepth, m_Image.size(), rect, rect_size);
    
    cv::Mat dstImage;
    RevertRotation(m_Cropped, dstImage, m_Image.size(), rect, rect_size);

    //find the picking point on the original image
    cv::Point newPickingPoint = FindColor(cv::Scalar(0, 0, 255), dstImage); //find the picking point on the original image

    printf("New Picking Point: (%d, %d)\n", newPickingPoint.x, newPickingPoint.y);

    //draw the point on the original image
    cv::circle(dstDepth, newPickingPoint, 1, cv::Scalar(0, 255, 0), -1);

    //save the images
    cv::imwrite(output_folder + std::string("/result_") + path.substr(path.find_last_of("/") + 1), dstImage);
    cv::imwrite(output_folder + path.substr(path.find_last_of("/")), m_Cropped);
    cv::imwrite(output_folder + std::string("/depth_") + path.substr(path.find_last_of("/") + 1), dstDepth);
    cv::imwrite(output_folder + std::string("/depthCropped_") + path.substr(path.find_last_of("/") + 1) + std::string(".exr"), m_DepthCropped);
    
    size_t count = 0;
    double avgDepth = 0.0;

    //calculate the average depth of the object
    for(auto row : m_Cells)
    {
        for(auto cell : row)
        {
            double depth = GetAvgDepth(cell.second);

            if (depth <= 0) {
                count++;
                continue;
            }
            
            avgDepth += depth;
        }
    }

    avgDepth /= (double) ((m_Cells.size() * m_Cells[0].size()) - count);

    if (avgDepth <= 0 || avgDepth > 1000 || std::isnan(avgDepth) || std::isinf(avgDepth)) {
        avgDepth = 500;
    }

    return {newPickingPoint, avgDepth};
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
    size_t x = 0, y = 0; 

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

cv::Point PickingPoint::Raycast(cv::Point startingPoint, cv::Point direction, bool useDepth) {

    cv::Point currentPoint = startingPoint;

    bool exited = false;

    while(m_Cropped.at<cv::Vec3b>(startingPoint)[0] == 0 && m_Cropped.at<cv::Vec3b>(startingPoint)[1] == 0 && m_Cropped.at<cv::Vec3b>(startingPoint)[2] == 0)
    {
        startingPoint += direction;

        if(startingPoint.x < 0 || startingPoint.x >= m_Cropped.cols || startingPoint.y < 0 || startingPoint.y >= m_Cropped.rows)
        {
            exited = true;
            break;
        }
    }

    if(!exited)
    {
        currentPoint = startingPoint;
    }

    cv::Point savedPoint = currentPoint;

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

    cv::Vec3f oldDepth = m_DepthCropped.at<cv::Vec3f>(currentPoint)[2];
    while (currentPoint.x >= 0 && currentPoint.x < m_Cropped.cols && currentPoint.y >= 0 && currentPoint.y < m_Cropped.rows) {
        color = m_Cropped.at<cv::Vec3b>(currentPoint);

        if (color[0] == 0 && color[1] == 0 && color[2] == 0) {
            if(prev_color_black == false)
            {
                return currentPoint;
            }
            prev_color_black = true;
        }
        else
        {
            prev_color_black = false;
        }

        if (useDepth) {
            cv::Vec3f depth = m_DepthCropped.at<cv::Vec3f>(currentPoint)[2];
            if (std::abs(depth[2] - oldDepth[2]) > 10) {
                return savedPoint;
            }

            oldDepth = depth;
        }

        currentPoint += direction;
    }

    if (!prev_color_black) {
        savedPoint = currentPoint - direction;
    }
    
    return savedPoint;
}

void PickingPoint::DrawHeatMap(const std::string& name, const std::string& output_folder) {
    m_HeatMap = m_Cropped.clone();

    std::pair<size_t, size_t> max_cell = FindMaxCell();
    printf("Max Cell: %lu %lu\n", max_cell.first, max_cell.second);
    auto cell = m_Cells[max_cell.first][max_cell.second];
    double maxValue = cell.first;

    // pixelValue : maxValue = x : 255
    for(size_t i = 0; i < m_Cells.size(); i++)
    {
        for(size_t j = 0; j < m_Cells[i].size(); j++)
        {
            unsigned int color = m_Cells[i][j].first;

            if(m_Cells[i][j].first == std::numeric_limits<double>::max())
            {
                color = 0;
            }

            cv::rectangle(m_HeatMap, m_Cells[i][j].second, cv::Scalar(0, 0, (color * 255.0) / maxValue), -1);
        }
    }

    cv::imwrite(output_folder + std::string("/heatmap_") + name.substr(name.find_last_of("/") + 1), m_HeatMap);
}

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
            if(color[0] != 0 || color[1] != 0 || color[2] != 0)
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
    std::vector<PickingPoint::Cell> cells;

    for(int i = 0; i < m_Cells.size(); i++)
    {
        for(int j = 0; j < m_Cells[i].size(); j++)
        {
            cells.push_back({m_Cells[i][j].first, j, i});
        }
    }

    // Sort the cells based on their value
    std::sort(cells.begin(), cells.end(), [](const Cell& a, const Cell& b) {
        return a.value < b.value;
    });

    // Keep only the top n cells, if n is less than the size of cells
    if(n < cells.size())
    {
        cells.resize(n);
    }

    return cells;
}

std::pair<size_t, size_t> PickingPoint::GetCellFromPoint(cv::Point point, size_t cell_size)
{
    return {point.y / cell_size, point.x / cell_size};
}

std::vector<std::pair<size_t, size_t>> PickingPoint::GetCellsFromCenter(std::pair<size_t, size_t> center, size_t cells_to_get)
{
    int side_length = std::max(std::ceil(std::sqrt(cells_to_get)), 1.0);

    if(side_length % 2 == 0)
    {
        side_length++;
    }

    int start_x, end_x, start_y, end_y;

    start_x = std::max((int) center.second - side_length / 2, (int) 0);
    end_x = std::min(start_x + side_length, (int) m_Cells[0].size());

    start_y = std::max((int) center.first - side_length / 2, (int) 0);
    end_y = std::min(start_y + side_length, (int) m_Cells.size());

    std::vector<std::pair<size_t, size_t>> cells;

    for(size_t i = start_y; i < end_y; i++)
    {
        for(size_t j = start_x; j < end_x; j++)
        {
            cells.push_back({i, j});
        }
    }

    return cells;
}

double PickingPoint::GetAvgDepth(cv::Rect& rect)
{
    double sum = 0;
    size_t count = 0;

    for(int i = rect.y; i < rect.y + rect.height; i++)
    {
        for(int j = rect.x; j < rect.x + rect.width; j++)
        {
            if(m_DepthCropped.at<cv::Vec3f>(i, j)[2] < 260)
            {
                count++;
                continue;
            }

            sum += m_DepthCropped.at<cv::Vec3f>(i, j)[2];
        }
    }

    double avg = sum / (double) ((rect.width * rect.height) - count);

    if(avg < 0 || std::isnan(avg)){
        return -1;
    }

    return avg;
}