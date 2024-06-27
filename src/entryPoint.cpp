#include <pickingPoint.hpp>
#include <timer.hpp>
#include <filesystem>

int main(int argc, char** argv)
{
    std::string mask_path = "assets/masks";

    for(const auto& mask_entry : std::filesystem::directory_iterator(mask_path))
    {
        for(const auto& entry : std::filesystem::directory_iterator(mask_entry.path()))
        {
            printf("Path: %s\n", entry.path().c_str());

            std::string tmp = entry.path().string().replace(entry.path().string().find("masks"), 5, "depth_masked");
            std::string tmp2 = tmp.replace(tmp.find_last_of("."), 4, ".exr");

            std::string output_folder = "output/" + mask_entry.path().filename().string();

            std::filesystem::create_directory(output_folder);

            Timer timer;

            PickingPoint pickingPoint;
            pickingPoint.Process(entry.path(), tmp2, output_folder);
        }
    }

    return 0;
}