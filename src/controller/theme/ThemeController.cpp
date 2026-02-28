#include "ThemeController.h"

ThemeController::ThemeController(std::shared_ptr<ThemeModel> model)
	: m_model(std::move(model))
{
}

void ThemeController::Update(float)
{
}

void ThemeController::OnToggleClicked()
{
	m_model->Invert();
}