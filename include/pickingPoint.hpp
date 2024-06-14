#pragma once

#include <opencv2/opencv.hpp>

#include <vector>

class PickingPoint
{
public:
    PickingPoint() = default;
    ~PickingPoint() = default;

    void Start(const std::string& path);

    void ExtractCells(size_t cell_size, cv::Mat img);
    void HandleCell(std::pair<double, cv::Rect>& cell, int row, int col);
    double GetDistance(int x1, int y1, int x2, int y2);
    unsigned int GetPixelCount(cv::Rect& rect);
    void DrawHeatMap(const std::string& path);

    std::pair<size_t, size_t> FindMinCell();
    std::pair<size_t, size_t> FindMaxCell();

private:
    cv::Mat m_Image;
    cv::Mat m_Cropped;
    cv::Mat m_HeatMap;

    std::vector<std::vector<std::pair<double, cv::Rect>>> m_Cells;
};