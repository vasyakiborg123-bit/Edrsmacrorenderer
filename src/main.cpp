#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/PauseLayer.hpp>

using namespace geode::prelude;

bool g_isRendering = false;

void captureAndSendFrame(int width, int height) {
    std::vector<uint8_t> frameData(width * height * 4);
    glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, frameData.data());

    geode::Loader::get()->dispatchEvent("eclipse.ffmpeg-api/record-frame", frameData.data());
}

class $modify(MyPauseLayer, PauseLayer) {
    void customSetup() {
        PauseLayer::customSetup();

        auto rightMenu = this->getChildByID("right-side-menu");
        if (!rightMenu) return;

        auto spr = CCSprite::createWithSpriteFrameName("GJ_playBtn2_001.png");
        spr->setScale(0.5f);

        auto btn = CCMenuItemSpriteExtra::create(
            spr, this, menu_selector(MyPauseLayer::onToggleRender)
        );
        btn->setID("render-toggle-button"_spr);

        rightMenu->addChild(btn);
        rightMenu->updateLayout();
    }

    void onToggleRender(CCObject*) {
        g_isRendering = !g_isRendering;

        if (g_isRendering) {
            int fps = static_cast<int>(Mod::get()->getSettingValue<int64_t>("render-fps"));
            int width = static_cast<int>(Mod::get()->getSettingValue<int64_t>("render-width"));
            int height = static_cast<int>(Mod::get()->getSettingValue<int64_t>("render-height"));
            bool audio = Mod::get()->getSettingValue<bool>("render-audio");

            log::info("Старт рендера: {}x{} @ {} FPS (Audio: {})", width, height, fps, audio);

            geode::Loader::get()->dispatchEvent("eclipse.ffmpeg-api/start-render", nullptr);
            
            FLAlertLayer::create("Renderer", "Рендер запущен!", "OK")->show();
        } else {
            log::info("Стоп рендера");
            
            geode::Loader::get()->dispatchEvent("eclipse.ffmpeg-api/stop-render", nullptr);

            FLAlertLayer::create("Renderer", "Рендер остановлен!", "OK")->show();
        }
    }
};

class $modify(MyPlayLayer, PlayLayer) {
    void update(float dt) {
        if (!g_isRendering) {
            PlayLayer::update(dt);
            return;
        }

        int targetFPS = static_cast<int>(Mod::get()->getSettingValue<int64_t>("render-fps"));
        if (targetFPS <= 0) targetFPS = 120;

        float fixedDT = 1.0f / static_cast<float>(targetFPS);

        PlayLayer::update(fixedDT);

        int width = static_cast<int>(Mod::get()->getSettingValue<int64_t>("render-width"));
        int height = static_cast<int>(Mod::get()->getSettingValue<int64_t>("render-height"));

        captureAndSendFrame(width, height);
    }
};
