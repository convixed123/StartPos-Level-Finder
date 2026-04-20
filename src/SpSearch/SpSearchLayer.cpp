#include <Geode/Geode.hpp>

#include "SpSearchLayer.hpp"

#include "Geode/cocos/layers_scenes_transitions_nodes/CCLayer.h"
#include "Geode/cocos/layers_scenes_transitions_nodes/CCScene.h"
#include <Geode/binding/GJSearchObject.hpp>
#include <Geode/binding/FLAlertLayer.hpp>

using namespace geode::prelude;

std::string SpSearchLayer::levelName;
int SpSearchLayer::levelID;

std::string SpSearchLayer::customLevelName;

bool SpSearchLayer::active = false;

SpSearchLayer* SpSearchLayer::create() {
    auto ret = new SpSearchLayer();
    if (ret->init()) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

CCScene* SpSearchLayer::scene(std::string name, int id) {
    auto ret = CCScene::create();
    auto layer = SpSearchLayer::create();

    AppDelegate::get()->m_runningScene = ret;
    ret->addChild(layer);

    layer->levelName = name;
    layer->levelID = id;

    layer->customLevelName = "";

    return ret;
}

bool SpSearchLayer::init() {
    if (!CCLayer::init()) return false;
    this->setID("startpos-search-layer");

    active = true;

    this->setKeyboardEnabled(true);
    this->setKeypadEnabled(true);

    guiMain();
    guiList();
    guiExtra();

    this->runAction(CCSequence::create(
        CCDelayTime::create(0),
        CCCallFunc::create(this, callfunc_selector(SpSearchLayer::searchForLevels)),
        nullptr
    ));

    onLoading();

    this->updateLayout();
    return true;
}

void SpSearchLayer::guiMain() {
    auto winSize = CCDirector::get()->getWinSize();

    auto backButtonMenu = CCMenu::create(nullptr);
    backButtonMenu->setID("back-menu");
    backButtonMenu->setAnchorPoint({0, 0});
    backButtonMenu->setScale(1);
    backButtonMenu->setContentSize({50, 50});
    backButtonMenu->setPosition(-1, winSize.height - 48);
    this->addChild(backButtonMenu, 1);

    auto backBtn = CCMenuItemSpriteExtra::create(
        CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png"),
        this,
        menu_selector(SpSearchLayer::onBack)
    );
    backBtn->setID("back-button");
    backBtn->setPosition({25, 25});
    backButtonMenu->addChild(backBtn);

    auto reloadMenu = CCMenu::create(nullptr);
    reloadMenu->setID("reload-menu");
    reloadMenu->setAnchorPoint({0, 0});
    reloadMenu->setScale(1);
    reloadMenu->setContentSize({50, 50});
    reloadMenu->setPosition(winSize.width - 52, 2);
    this->addChild(reloadMenu, 2);

    auto reloadBtn = CCMenuItemSpriteExtra::create(
        CCSprite::createWithSpriteFrameName("GJ_updateBtn_001.png"),
        this,
        menu_selector(SpSearchLayer::onReload)
    );
    reloadBtn->setID("reload-button");
    reloadBtn->setPosition({25, 25});
    reloadMenu->addChild(reloadBtn);
}

void SpSearchLayer::guiList() {
    auto winSize = CCDirector::get()->getWinSize();

    auto searchMenu = CCMenu::create(nullptr);
    searchMenu->setID("search-menu");
    searchMenu->setContentSize({400, 50});
    searchMenu->setPositionX(winSize.width / 2 - searchMenu->getContentSize().width / 2);
    searchMenu->setPositionY((winSize.height / 2 - searchMenu->getContentSize().height / 2) + 90);
    this->addChild(searchMenu, 2);

    auto searchBG = CCLayerColor::create(
        {194, 114, 62, 255},
        350, 40.5f
    );
    searchBG->setPosition(25, -1);
    searchBG->setID("search-bg");
    searchMenu->addChild(searchBG, 0);

    searchBar = TextInput::create(295, "Search levels...");
    searchBar->setID("search-bar");
    searchBar->setTextAlign(TextInputAlign::Left);
    searchBar->setMaxCharCount(20);
    searchBar->setPosition(180, 20);
    searchMenu->addChild(searchBar, 99);

    auto searchBtn = CCMenuItemSpriteExtra::create(
        CCSprite::createWithSpriteFrameName("gj_findBtn_001.png"),
        this,
        menu_selector(SpSearchLayer::onSearch)
    );
    searchBtn->setID("search-button");
    searchBtn->setPosition(350, 20);
    searchMenu->addChild(searchBtn, 2);

    auto listMenu = CCMenu::create(nullptr);
    listMenu->setID("list-menu");
    listMenu->setPosition(0, 0);
    this->addChild(listMenu, 3);
    
    auto bg = CCLayerColor::create(
        {194, 114, 62, 255},
        356, 180
    );
    bg->setID("level-list-bg");
    bg->setPositionX(winSize.width / 2 - bg->getContentSize().width / 2);
    bg->setPositionY((winSize.height / 2 - bg->getContentSize().height / 2) - 22.5f);
    listMenu->addChild(bg, 0);

    auto levelList = GJListLayer::create(
        nullptr,
        "Startpos Search",
        {0, 0, 0},
        356, 220, 0
    );
    levelList->setID("level-list");
	levelList->setContentSize({356, 175});
	levelList->setPositionX(winSize.width / 2 - levelList->getContentSize().width / 2);
	levelList->setPositionY((winSize.height / 2 - levelList->getContentSize().height / 2) - 27.5f);
    listMenu->addChild(levelList, 2);

    loadingCircle = LoadingCircle::create();
    loadingCircle->setID("loading-circle");
    loadingCircle->setParentLayer(this);
    loadingCircle->show();
}

void SpSearchLayer::guiExtra() {
    auto winSize = CCDirector::get()->getWinSize();
    
    auto bg = CCSprite::create("GJ_gradientBG.png");
    bg->setID("background");
    bg->setColor({0, 102, 255});
    bg->setAnchorPoint({0, 0});
    bg->setPosition({0, 0});
    bg->setScaleX((winSize.width + 10) / bg->getTextureRect().size.width);
    bg->setScaleY(winSize.height / bg->getTextureRect().size.height);
    this->addChild(bg, 0);

    auto leftCorner = CCSprite::createWithSpriteFrameName("GJ_sideArt_001.png");
    leftCorner->setID("left-corner");
    leftCorner->setAnchorPoint({0, 0});
    leftCorner->setContentSize({71.5, 71.5});
    leftCorner->setPosition({-1, -1});
    this->addChild(leftCorner, 1);

    auto rightCorner = CCSprite::createWithSpriteFrameName("GJ_sideArt_001.png");
    rightCorner->setID("right-corner");
    rightCorner->setFlipX(true);
    rightCorner->setAnchorPoint({0, 0});
    rightCorner->setContentSize({71.5, 71.5});
    rightCorner->setPosition({(winSize.width - 70.5f), -1});
    this->addChild(rightCorner, 1);
}

void SpSearchLayer::searchForLevels() {
    auto glm = GameLevelManager::get();
    glm->m_levelManagerDelegate = this;

    std::string finalLevelName = ((customLevelName.empty()) ? levelName + " startpos" : customLevelName);
    auto searchObject = GJSearchObject::create(
        SearchType::Search,
        ((finalLevelName.size() < 20) ? finalLevelName : finalLevelName.erase(20))
    );

    glm->getOnlineLevels(searchObject);
}

void SpSearchLayer::onSearch(CCObject*) {
    customLevelName = searchBar->getString();

    onReload(nullptr);
}

void SpSearchLayer::onReload(CCObject*) {
    onLoading();
    searchForLevels();
}

void SpSearchLayer::loadLevelsFinished(CCArray* levels, const char*, int) {
    auto winSize = CCDirector::get()->getWinSize();

    auto listMenu = this->getChildByID("list-menu");

    if (auto old = listMenu->getChildByID("list-view")) {
        old->removeFromParent();
    }

    listView = CustomListView::create(levels, BoomListType::Level, 175, 356);
    listView->retain();
    listView->setID("list-view");
	listView->setPositionX(winSize.width / 2 - listView->getContentSize().width / 2);
	listView->setPositionY((winSize.height / 2 - listView->getContentSize().height / 2) - 22.5f);
    listMenu->addChild(listView, 1);

    loadingCircle->setVisible(false);
}

void SpSearchLayer::loadLevelsFailed(const char*) {
    loadingCircle->setVisible(false);
}

void SpSearchLayer::onLoading() {
    loadingCircle->setVisible(true);
}

void SpSearchLayer::onBack(CCObject*) {
    active = false;
    CCDirector::get()->popSceneWithTransition(0.5f, kPopTransitionFade);
}

void SpSearchLayer::keyBackClicked() {
    onBack(nullptr);
}