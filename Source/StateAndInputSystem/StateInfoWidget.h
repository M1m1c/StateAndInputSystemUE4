// © 2018, Ludvig Baummann Olsson. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TextBlock.h"
#include "StateInfoWidget.generated.h"

/**
 * 
 */
UCLASS()
class STATEANDINPUTSYSTEM_API UStateInfoWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(meta = (BindWidget))
		UTextBlock* StateName;

	UPROPERTY(meta = (BindWidget))
		UTextBlock* StateInfo_1;

	UPROPERTY(meta = (BindWidget))
		UTextBlock* StateInfo_2;
	
	
};
