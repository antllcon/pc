#pragma once
#include "src/actor/BaseActor.h"
#include "src/common/Types.h"

class SnakeActor final : public BaseActor
{
public:
	SnakeActor(
		Bank& bank,
		AccountId accountId,
		AccountId homerAccountId,
		AccountId apuAccountId,
		ILogger& logger);

	void Step() override;

private:
	AccountId m_homerAccountId;
	AccountId m_apuAccountId;
};