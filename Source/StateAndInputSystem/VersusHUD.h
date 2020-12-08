// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "VersusHUD.generated.h"

/**
 *
 */
UCLASS()
class STATEANDINPUTSYSTEM_API AVersusHUD : public AHUD
{
	GENERATED_BODY()
public:
	AVersusHUD();

	void LoadVersusHUD(APlayerController* player1);

	void AddPlayerSpecificDynamics(ASamplePlayerController * controller);

	UFUNCTION()
	void UpdateInputStream(class ASamplePlayerController * player);

protected:
	class UClass * InputStreamWidgetClass;
	class UInputStreamWidget * InputStreamWidget;

	class UClass * InputFrameWidgetClass;
	bool AddInputStreamToView(class APlayerController* player);

	UPROPERTY(EditAnywhere)
	class UUITextureCollection* UITextureCollection;
};