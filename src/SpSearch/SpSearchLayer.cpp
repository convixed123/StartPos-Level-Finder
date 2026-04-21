#include <Geode/Geode.hpp>

#include "SpSearchLayer.hpp"

#include "Geode/cocos/cocoa/CCObject.h"
#include "Geode/cocos/label_nodes/CCLabelBMFont.h"
#include "Geode/cocos/layers_scenes_transitions_nodes/CCLayer.h"
#include "Geode/cocos/layers_scenes_transitions_nodes/CCScene.h"
#include "Geode/cocos/menu_nodes/CCMenu.h"
#include "Geode/cocos/menu_nodes/CCMenuItem.h"
#include "Geode/cocos/sprite_nodes/CCSprite.h"
#include <Geode/binding/GJSearchObject.hpp>
#include <Geode/binding/FLAlertLayer.hpp>
#include <string>

using namespace geode::prelude;

std::string SpSearchLayer::levelName;
int SpSearchLayer::levelID;

bool SpSearchLayer::active = false;

int page = 0;

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

    updatePageInfo();
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

    auto nextPageMenu = CCMenu::create(nullptr);
    nextPageMenu->setID("next-page-menu");
    nextPageMenu->setAnchorPoint({0, 0});
    nextPageMenu->setScale(1);
    nextPageMenu->setContentSize({50, 50});
    nextPageMenu->setPosition(winSize.width - 46, (winSize.height / 2 - nextPageMenu->getScaledContentSize().height / 2));
    this->addChild(nextPageMenu, 2);

    auto nextPageSpr = CCSprite::createWithSpriteFrameName("GJ_arrow_03_001.png");
    nextPageSpr->setFlipX(true);

    auto nextPageBtn = CCMenuItemSpriteExtra::create(
        nextPageSpr,
        this,
        menu_selector(SpSearchLayer::onNextPage)
    );
    nextPageBtn->setID("next-page-button");
    nextPageBtn->setPosition({25, 25});
    nextPageMenu->addChild(nextPageBtn);

    previousPageMenu = CCMenu::create(nullptr);
    previousPageMenu->setID("previous-page-menu");
    previousPageMenu->setVisible(false);
    previousPageMenu->setAnchorPoint({0, 0});
    previousPageMenu->setScale(1);
    previousPageMenu->setContentSize({50, 50});
    previousPageMenu->setPosition(-2, (winSize.height / 2 - previousPageMenu->getScaledContentSize().height / 2));
    this->addChild(previousPageMenu, 2);

    auto previousPageBtn = CCMenuItemSpriteExtra::create(
        CCSprite::createWithSpriteFrameName("GJ_arrow_03_001.png"),
        this,
        menu_selector(SpSearchLayer::onPreviousPage)
    );
    previousPageBtn->setID("previous-page-button");
    previousPageBtn->setPosition({25, 25});
    previousPageMenu->addChild(previousPageBtn);
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

    listMenu = CCMenu::create(nullptr);
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

    pageInfo = CCLabelBMFont::create(
        "",
        "goldFont.fnt"
    );
    pageInfo->setID("page-info");
    pageInfo->setPosition({winSize.width - 60, winSize.height - 12});
    pageInfo->setScale(0.5f);
    this->addChild(pageInfo);
}

void SpSearchLayer::searchForLevels() {
    auto glm = GameLevelManager::get();
    glm->m_levelManagerDelegate = this;

    std::string finalLevelName = ((customLevelName.empty()) ? levelName + " startpos" : customLevelName);
    auto searchObject = GJSearchObject::create(
        SearchType::Search,
        ((finalLevelName.size() < 20) ? finalLevelName : finalLevelName.erase(20))
    );

    auto finalSearchObject = searchObject->getPageObject(page);

    glm->getOnlineLevels(finalSearchObject);
}

void SpSearchLayer::loadLevelsFinished(CCArray* levels, const char*, int) {
    auto winSize = CCDirector::get()->getWinSize();

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
    if (auto old = listMenu->getChildByID("list-view")) {
        old->removeFromParent();
    }

    loadingCircle->setVisible(false);
}

void SpSearchLayer::onNextPage(CCObject*) {
    page++;

    previousPageMenu->setVisible(true);

    updatePageInfo();
    onLoading();
    searchForLevels();
}

void SpSearchLayer::onPreviousPage(CCObject*) {
    if (page == 0) return;

    page--;

    if (page == 0) {
        previousPageMenu->setVisible(false);
    }

    updatePageInfo();
    onLoading();
    searchForLevels();
}

void SpSearchLayer::onSearch(CCObject*) {
    customLevelName = searchBar->getString();

    onReload(nullptr);
}

void SpSearchLayer::onReload(CCObject*) {
    onLoading();
    searchForLevels();
}

void SpSearchLayer::onLoading() {
    if (auto old = listMenu->getChildByID("list-view")) {
        old->removeFromParent();
    }

    loadingCircle->setVisible(true);
}

void SpSearchLayer::onBack(CCObject*) {
    active = false;
    CCDirector::get()->popSceneWithTransition(0.5f, kPopTransitionFade);
}

void SpSearchLayer::keyBackClicked() {
    onBack(nullptr);
}

void SpSearchLayer::updatePageInfo() {
    pageInfo->setString(
        fmt::format("{} of {} of 9999", page * 10 + 1, page * 10 + 10).c_str(),
        "goldFont.fnt"
    );
}