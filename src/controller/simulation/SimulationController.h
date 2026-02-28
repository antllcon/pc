#pragma once
#include "../../model/simulation/SimulationModel.h"
#include "../IController.h"
#include <memory>

class SimulationController final : public IController
{
public:
	explicit SimulationController(std::shared_ptr<SimulationModel> model);

	void Update(float dt) override;

	void OnMouseMoved(float x, float y);
	void OnCanvasClicked();
	void OnRightArrowPressed();
	void OnLeftArrowPressed();

private:
	std::shared_ptr<SimulationModel> m_model;
};