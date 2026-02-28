#include "ToastController.h"
#include <stdexcept>

namespace
{
constexpr float TOAST_DURATION_SECONDS = 3.0f;

void AssertIsNotEmpty(const std::string& text)
{
	if (text.empty())
	{
		throw std::runtime_error("Текст уведомления не может быть пустым");
	}
}
}

ToastController::ToastController(std::shared_ptr<ToastModel> model)
	: m_model(std::move(model))
	, m_timer(0.0f)
{
}

void ToastController::Update(float dt)
{
	if (m_timer > 0.0f)
	{
		m_timer -= dt;
		if (m_timer <= 0.0f)
		{
			m_timer = 0.0f;
			m_model->Hide();
		}
	}
}

void ToastController::ShowError(const std::string& errorMessage)
{
	AssertIsNotEmpty(errorMessage);
	m_timer = TOAST_DURATION_SECONDS;
	m_model->Show(errorMessage);
}