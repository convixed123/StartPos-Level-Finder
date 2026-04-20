#include <Geode/Geode.hpp>

#include <Geode/modify/LevelInfoLayer.hpp>
#include <Geode/binding/GJGameLevel.hpp>

#include "../SpSearch/SaveUtils.hpp"

using namespace geode::prelude;

class $modify(CustomLevelInfoLayer, LevelInfoLayer) {
    bool init(GJGameLevel* level, bool challenge) {
        if (!LevelInfoLayer::init(level, challenge)) return false;

        if (auto leftSideMenu = this->getChildByID("left-side-menu")) {
            if (SpSaveUtils::checkForExistence(this->m_level->m_levelID)) {
                auto unpinBtn = CCMenuItemSpriteExtra::create(
                    CircleButtonSprite::createWithSpriteFrameName(
                        "edit_eStartPosBtn_001.png", 1,
                        CircleBaseColor::DarkAqua,
                        CircleBaseSize::Medium
                    ),
                    this,
                    menu_selector(CustomLevelInfoLayer::onUnpinBtn)
                );
                unpinBtn->setID("unpin-startpos-button");
                leftSideMenu->addChild(unpinBtn);
                leftSideMenu->updateLayout();
            }
        }

        return true;
    }

    void onUnpinBtn(CCObject* sender) {
        auto btn = static_cast<CCMenuItemSpriteExtra*>(sender);

        createQuickPopup(
            "Remove Start Pos",
            "Do you want to remove this level from save?",
            "No", "Yes",
            [this, btn] (auto, bool onBtn) {
                if (onBtn) {
                    SpSaveUtils::removeStartPos(this->m_level->m_levelID);
                    CCDirector::get()->popSceneWithTransition(0.5f, kPopTransitionFade);
                }
            }
        );
    }
};