#pragma once

#include <Geode/Geode.hpp>
#include <Geode/binding/LevelManagerDelegate.hpp>
#include <Geode/binding/SetIDPopupDelegate.hpp>
#include <Geode/binding/GameLevelManager.hpp>

using namespace geode::prelude;

class SpSearchLayer : public CCLayer, public SetIDPopupDelegate, public LevelManagerDelegate {
public:
    static std::string levelName;
    static int levelID;

    static std::string customLevelName;

    static bool active;

    virtual bool init() override;

    static SpSearchLayer* create();
    static cocos2d::CCScene* scene(std::string, int);

    virtual void keyBackClicked() override;
    void onBack(CCObject*);
private:
    LoadingCircle* loadingCircle;
    CustomListView* listView;
    TextInput* searchBar;

    virtual void loadLevelsFinished(CCArray*, const char*, int) override;
    virtual void loadLevelsFailed(const char*) override;

    void guiMain();
    void guiList();
    void guiExtra();

    void searchForLevels();
    
    void onLoading();

    void onSearch(CCObject*);
    void onReload(CCObject*);
};