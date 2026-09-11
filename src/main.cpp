#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/binding/PlayerObject.hpp>

using namespace geode::prelude;

namespace bunny_hop {
    constexpr double TEST_JUMP_VELOCITY = 10.0;
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
        if (interval < 0.1) interval = 0.1;
        if (interval > 10.0) interval = 10.0;

        m_fields->elapsed += static_cast<double>(dt);

        if (m_fields->elapsed < interval) return;

        // Consume complete intervals so a long frame cannot create an
        // unbounded timer value. The boost itself is applied once per update.
        m_fields->elapsed = 0.0;

        auto player = m_player1;
        if (!player || player->m_isDead) return;

        // This writes the game's actual PlayerObject vertical velocity rather
        // than changing the level or simulating a jump through level objects.
        // Positive Y is upward in GD's normal orientation; gravity reversal
        // requires the opposite sign.
        player->m_yVelocity = player->m_isUpsideDown
            ? -bunny_hop::TEST_JUMP_VELOCITY
            : bunny_hop::TEST_JUMP_VELOCITY;
    }

    void resetLevel() {
        m_fields->elapsed = 0.0;
        PlayLayer::resetLevel();
    }
};

