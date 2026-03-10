#pragma once
#include "src/actor/BaseActor.h"
#include "src/common/Types.h"
#include <atomic>

class SmithersActor final : public BaseActor
{
public:
	SmithersActor(
		Bank& bank,
		AccountId accountId,
		AccountId apuAccountId,
		std::atomic<AccountId>& sharedBurnsAccountId,
		ILogger& logger);

	void Step() override;

private:
	AccountId m_apuAccountId;
	std::atomic<AccountId>& m_sharedBurnsAccountId;
};