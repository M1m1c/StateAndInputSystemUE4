// Fill out your copyright notice in the Description page of Project Settings.

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
