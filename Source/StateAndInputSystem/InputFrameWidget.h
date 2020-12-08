// Fill out your copyright notice in the Description page of Project Settings.

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

	//TODO assign this array to correspond with bthe above slots as soon as this is instantiated
	TArray<class UInputWidget*> Slots;

};
