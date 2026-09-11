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

        // A hop is waiting for the player to become grounded.
        bool hopPending = false;

        // Prevents another automatic hop after one has fired.
        bool hopLocked = false;

        // Becomes true only after we have actually observed the
        // player airborne after an automatic hop.
        bool hasLeftGround = false;
    };

    void postUpdate(float dt) {
        // Always let Geometry Dash perform its normal update first.
        PlayLayer::postUpdate(dt);

        auto* player = m_player1;

        if (!player) {
            m_fields->elapsed = 0.0;
            m_fields->hopPending = false;
            m_fields->hopLocked = false;
            m_fields->hasLeftGround = false;
            return;
        }

        // Disabled = completely reset the mod state.
        if (!Mod::get()->getSettingValue<bool>("enabled")) {
            m_fields->elapsed = 0.0;
            m_fields->hopPending = false;
            m_fields->hopLocked = false;
            m_fields->hasLeftGround = false;
            return;
        }

        // Never interact with a dead player.
        if (player->m_isDead) {
            m_fields->hopPending = false;
            m_fields->hopLocked = false;
            m_fields->hasLeftGround = false;
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
         * 1. If an automatic hop has already happened, wait for a
         *    complete airborne -> grounded cycle.
         * ------------------------------------------------------------
         */

        if (m_fields->hopLocked) {
            // First, we MUST observe the player actually leave
            // the ground. This prevents the lock from being cleared
            // immediately after applying the jump.
            if (!player->m_isOnGround) {
                m_fields->hasLeftGround = true;
            }

            // Only unlock after:
            //   airborne was observed
            //   AND
            //   player has subsequently become grounded.
            if (m_fields->hasLeftGround && player->m_isOnGround) {
                m_fields->hopLocked = false;
                m_fields->hasLeftGround = false;
            }

            // Never allow the timer to fire while locked.
            return;
        }

        /*
         * ------------------------------------------------------------
         * 2. Build the interval timer.
         * ------------------------------------------------------------
         */

        if (!m_fields->hopPending) {
            m_fields->elapsed += static_cast<double>(dt);

            if (m_fields->elapsed >= interval) {
                m_fields->hopPending = true;
            }
        }

        /*
         * ------------------------------------------------------------
         * 3. If the timer expired while airborne, wait.
         *
         *    IMPORTANT:
         *    We do NOT modify velocity while airborne.
         * ------------------------------------------------------------
         */

        if (m_fields->hopPending && !player->m_isOnGround) {
            return;
        }

        /*
         * ------------------------------------------------------------
         * 4. Timer expired + player is genuinely grounded.
         *
         *    Perform exactly ONE hop.
         * ------------------------------------------------------------
         */

        if (m_fields->hopPending && player->m_isOnGround) {
            const double velocity = player->m_isUpsideDown
                ? -bunny_hop::JUMP_VELOCITY
                : JUMP_VELOCITY;

            player->setYVelocity(velocity, 68);

            // Reset the interval.
            m_fields->elapsed = 0.0;

            // Consume the pending hop.
            m_fields->hopPending = false;

            // Lock until an actual airborne -> grounded cycle
            // has been observed.
            m_fields->hopLocked = true;
            m_fields->hasLeftGround = false;

            return;
        }
    }

    void resetLevel() {
        m_fields->elapsed = 0.0;
        m_fields->hopPending = false;
        m_fields->hopLocked = false;
        m_fields->hasLeftGround = false;

        PlayLayer::resetLevel();
    }
};
