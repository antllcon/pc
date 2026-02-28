#include "SimulationModel.h"
#include "src/system/AppConfig.h"
#include <algorithm>
#include <cmath>
#include <numbers>
#include <random>
#include <stdexcept>

namespace
{
constexpr size_t MAX_BALLS = 50;
constexpr float FREE_FALL_ACCELERATION = 980.0f;
constexpr float BOUNCE_DAMPING = 0.8f;
constexpr float BALL_RADIUS = 8.0f;
constexpr float INITIAL_SPEED = 700.0f;

constexpr float DEFAULT_TIME_SCALE = 1.0f;
constexpr float TIME_SCALE_STEP = 0.2f;
constexpr float MIN_TIME_SCALE = 0.2f;
constexpr float MAX_TIME_SCALE = 3.0f;

constexpr sf::Vector2f DEFAULT_GUN_DIRECTION = {0.0f, -1.0f};

constexpr size_t RANDOM_BLOCK_COUNT = 5;
constexpr float SAFE_ZONE_RADIUS = 120.0f;

constexpr float SPAWN_ZONE_MIN_X = 50.0f;
constexpr float SPAWN_ZONE_MAX_X = 350.0f;
constexpr float SPAWN_ZONE_MIN_Y = 150.0f;
constexpr float SPAWN_ZONE_MAX_Y = 500.0f;

constexpr float MIN_BLOCK_WIDTH = 40.0f;
constexpr float MAX_BLOCK_WIDTH = 120.0f;
constexpr float MIN_BLOCK_HEIGHT = 15.0f;
constexpr float MAX_BLOCK_HEIGHT = 30.0f;
constexpr float MIN_ANGLE = -std::numbers::pi_v<float>;
constexpr float MAX_ANGLE = std::numbers::pi_v<float>;
constexpr float EPSILON = std::numeric_limits<float>::epsilon();

void AssertIsNonNegativeDeltaTime(float dt)
{
	if (dt < 0.0f)
	{
		throw std::runtime_error("Время дельты не может быть отрицательным");
	}
}

float CalculateDistance(const sf::Vector2f& p1, const sf::Vector2f& p2)
{
	float dx = p2.x - p1.x;
	float dy = p2.y - p1.y;

	return std::sqrt(dx * dx + dy * dy);
}

sf::Vector2f NormalizeVector(const sf::Vector2f& v)
{
	float length = std::sqrt(v.x * v.x + v.y * v.y);

	if (length == EPSILON)
	{
		return {0.0f, 0.0f};
	}

	return {v.x / length, v.y / length};
}

sf::Vector2f RotateVector(const sf::Vector2f& v, float angle)
{
	float cosAngle = std::cos(angle);
	float sinAngle = std::sin(angle);

	return {v.x * cosAngle - v.y * sinAngle, v.x * sinAngle + v.y * cosAngle};
}

float DotProduct(const sf::Vector2f& a, const sf::Vector2f& b)
{
	return a.x * b.x + a.y * b.y;
}

void ResolveBallToBallCollision(BallData& b1, BallData& b2)
{
	float dist = CalculateDistance(b1.position, b2.position);
	float minDist = b1.radius + b2.radius;

	if (dist < minDist && dist > EPSILON)
	{
		sf::Vector2f normal = {(b2.position.x - b1.position.x) / dist, (b2.position.y - b1.position.y) / dist};
		float overlap = minDist - dist;

		b1.position.x -= normal.x * overlap * 0.5f;
		b2.position.x += normal.x * overlap * 0.5f;
		b1.position.y -= normal.y * overlap * 0.5f;
		b2.position.y += normal.y * overlap * 0.5f;

		sf::Vector2f relativeSpeed = {b2.speed.x - b1.speed.x, b2.speed.y - b1.speed.y};
		float speedAlongNormal = DotProduct(relativeSpeed, normal);

		if (speedAlongNormal < 0.0f)
		{
			float impulse = -(1.0f + BOUNCE_DAMPING) * speedAlongNormal / 2.0f;

			b1.speed.x -= impulse * normal.x;
			b2.speed.x += impulse * normal.x;
			b1.speed.y -= impulse * normal.y;
			b2.speed.y += impulse * normal.y;
		}
	}
}

void ResolveBallToBlockCollision(BallData& ball, const BlockData& block)
{
	sf::Vector2f localCircleCenter = {ball.position.x - block.center.x, ball.position.y - block.center.y};
	localCircleCenter = RotateVector(localCircleCenter, -block.rotationAngle);

	float halfW = block.size.x / 2.0f;
	float halfH = block.size.y / 2.0f;

	sf::Vector2f closestPoint = {
		std::clamp(localCircleCenter.x, -halfW, halfW),
		std::clamp(localCircleCenter.y, -halfH, halfH)};

	sf::Vector2f localDelta = {localCircleCenter.x - closestPoint.x, localCircleCenter.y - closestPoint.y};
	float distSq = localDelta.x * localDelta.x + localDelta.y * localDelta.y;

	if (distSq > EPSILON && distSq <= ball.radius * ball.radius)
	{
		float dist = std::sqrt(distSq);
		sf::Vector2f localNormal = {localDelta.x / dist, localDelta.y / dist};
		sf::Vector2f worldNormal = RotateVector(localNormal, block.rotationAngle);

		float overlap = ball.radius - dist;
		ball.position.x += worldNormal.x * overlap;
		ball.position.y += worldNormal.y * overlap;

		float speedAlongNormal = DotProduct(ball.speed, worldNormal);

		if (speedAlongNormal < 0.0f)
		{
			float impulse = -(1.0f + BOUNCE_DAMPING) * speedAlongNormal;
			ball.speed.x += impulse * worldNormal.x;
			ball.speed.y += impulse * worldNormal.y;
		}
	}
}

void ResolveBallToWallsCollision(BallData& ball, float width)
{
	if (ball.position.x - ball.radius < 0.0f)
	{
		ball.position.x = ball.radius;
		ball.speed.x = -ball.speed.x * BOUNCE_DAMPING;
		ball.speed.y *= BOUNCE_DAMPING;
	}
	else if (ball.position.x + ball.radius > width)
	{
		ball.position.x = width - ball.radius;
		ball.speed.x = -ball.speed.x * BOUNCE_DAMPING;
		ball.speed.y *= BOUNCE_DAMPING;
	}

	if (ball.position.y - ball.radius < 0.0f)
	{
		ball.position.y = ball.radius;
		ball.speed.y = -ball.speed.y * BOUNCE_DAMPING;
		ball.speed.x *= BOUNCE_DAMPING;
	}
}

float GetRandomFloat(float min, float max)
{
	static std::mt19937 rng(std::random_device{}());
	std::uniform_real_distribution<float> dist(min, max);

	return dist(rng);
}

bool IsPositionSafe(const sf::Vector2f& position, const sf::Vector2f& gunPosition)
{
	return CalculateDistance(position, gunPosition) > SAFE_ZONE_RADIUS;
}
std::vector<BlockData> CreateLevel(const sf::Vector2f& gunPosition)
{
	std::vector<BlockData> level;
	level.reserve(5 + RANDOM_BLOCK_COUNT);

	level.emplace_back(sf::Vector2f{200.0f, 750.0f}, sf::Vector2f{120.0f, 20.0f}, 0.0f);
	level.emplace_back(sf::Vector2f{150.0f, 700.0f}, sf::Vector2f{20.0f, 100.0f}, 0.0f);
	level.emplace_back(sf::Vector2f{250.0f, 700.0f}, sf::Vector2f{20.0f, 100.0f}, 0.0f);
	level.emplace_back(sf::Vector2f{80.0f, 500.0f}, sf::Vector2f{100.0f, 20.0f}, 0.785f);
	level.emplace_back(sf::Vector2f{320.0f, 500.0f}, sf::Vector2f{100.0f, 20.0f}, -0.785f);

	for (size_t i = 0; i < RANDOM_BLOCK_COUNT; ++i)
	{
		sf::Vector2f position;

		do
		{
			position.x = GetRandomFloat(SPAWN_ZONE_MIN_X, SPAWN_ZONE_MAX_X);
			position.y = GetRandomFloat(SPAWN_ZONE_MIN_Y, SPAWN_ZONE_MAX_Y);
		} while (!IsPositionSafe(position, gunPosition));

		sf::Vector2f size;
		size.x = GetRandomFloat(MIN_BLOCK_WIDTH, MAX_BLOCK_WIDTH);
		size.y = GetRandomFloat(MIN_BLOCK_HEIGHT, MAX_BLOCK_HEIGHT);
		float angle = GetRandomFloat(MIN_ANGLE, MAX_ANGLE);

		level.emplace_back(position, size, angle);
	}

	return level;
}
} // namespace

