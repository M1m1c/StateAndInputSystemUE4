//  © 2018, Ludvig Baummann Olsson. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "StateBase.h"
#include "SharedStateLinks.generated.h"

/**
 * 
 */
UCLASS()
class STATEANDINPUTSYSTEM_API USharedStateLinks : public UDataAsset
{
	GENERATED_BODY()
	
public:
	// These are the SharedStateLinks that a state can use. They are in priority order,
	//Place the hardest to perform at the top, so that they are more likley to happen.
	UPROPERTY(EditAnywhere)
	TArray<FStateLink> StateLinks;
	
	
	
};
