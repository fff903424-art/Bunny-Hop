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
        bool hopPending = false;
        bool hopLocked = false;
    };

    void postUpdate(float dt) {
        // Let Geometry Dash perform its normal update first.
        PlayLayer::postUpdate(dt);

        auto* player = m_player1;

        // No player available.
        if (!player) {
            m_fields->elapsed = 0.0;
            m_fields->hopPending = false;
            m_fields->hopLocked = false;
            return;
        }

        // If the mod is disabled, completely reset its internal state.
        if (!Mod::get()->getSettingValue<bool>("enabled")) {
            m_fields->elapsed = 0.0;
            m_fields->hopPending = false;
            m_fields->hopLocked = false;
            return;
        }

        // Dead players should not receive a hop.
        if (player->m_isDead) {
            m_fields->hopPending = false;
            m_fields->hopLocked = false;
            return;
        }

        const double interval = std::clamp(
            static_cast<double>(
                Mod::get()->getSettingValue<float>("interval")
            ),
            0.1,
            10.0
        );

        /*
         * Advance the timer only while no hop is waiting.
         *
         * Once the interval is reached, we mark a hop as pending.
         * The actual hop waits until the player is grounded.
         */
        if (!m_fields->hopPending && !m_fields->hopLocked) {
            m_fields->elapsed += static_cast<double>(dt);

            if (m_fields->elapsed >= interval) {
                m_fields->hopPending = true;
            }
        }

        /*
         * A hop has already happened.
         *
         * Do absolutely nothing until Geometry Dash reports that
         * the player has landed again.
         */
        if (m_fields->hopLocked) {
            if (player->m_isOnGround) {
                m_fields->hopLocked = false;
            }

            return;
        }

        /*
         * The timer expired while the player was airborne.
         *
         * Keep the hop pending instead of firing it in mid-air.
         */
        if (!m_fields->hopPending)
            return;

        if (!player->m_isOnGround)
            return;

        /*
         * We are grounded and a hop is pending.
         *
         * Apply the upward velocity once, then lock the hop until
         * the player lands again.
         */
        const double velocity = player->m_isUpsideDown
            ? -bunny_hop::JUMP_VELOCITY
            : bunny_hop::JUMP_VELOCITY;

        player->setYVelocity(velocity, 68);

        m_fields->elapsed = 0.0;
        m_fields->hopPending = false;
        m_fields->hopLocked = true;
    }

    void resetLevel() {
        m_fields->elapsed = 0.0;
        m_fields->hopPending = false;
        m_fields->hopLocked = false;

        PlayLayer::resetLevel();
    }
};
