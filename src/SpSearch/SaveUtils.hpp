#pragma once

#include <Geode/Geode.hpp>

using namespace geode::prelude;

namespace SpSaveUtils {
    void saveStartPos(int, int);
    int getStartPos(int);
    void removeStartPos(int);
    bool checkForExistence(int);
}