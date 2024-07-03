#pragma once

#include <opencv2/opencv.hpp>

#include <vector>

class PickingPoint
{
public:
    PickingPoint() = default;
    ~PickingPoint() = default;

    struct Cell
    {
        double value;
        unsigned int x;
        unsigned int y;
    };

    /**
     * \brief Initializes the PickingPoint object and calls Process()
     */
    static void Start();

    /**
     * \brief Main function that processes the given mask and depth images and finds the picking point
     * \param mask_path the path to the mask image
     * \param depth_path the path to the depth image (masked)
     * \param output_folder the folder where the output images will be saved
     * \return a pair containing the picking point and the average depth of the object
     */
    std::pair<cv::Point, double> Process(const std::string& mask_path, const std::string& depth_path, const std::string& output_folder);

    /**
     * \brief Extracts the cells from the given image and save them in the m_Cells matrix
     * \param cell_size the size of the cell
     * \param img the image to extract the cells from
     */
    void ExtractCells(size_t cell_size, cv::Mat img);

    /**
     * \brief Iterates over the cells and gives each cell a score based on the cells around it
     * \param cell the cell to handle
     * \param row the row of the cell
     * \param col the column of the cell
     */
    void HandleCell(std::pair<double, cv::Rect>& cell, int row, int col);

    /**
     * \brief Returns the distance between two points
     */
    double GetDistance(int x1, int y1, int x2, int y2);

    /**
     * \brief Returns the number of non-black pixels in the given rectangle (it samples the m_Cropped image)
     * \param rect the rectangle to check
     * \param row the row of the cell
     * \param col the column of the cell
     */
    unsigned int GetPixelCount(cv::Rect& rect, size_t row, size_t col);

    /**
     * \brief Draws the heat map of the cells based on the scores given by HandleCell
     * \param name the name of the output image (heatmap_<name>)
     * \param output_folder the folder where the output images will be saved
     */
    void DrawHeatMap(const std::string& name, const std::string& output_folder);

    /**
     * \brief Calculates the average pixel depth of the given rectangle (it samples the m_DepthCropped image)
     * \param rect the rectangle to check
     * \return the average depth of the rectangle
     */
    double GetAvgDepth(cv::Rect& rect);

    /**
     * \return the indices of the cell that contains the given point (y, x)
    */
    std::pair<size_t, size_t> GetCellFromPoint(cv::Point point, size_t cell_size);

    /**
     * \return the indices of the cells around the given center cell
    */
    std::vector<std::pair<size_t, size_t>> GetCellsFromCenter(std::pair<size_t, size_t> center, size_t cells_to_get);

    /**
     * \return the point where the ray intersects the first non-black pixel or a big difference in depth
     */
    cv::Point Raycast(cv::Point startingPoint, cv::Point direction, bool useDepth = false);

    /**
     * \brief Finds the color in the image and returns the point where it was found
     * \param color the color to find
     * \param image the image to search in
     * \return the point where the color was found
     */
    cv::Point FindColor(cv::Scalar color, cv::Mat& image);

    /**
     * \return the first n cells with the lowest score assigned by HandleCell
     */
    std::vector<Cell> FindMinCell(unsigned int n);

    /**
     * \return the cell with the highest score assigned by HandleCell
     */
    std::pair<size_t, size_t> FindMaxCell();

    static inline const char* GetClassName(unsigned int class_id)
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

private:
    cv::Mat m_Image;
    cv::Mat m_Cropped;
    cv::Mat m_HeatMap;
    cv::Mat m_DepthMap;
    cv::Mat m_DepthCropped;
    cv::Mat m_DepthCroppedNormalized;

    std::vector<std::vector<std::pair<double, cv::Rect>>> m_Cells;
    std::vector<std::vector<unsigned int>> m_CellsCache;
    std::vector<std::vector<unsigned int>> m_DepthCache;
};