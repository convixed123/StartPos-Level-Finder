#include <Geode/Geode.hpp>
#include "Geode/cocos/CCDirector.h"
#include "Geode/cocos/cocoa/CCObject.h"

#include <Geode/modify/LevelCell.hpp>
#include <Geode/binding/LevelInfoLayer.hpp>
#include <Geode/binding/GJGameLevel.hpp>
#include <string>

#include "../SpSearch/SpSearchLayer.hpp"
#include "../SpSearch/SaveUtils.hpp"

using namespace geode::prelude;

class loadLevel : public LevelManagerDelegate {
public:
    //i know its a kludge, but i couldnt find another way
    static void load(int id) {
        auto delegate = new loadLevel();

        auto glm = GameLevelManager::get();
        glm->m_levelManagerDelegate = delegate;

        auto searchObject = GJSearchObject::create(
            SearchType::Search,
            std::to_string(id)
        );

        glm->getOnlineLevels(searchObject);
    }
    //im terrible at programming...         sorry
    
    void loadLevelsFinished(CCArray* levels, char const*, int) override {
        auto level = static_cast<GJGameLevel*>(levels->objectAtIndex(0));
        if (!level) return;

        auto levelPageScene = LevelInfoLayer::scene(level, false);
        CCDirector::get()->pushScene(CCTransitionFade::create(0.5f, levelPageScene));
    }
};

class $modify(CustomLevelCell, LevelCell) {
    void loadFromLevel(GJGameLevel* level) {
        LevelCell::loadFromLevel(level);

        auto winSize = CCDirector::get()->getWinSize();

        if (level->m_levelType == GJLevelType::Editor) return;

        auto levelCell = this->m_mainLayer;

        auto mainMenu = this->m_mainMenu;

        CCPoint spBtnPos = {
            mainMenu->getChildByID("view-button")->getPosition().x - 49,
            mainMenu->getChildByID("view-button")->getPosition().y
        };

        if (!SpSearchLayer::active) {
            if (SpSaveUtils::getStartPos(this->m_level->m_levelID)) {
                auto spFinderBtnActive = CCMenuItemSpriteExtra::create(
                    CircleButtonSprite::createWithSpriteFrameName(
                        "edit_eStartPosBtn_001.png", 0.9f,
                        CircleBaseColor::Green,
                        CircleBaseSize::Tiny
                    ),
                    this,
                    menu_selector(CustomLevelCell::onActiveFinderBtn)
                );
                spFinderBtnActive->setID("startpos-level-finder-active");
                spFinderBtnActive->setPosition(spBtnPos);

                mainMenu->addChild(spFinderBtnActive);
            } else {
                auto spFinderBtn = CCMenuItemSpriteExtra::create(
                    CircleButtonSprite::createWithSpriteFrameName(
                        "edit_eStartPosBtn_001.png", 0.9f,
                        CircleBaseColor::Cyan,
                        CircleBaseSize::Tiny
                    ),
                    this,
                    menu_selector(CustomLevelCell::onFinderBtn)
                );
                spFinderBtn->setID("startpos-level-finder");
                spFinderBtn->setPosition(spBtnPos);

                mainMenu->addChild(spFinderBtn);
            }

            if (auto percentLabel = this->getChildByIDRecursive("percentage-label")) {
                if (this->m_compactView) {
                    percentLabel->setPositionX(percentLabel->getPosition().x - 30);
                }
            }

            if (auto toggler = this->getChildByIDRecursive("select-toggler")) {
                toggler->setPositionX(toggler->getPosition().x - 25);
            }

            if (auto completed = this->getChildByIDRecursive("completed-icon")) {
                if (this->m_compactView) {
                    completed->setPositionX(completed->getPosition().x - 30);
                }
            }
        } else {
            auto spBtnPin = CCMenuItemSpriteExtra::create(
                CircleButtonSprite::createWithSpriteFrameName(
                    "GJ_starsIcon_001.png", 0.9f,
                    CircleBaseColor::Green,
                    CircleBaseSize::Tiny
                ),
                this,
                menu_selector(CustomLevelCell::onPinBtn)
            );
            spBtnPin->setID("startpos-btn-pin");
            spBtnPin->setPosition(spBtnPos);

            mainMenu->addChild(spBtnPin);
        }
        mainMenu->updateLayout();
    }

    void onFinderBtn(CCObject*) {
        auto scene = SpSearchLayer::scene(
            this->m_level->m_levelName,
            this->m_level->m_levelID
        );

        CCDirector::get()->pushScene(CCTransitionFade::create(0.5f, scene));
    }

    void onActiveFinderBtn(CCObject*) {
        int id = SpSaveUtils::getStartPos(this->m_level->m_levelID);
        
        loadLevel::load(id);
    }

    void onPinBtn(CCObject*) {
        SpSaveUtils::saveStartPos(
            SpSearchLayer::levelID,
            this->m_level->m_levelID
        );

        SpSearchLayer::active = false;
        CCDirector::get()->popScene();
    }
};