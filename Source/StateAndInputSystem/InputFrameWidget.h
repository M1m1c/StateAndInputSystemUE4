// © 2018, Ludvig Baummann Olsson. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InputFrameWidget.generated.h"

/**
 *
 */
UCLASS()
class STATEANDINPUTSYSTEM_API UInputFrameWidget : public UUserWidget
{
	GENERATED_BODY()
public:

	void Setup();

	UPROPERTY(meta = (BindWidget))
		class UUniformGridPanel * InputFrameHolder;

	UPROPERTY(meta = (BindWidget))
		class UInputWidget * Slot_0;

	UPROPERTY(meta = (BindWidget))
		class UInputWidget * Slot_1;

	UPROPERTY(meta = (BindWidget))
		class UInputWidget * Slot_2;

	UPROPERTY(meta = (BindWidget))
		class UInputWidget * Slot_3;

	UPROPERTY(meta = (BindWidget))
		class UInputWidget * Slot_4;

	TArray<class UInputWidget*> Slots;

};
