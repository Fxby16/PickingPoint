#include <pickingPoint.hpp>
#include <timer.hpp>
#include <filesystem>

int main(int argc, char** argv)
{
    std::string path = "assets/masks";

    for (const auto& entry : std::filesystem::directory_iterator(path)) {
        printf("Path: %s\n", entry.path().c_str());

        Timer timer;

        PickingPoint pickingPoint;
        std::string tmp = entry.path().string().replace(entry.path().string().find("masks"), 5, "depth_masked");
        pickingPoint.Process(entry.path(), tmp.replace(tmp.find_last_of("."), 4, ".exr"));
    }

    // for(const auto& entry : std::filesystem::directory_iterator("assets/masks"))
    // {
    //     printf("Path: %s\n", entry.path().c_str());
        
    //     Timer timer;

    //     PickingPoint pickingPoint;
    //     pickingPoint.Process(entry.path(), "assets/depth_masked/" + (entry.path().filename().string().substr(0, entry.path().filename().string().find_last_of(".")) + ".exr"));
    // }

    return 0;
}