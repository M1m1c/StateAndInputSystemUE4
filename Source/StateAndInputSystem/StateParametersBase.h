// © 2018, Ludvig Baummann Olsson. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "StateParametersBase.generated.h"

/**
 * 
 */
UCLASS()
class STATEANDINPUTSYSTEM_API UStateParametersBase : public UDataAsset
{
	GENERATED_BODY()
	
public:

	UPROPERTY(EditAnywhere)
		int32 Damage;
	
	UPROPERTY(EditAnywhere)
		FName Info;
	
	
};
