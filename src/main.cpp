#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/binding/PlayerObject.hpp>

using namespace geode::prelude;

namespace bunny_hop {
    constexpr double DEFAULT_JUMP_VELOCITY = 10.0;
}

class $modify(BunnyHopPlayLayer, PlayLayer) {
public:
    struct Fields {
        double elapsed = 0.0;
    };

    void postUpdate(float dt) {
        // Let Geometry Dash finish its normal frame first.
        PlayLayer::postUpdate(dt);

        if (!Mod::get()->getSettingValue<bool>("enabled")) {
            m_fields->elapsed = 0.0;
            return;
        }

        const double interval = std::clamp(
            Mod::get()->getSettingValue<float>("interval"),
            0.1f,
            10.0f
        );

        m_fields->elapsed += static_cast<double>(dt);

        if (m_fields->elapsed < interval)
            return;

        // Keep excess time so the timer stays stable.
        m_fields->elapsed -= interval;

        auto* player = m_player1;

        if (!player || player->m_isDead)
            return;

        const double velocity = player->m_isUpsideDown
            ? -bunny_hop::DEFAULT_JUMP_VELOCITY
            : bunny_hop::DEFAULT_JUMP_VELOCITY;

        // Directly set the player's vertical velocity after normal physics.
        player->setYVelocity(velocity, 68);
    }

    void resetLevel() {
        m_fields->elapsed = 0.0;
        PlayLayer::resetLevel();
    }
};
