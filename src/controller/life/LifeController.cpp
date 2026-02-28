#include "LifeController.h"
#include <stdexcept>

namespace
{
constexpr float CELL_SIZE = 10.0f;

void AssertIsModelValid(const std::shared_ptr<LifeModel>& model)
{
	if (!model)
	{
		throw std::runtime_error("Указатель на модель игры не может быть пустым");
	}
}

unsigned int MapToGrid(float coordinate)
{
	if (coordinate < 0.0f)
	{
		return 0;
	}
	return static_cast<unsigned int>(coordinate / CELL_SIZE);
}
}

LifeController::LifeController(std::shared_ptr<LifeModel> model)
	: m_model(std::move(model))
	, m_lastToggledX(-1)
	, m_lastToggledY(-1)
{
	AssertIsModelValid(m_model);
}

void LifeController::Update(float dt)
{
	m_model->Update(dt);
}

void LifeController::OnMousePressed(float x, float y)
{
	unsigned int gridX = MapToGrid(x);
	unsigned int gridY = MapToGrid(y);

	const auto& data = m_model->GetData();
	if (gridX < data.width && gridY < data.height)
	{
		m_model->ToggleCell(gridX, gridY);
		m_lastToggledX = static_cast<int>(gridX);
		m_lastToggledY = static_cast<int>(gridY);
	}
}

void LifeController::OnMouseMoved(float x, float y, bool isLeftButtonPressed)
{
	if (!isLeftButtonPressed)
	{
		m_lastToggledX = -1;
		m_lastToggledY = -1;
		return;
	}

	unsigned int gridX = MapToGrid(x);
	unsigned int gridY = MapToGrid(y);

	const auto& data = m_model->GetData();
	if (gridX < data.width && gridY < data.height)
	{
		if (static_cast<int>(gridX) != m_lastToggledX || static_cast<int>(gridY) != m_lastToggledY)
		{
			m_model->ToggleCell(gridX, gridY);
			m_lastToggledX = static_cast<int>(gridX);
			m_lastToggledY = static_cast<int>(gridY);
		}
	}
}

void LifeController::OnSpacePressed()
{
	m_model->TogglePause();
}

void LifeController::OnRPressed()
{
	m_model->Randomize();
}

void LifeController::OnCPressed()
{
	m_model->Clear();
}