#pragma once
#include "../../model/life/LifeModel.h"
#include "../IController.h"
#include <memory>

class LifeController final : public IController
{
public:
	explicit LifeController(std::shared_ptr<LifeModel> model);

	void Update(float dt) override;

	void OnMousePressed(float x, float y);
	void OnMouseMoved(float x, float y, bool isLeftButtonPressed);
	void OnSpacePressed();
	void OnRPressed();
	void OnCPressed();

private:
	std::shared_ptr<LifeModel> m_model;
	int m_lastToggledX;
	int m_lastToggledY;
};