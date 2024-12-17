#ifndef GAMEPLAY_H
#define GAMEPLAY_H

#include "raylib.h"
#include "game_state.h"
#include "resources.h"
#include "maze.h"
#include <vector>
#include <random>

// Particle settings
const int MAX_PORTAL_PARTICLES = 100;
const float PARTICLE_SPAWN_RATE = 0.016f; // Spawn every 16ms
const float PARTICLE_MAX_LIFETIME = 1.0f;
const float PARTICLE_SPEED = 25.0f;

// Powerup settings
const int MAX_TIME_POWERUPS = 5;
const float TIME_BONUS_FEEDBACK_DURATION = 2.0f; // Show feedback for 2 seconds

const float SPEED_BOOST_DURATION = 5.0f;  // 5 seconds
const float SPEED_BOOST_MULTIPLIER = 1.75f; // 75% speed boost


struct Particle {
    Vector2 position;
    Vector2 velocity;
    Color color;
    float lifetime;
    bool active;
    
    Particle();
};

struct TimePowerup {
    int x, y;
    bool active;
    float rotationAngle;
    
    TimePowerup();
};

struct SpeedBoost {
    int x, y;
    bool active;
    
    SpeedBoost();
};

extern std::vector<TimePowerup> timePowerups;
extern std::vector<SpeedBoost> speedBoosts;  // 5 speed boosts max

struct DifficultySettings {
    int cellSize;
    float obstaclePercentage;
    float timeBonusAmount;
    
    static DifficultySettings Get(Difficulty diff);
};

class PortalEffect {
public:
    PortalEffect(int maxParticles = MAX_PORTAL_PARTICLES) 
        : particles(maxParticles), spawnTimer(0), rng(std::random_device{}()) {}

    void SetPosition(Vector2 pos, float r) {
        center = pos;
        radius = r;
    }

    void Update(float deltaTime) {
        spawnTimer -= deltaTime;

        // Reuse inactive particles when spawning
        if (spawnTimer <= 0) {
            SpawnParticle();
            spawnTimer = PARTICLE_SPAWN_RATE;
        }

        // Update all active particles
        for (auto& p : particles) {
            if (!p.active) continue;

            p.lifetime -= deltaTime;
            if (p.lifetime <= 0) {
                p.active = false;  // Mark particle as inactive
                continue;
            }

            p.position.x += p.velocity.x * deltaTime;
            p.position.y += p.velocity.y * deltaTime;

            float alpha = p.lifetime / PARTICLE_MAX_LIFETIME;
            p.color.a = static_cast<unsigned char>(255.0f * alpha);
        }
    }

    void Draw() {
        for (const auto& p : particles) {
            if (p.active) {
                DrawCircleV(p.position, 2, p.color);  // Draw only active particles
            }
        }
    }

private:
    std::vector<Particle> particles;  // Pre-allocated pool of particles
    float spawnTimer;
    Vector2 center;
    float radius;
    std::mt19937 rng;

    void SpawnParticle() {
        // Find an inactive particle from the pool
        for (auto& p : particles) {
            if (p.active) continue;  // Skip active particles

            // Random angle around circle
            std::uniform_real_distribution<float> angleDist(0, 2 * PI);
            float angle = angleDist(rng);

            // Random distance from center within radius
            std::uniform_real_distribution<float> radiusDist(0, radius);
            float distance = radiusDist(rng);

            // Set particle properties
            p.position.x = center.x + cosf(angle) * distance;
            p.position.y = center.y + sinf(angle) * distance;
            p.velocity.x = cosf(angle) * PARTICLE_SPEED;
            p.velocity.y = sinf(angle) * PARTICLE_SPEED;
            p.color = ColorAlpha(GAME_STATE.getCurrentColors().goalColor, 1.0f);
            p.lifetime = PARTICLE_MAX_LIFETIME;
            p.active = true;
            break;  // Spawn only one particle at a time
        }
    }
};

void ResetGame();
void InitializeGameWithDifficulty();
void PrecomputeValidCells(std::vector<std::pair<int, int>>& validCells);
void SpawnPowerups();
void UpdateTimePowerups(float deltaTime);
void DrawTimePowerups();
void UpdateSpeedBoosts(float deltaTime);
void DrawSpeedBoosts();
void PlaceRandomExit();
void DrawExit(int exitX, int exitY, float time);
bool CanMove(int direction);
float Lerp(float start, float end, float amount);
void MovePlayer();
void UpdatePlayerRotation();

#endif