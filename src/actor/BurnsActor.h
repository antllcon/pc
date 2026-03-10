#pragma once
#include "src/actor/BaseActor.h"
#include "src/common/Types.h"
#include <atomic>

class BurnsActor final : public BaseActor
{
public:
	BurnsActor(
		Bank& bank,
		AccountId accountId,
		AccountId homerAccountId,
		std::atomic<AccountId>& smithersAccountId,
		ILogger& logger);

	void Step() override;

private:
	AccountId m_homerAccountId;
	std::atomic<AccountId>& m_smithersAccountId;
};