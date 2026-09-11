#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/binding/PlayerObject.hpp>

using namespace geode::prelude;

namespace bunny_hop {
    constexpr double JUMP_VELOCITY = 10.0;
}

class $modify(BunnyHopPlayLayer, PlayLayer) {
public:
    struct Fields {
        double elapsed = 0.0;
    };

    void postUpdate(float dt) {
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

        m_fields->elapsed -= interval;

        auto* player = m_player1;

        if (!player || player->m_isDead)
            return;

        // Don't repeatedly reset an already-active jump.
        if (player->playerIsMovingUp())
            return;

        const double velocity = player->m_isUpsideDown
            ? -bunny_hop::JUMP_VELOCITY
            : bunny_hop::JUMP_VELOCITY;

        player->setYVelocity(velocity, 68);
    }

    void resetLevel() {
        m_fields->elapsed = 0.0;
        PlayLayer::resetLevel();
    }
};
