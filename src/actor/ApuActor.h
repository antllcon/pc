#pragma once
#include "src/actor/BaseActor.h"
#include "src/common/Types.h"

class ApuActor final : public BaseActor
{
public:
	ApuActor(
		Bank& bank,
		AccountId accountId,
		AccountId powerPlantAccountId,
		ILogger& logger);

	void Step() override;

private:
	AccountId m_powerPlantAccountId;
};