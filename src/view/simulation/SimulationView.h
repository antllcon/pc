#pragma once
#include "../../model/simulation/SimulationModel.h"
#include "../IView.h"
#include "SFML/Graphics/CircleShape.hpp"
#include "SFML/Graphics/RectangleShape.hpp"
#include "src/system/Observer.h"
#include <memory>
#include <vector>

class SimulationController;

class SimulationView final
	: public IView
	, public IObserver<SimulationData>
{
public:
	SimulationView(std::shared_ptr<SimulationModel> model, std::shared_ptr<SimulationController> controller);

	void HandleEvent(const sf::Event& event, const sf::RenderWindow& window) override;
	void Render(sf::RenderWindow& window) const override;
	void Update(const SimulationData& data, IObservable<SimulationData>* subject) override;

private:
	void UpdateBalls(const std::vector<BallData>& balls);
	void UpdateBlocks(const std::vector<BlockData>& blocks);
	void UpdateGun(const GunData& gun);

	std::shared_ptr<SimulationController> m_controller;
	std::vector<sf::CircleShape> m_balls;
	std::vector<sf::RectangleShape> m_blocks;
	sf::RectangleShape m_gun;
};