#include <Geode/Geode.hpp>

using namespace geode::prelude;

namespace SpSaveUtils {
    void saveStartPos(int ID, int SpID) {
        std::string IDStr = std::to_string(ID);

        auto savedIds = Mod::get()->getSavedValue<std::unordered_map<std::string, int>>("savedIds");
        savedIds.insert({IDStr, SpID});

        Mod::get()->setSavedValue<std::unordered_map<std::string, int>>("savedIds", savedIds);
    }

    int getStartPos(int ID) {
        std::string IDStr = std::to_string(ID);

        auto savedIds = Mod::get()->getSavedValue<std::unordered_map<std::string, int>>("savedIds");
        auto it = savedIds.find(IDStr);

        if (it != savedIds.end()) {
            return it->second;
        } else {
            return 0;
        }
    }

    void removeStartPos(int SpID) {
        auto savedIds = Mod::get()->getSavedValue<std::unordered_map<std::string, int>>("savedIds");

        std::erase_if(savedIds, [SpID](const auto& item) {
            return item.second == SpID;
        });

        Mod::get()->setSavedValue<std::unordered_map<std::string, int>>("savedIds", savedIds);
    }

    bool checkForExistence(int SpID) {
        auto savedIds = Mod::get()->getSavedValue<std::unordered_map<std::string, int>>("savedIds");

        auto it = std::find_if(savedIds.begin(), savedIds.end(), [SpID](const auto& pair) {
            return pair.second == SpID;
        });

        return it != savedIds.end();
    }
}