#pragma once
#include "src/actor/BaseActor.h"
#include "src/common/Types.h"

class HomerActor final : public BaseActor
{
public:
	HomerActor(
			Bank& bank,
			AccountId accountId,
			AccountId margeAccountId,
			AccountId powerPlantAccountId,
			Wallet& bartWallet,
			Wallet& lisaWallet,
			ILogger& logger);

	void Step() override;

private:
	AccountId m_margeAccountId;
	AccountId m_powerPlantAccountId;
	Wallet& m_bartWallet;
	Wallet& m_lisaWallet;
};