#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>

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

        const double interval =
            std::clamp(
                Mod::get()->getSettingValue<double>("interval"),
                0.1,
                10.0
            );

        m_fields->elapsed += static_cast<double>(dt);

        if (m_fields->elapsed < interval)
            return;

        // Keep the timer bounded and trigger exactly one hop for this frame.
        m_fields->elapsed -= interval;

        auto* player = m_player1;

        if (!player || player->m_isDead)
            return;

        const float velocity =
            player->m_isUpsideDown
                ? -bunny_hop::TEST_JUMP_VELOCITY
                : bunny_hop::TEST_JUMP_VELOCITY;

        player->pushPlayer(velocity);
    }

    void resetLevel() {
        m_fields->elapsed = 0.0;
        PlayLayer::resetLevel();
    }
};
