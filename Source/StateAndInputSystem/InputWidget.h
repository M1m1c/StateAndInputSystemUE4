// © 2018, Ludvig Baummann Olsson. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InputWidget.generated.h"

/**
 *
 */
UCLASS()
class STATEANDINPUTSYSTEM_API UInputWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(meta = (BindWidget))
	class UImage* InputImage;


};
