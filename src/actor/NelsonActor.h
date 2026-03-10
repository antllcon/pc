#pragma once
#include "src/actor/BaseActor.h"

class NelsonActor final : public BaseActor
{
public:
	NelsonActor(
		Bank& bank,
		Wallet& bartWallet,
		Wallet& apuWallet,
		ILogger& logger);

	void Step() override;

private:
	Wallet& m_bartWallet;
	Wallet& m_apuWallet;
};