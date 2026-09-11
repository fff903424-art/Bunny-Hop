#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/binding/PlayerObject.hpp>

using namespace geode::prelude;

namespace bunny_hop {
    // Test value for the initial version.
    // This is an additive vertical velocity, not a replacement.
    constexpr float JUMP_VELOCITY = 10.0f;
}

class $modify(BunnyHopPlayLayer, PlayLayer) {
public:
    struct Fields {
        // Time accumulated toward the next automatic hop.
        double elapsed = 0.0;

        // The interval has expired, but the player is currently airborne.
        bool hopPending = false;

        // An automatic hop has happened and we are waiting for
        // Geometry Dash to report a new landing.
        bool waitingForLanding = false;

        // Value of m_lastLandTime recorded when the automatic hop happened.
        double landTimeAtHop = 0.0;
    };

    void resetBunnyHopState() {
        m_fields->elapsed = 0.0;
        m_fields->hopPending = false;
        m_fields->waitingForLanding = false;
        m_fields->landTimeAtHop = 0.0;
    }

    void postUpdate(float dt) {
        // Let Geometry Dash run its normal gameplay physics first.
        PlayLayer::postUpdate(dt);

        auto* player = m_player1;

        if (!player) {
            resetBunnyHopState();
            return;
        }

        // Disabled.
        if (!Mod::get()->getSettingValue<bool>("enabled")) {
            resetBunnyHopState();
            return;
        }

        // Never apply an automatic hop to a dead player.
        if (player->m_isDead) {
            m_fields->hopPending = false;
            m_fields->waitingForLanding = false;
            m_fields->elapsed = 0.0;
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
         * ------------------------------------------------------------
         * AFTER A HOP:
         *
         * Do not allow another automatic hop until Geometry Dash
         * reports a NEW landing event.
         *
         * We deliberately use m_lastLandTime here instead of relying
         * on m_isOnGround to unlock the state.
         * ------------------------------------------------------------
         */
        if (m_fields->waitingForLanding) {
            if (player->m_lastLandTime > m_fields->landTimeAtHop) {
                // A genuine new landing occurred.
                m_fields->waitingForLanding = false;
                m_fields->hopPending = false;
                m_fields->elapsed = 0.0;
            }

            // While waiting for landing, never apply another hop.
            return;
        }

        /*
         * ------------------------------------------------------------
         * TIMER
         * ------------------------------------------------------------
         */
        if (!m_fields->hopPending) {
            m_fields->elapsed += static_cast<double>(dt);

            if (m_fields->elapsed >= interval) {
                m_fields->hopPending = true;
            }
        }

        if (!m_fields->hopPending)
            return;

        /*
         * ------------------------------------------------------------
         * TIMER EXPIRED WHILE AIRBORNE
         *
         * Keep the hop pending. Do not touch velocity.
         * ------------------------------------------------------------
         */
        if (!player->m_isOnGround)
            return;

        /*
         * ------------------------------------------------------------
         * GROUNDED + HOP PENDING
         *
         * Use the game's native PlayerObject::pushPlayer() operation.
         * In the 2.2081 binding this is an additive Y-velocity change,
         * rather than directly replacing m_yVelocity.
         * ------------------------------------------------------------
         */
        const float velocity = player->m_isUpsideDown
            ? -bunny_hop::JUMP_VELOCITY
            : bunny_hop::JUMP_VELOCITY;

        player->pushPlayer(velocity);

        /*
         * Consume this hop.
         */
        m_fields->hopPending = false;
        m_fields->elapsed = 0.0;

        /*
         * Lock automatic hopping until a NEW landing occurs.
         *
         * Record the current landing timestamp. A future landing must
         * have a strictly newer timestamp to unlock the next hop.
         */
        m_fields->waitingForLanding = true;
        m_fields->landTimeAtHop = player->m_lastLandTime;
    }

    void resetLevel() {
        resetBunnyHopState();

        PlayLayer::resetLevel();
    }
};
