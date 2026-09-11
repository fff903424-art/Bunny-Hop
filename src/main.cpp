#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/binding/PlayerObject.hpp>

using namespace geode::prelude;

namespace bunny_hop {
    constexpr float TEST_JUMP_VELOCITY = 10.0f;
}

class $modify(BunnyHopPlayLayer, PlayLayer) {
public:
    struct Fields {
        double elapsed = 0.0;
    };

    void update(float dt) {
        PlayLayer::update(dt);

        if (!Mod::get()->getSettingValue<bool>("enabled")) {
            m_fields->elapsed = 0.0;
            return;
        }

        auto interval = Mod::get()->getSettingValue<double>("interval");

        if (interval < 0.1)
            interval = 0.1;

        if (interval > 10.0)
            interval = 10.0;

        m_fields->elapsed += static_cast<double>(dt);

        if (m_fields->elapsed < interval)
            return;

        m_fields->elapsed = 0.0;

        auto player = m_player1;

        if (!player || player->m_isDead)
            return;

        // Use Geometry Dash's PlayerObject boost function rather than
        // directly overwriting the vertical-velocity field.
        player->boostPlayer(bunny_hop::TEST_JUMP_VELOCITY);
    }

    void resetLevel() {
        m_fields->elapsed = 0.0;
        PlayLayer::resetLevel();
    }
};
