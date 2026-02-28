#pragma once

class IController
{
public:
	virtual ~IController() = default;
	virtual void Update(float dt) = 0;
};