SimulationModel::SimulationModel()
	: m_timeScale(DEFAULT_TIME_SCALE)
{
	m_data.gun.position = AppConfig::GetWindowCenter();
	m_data.gun.direction = DEFAULT_GUN_DIRECTION;
	m_data.blocks = CreateLevel(m_data.gun.position);
}

void SimulationModel::Update(float dt)
{
	AssertIsNonNegativeDeltaTime(dt);

	if (dt == 0.0f)
	{
		return;
	}

	float scaledDt = dt * m_timeScale;

	ResolveCollisions();
	ApplyGravity(scaledDt);
	MoveBalls(scaledDt);
	RemoveFallenBalls();
	NotifyObservers();
}

void SimulationModel::SetTarget(float x, float y)
{
	sf::Vector2f direction = {x - m_data.gun.position.x, y - m_data.gun.position.y};
	m_data.gun.direction = NormalizeVector(direction);
	NotifyObservers();
}

void SimulationModel::Shoot()
{
	if (m_data.balls.size() >= MAX_BALLS)
	{
		return;
	}

	BallData newBall;
	newBall.position = m_data.gun.position;
	newBall.speed = {m_data.gun.direction.x * INITIAL_SPEED, m_data.gun.direction.y * INITIAL_SPEED};
	newBall.radius = BALL_RADIUS;

	m_data.balls.emplace_back(newBall);
	NotifyObservers();
}

