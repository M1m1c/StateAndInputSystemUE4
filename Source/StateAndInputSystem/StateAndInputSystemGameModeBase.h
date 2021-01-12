// © 2018, Ludvig Baummann Olsson. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "StateAndInputSystemGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class STATEANDINPUTSYSTEM_API AStateAndInputSystemGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
	
protected:

	virtual void BeginPlay() override;
	
};
