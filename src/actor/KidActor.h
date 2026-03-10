#pragma once
#include "src/actor/BaseActor.h"

class KidActor final : public BaseActor
{
public:
	KidActor(
		std::string name,
		Bank& bank,
		Wallet& apuWallet,
		ILogger& logger);

	void Step() override;

private:
	Wallet& m_apuWallet;
};