void SimulationModel::IncreaseTimeScale()
{
	if (m_timeScale + TIME_SCALE_STEP <= MAX_TIME_SCALE)
	{
		m_timeScale += TIME_SCALE_STEP;
	}
}

void SimulationModel::DecreaseTimeScale()
{
	if (m_timeScale - TIME_SCALE_STEP >= MIN_TIME_SCALE)
	{
		m_timeScale -= TIME_SCALE_STEP;
	}
}

const SimulationData& SimulationModel::GetData() const
{
	return m_data;
}

SimulationData SimulationModel::GetChangedData() const
{
	return m_data;
}

void SimulationModel::ApplyGravity(float dt)
{
	for (auto& ball : m_data.balls)
	{
		ball.speed.y += FREE_FALL_ACCELERATION * dt;
	}
}

void SimulationModel::MoveBalls(float dt)
{
	for (auto& ball : m_data.balls)
	{
		ball.position.x += ball.speed.x * dt;
		ball.position.y += ball.speed.y * dt;
	}
}

void SimulationModel::ResolveCollisions()
{
	for (size_t i = 0; i < m_data.balls.size(); ++i)
	{
		ResolveBallToWallsCollision(m_data.balls[i], static_cast<float>(AppConfig::WINDOW_WIDTH));

		for (const auto& block : m_data.blocks)
		{
			ResolveBallToBlockCollision(m_data.balls[i], block);
		}

		for (size_t j = i + 1; j < m_data.balls.size(); ++j)
		{
			ResolveBallToBallCollision(m_data.balls[i], m_data.balls[j]);
		}
	}
}

void SimulationModel::RemoveFallenBalls()
{
	auto it = m_data.balls.begin();

	while (it != m_data.balls.end())
	{
		if (it->position.y - it->radius > static_cast<float>(AppConfig::WINDOW_HEIGHT))
		{
			it = m_data.balls.erase(it);
		}
		else
		{
			++it;
		}
	}
}