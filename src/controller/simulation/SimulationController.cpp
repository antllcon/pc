#include "SimulationController.h"
#include <stdexcept>

namespace
{
void AssertIsModelValid(const std::shared_ptr<SimulationModel>& model)
{
	if (!model)
	{
		throw std::runtime_error("Указатель на модель симуляции не может быть пустым");
	}
}
}

SimulationController::SimulationController(std::shared_ptr<SimulationModel> model)
	: m_model(std::move(model))
{
	AssertIsModelValid(m_model);
}

void SimulationController::Update(float dt)
{
	m_model->Update(dt);
}

void SimulationController::OnMouseMoved(float x, float y)
{
	m_model->SetTarget(x, y);
}

void SimulationController::OnCanvasClicked()
{
	m_model->Shoot();
}

void SimulationController::OnRightArrowPressed()
{
	m_model->IncreaseTimeScale();
}

void SimulationController::OnLeftArrowPressed()
{
	m_model->DecreaseTimeScale();
}