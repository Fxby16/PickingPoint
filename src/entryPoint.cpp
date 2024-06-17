#include <pickingPoint.hpp>
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
}