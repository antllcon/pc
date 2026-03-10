#pragma once
#include "src/actor/BaseActor.h"
#include "src/common/Types.h"

class MargeActor final : public BaseActor
{
public:
	MargeActor(
		Bank& bank,
		AccountId accountId,
		AccountId apuAccountId,
		ILogger& logger);

	void Step() override;

private:
	AccountId m_apuAccountId;
};