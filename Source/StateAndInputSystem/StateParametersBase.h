// Fill out your copyright notice in the Description page of Project Settings.

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
