#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/PlayerObject.hpp>

using namespace geode::prelude;

namespace bunny_hop {
    constexpr double TEST_JUMP_VELOCITY = 10.0;
}

class $modify(BunnyHopPlayLayer, PlayLayer) {
public:
    struct Fields {
        double elapsed = 0.0;
        bool jumpPending = false;
    };

    void update(float dt) {
        PlayLayer::update(dt);

        if (!Mod::get()->getSettingValue<bool>("enabled")) {
            m_fields->elapsed = 0.0;
            m_fields->jumpPending = false;
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
        m_fields->jumpPending = true;
    }

    void resetLevel() {
        m_fields->elapsed = 0.0;
        m_fields->jumpPending = false;
        PlayLayer::resetLevel();
    }
};

class $modify(BunnyHopPlayerObject, PlayerObject) {
public:
    void update(float dt) {
        PlayerObject::update(dt);

        auto layer = PlayLayer::get();

        if (!layer)
            return;

        if (layer->m_player1 != this)
            return;

        if (m_isDead)
            return;

        auto fields = static_cast<BunnyHopPlayLayer*>(layer)->m_fields.self();

        if (!fields || !fields->jumpPending)
            return;

        fields->jumpPending = false;

        const double velocity = m_isUpsideDown
            ? -bunny_hop::TEST_JUMP_VELOCITY
            : bunny_hop::TEST_JUMP_VELOCITY;

        // Type 68 is the type used by Geode's official pushPlayer()
        // implementation in the 2.2081 bindings.
        setYVelocity(velocity, 68);
    }
};
