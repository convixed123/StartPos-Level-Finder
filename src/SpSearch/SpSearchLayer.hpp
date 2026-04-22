#pragma once

#include "Geode/cocos/cocoa/CCObject.h"
#include "Geode/cocos/label_nodes/CCLabelBMFont.h"
#include "Geode/cocos/menu_nodes/CCMenu.h"
#include <Geode/Geode.hpp>
#include <Geode/binding/LevelManagerDelegate.hpp>
#include <Geode/binding/SetIDPopupDelegate.hpp>
#include <Geode/binding/GameLevelManager.hpp>

using namespace geode::prelude;

class SpSearchLayer : public CCLayer, public SetIDPopupDelegate, public LevelManagerDelegate {
public:
    static std::string levelName;
    static int levelID;

    std::string customLevelName;

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
    CCMenu* reloadMenu;
    CCMenu* listMenu;
    CCMenu* nextPageMenu;
    CCMenu* previousPageMenu;
    CCLabelBMFont* pageInfo;

    int page;

    bool loadFinished;

    virtual void loadLevelsFinished(CCArray*, const char*, int) override;
    virtual void loadLevelsFailed(const char*) override;

    void guiMain();
    void guiList();
    void guiExtra();

    void searchForLevels();
    
    void onLoading();

    void onSearch(CCObject*);
    void onReload(CCObject*);

    void onNextPage(CCObject*);
    void onPreviousPage(CCObject*);

    void updatePageInfo();
};