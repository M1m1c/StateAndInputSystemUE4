// Fill out your copyright notice in the Description page of Project Settings.

#include "VersusHUD.h"
#include "Runtime/UMG/Public/UMG.h"
#include "Runtime/UMG/Public/UMGStyle.h"
#include "Runtime/UMG/Public/Slate/SObjectWidget.h"
#include "Runtime/UMG/Public/IUMGModule.h"
#include "Runtime/UMG/Public/Blueprint/UserWidget.h"
#include "Blueprint/UserWidget.h"
#include "UITextureCollection.h"
#include "InputStreamWidget.h"
#include "SamplePlayerController.h"

AVersusHUD::AVersusHUD()
{

	static ConstructorHelpers::FClassFinder<UUserWidget> InputStreamWidgetObj(TEXT("/Game/UMG/BP_InputStreamWidget"));
	if (InputStreamWidgetObj.Succeeded())
	{
		InputStreamWidgetClass = InputStreamWidgetObj.Class;
	}

	static ConstructorHelpers::FClassFinder<UUserWidget> InputFrameWidgetObj(TEXT("/Game/UMG/BP_InputFrameWidget"));
	if (InputFrameWidgetObj.Succeeded())
	{
		InputFrameWidgetClass = InputFrameWidgetObj.Class;
	}
}

void AVersusHUD::LoadVersusHUD(APlayerController* player1)
{


	if (player1 == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("%s @LoadVersusHUD no player to add hud to"), *GetName());
		return;
	}

	if (!AddInputStreamToView(player1))
	{
		UE_LOG(LogTemp, Warning, TEXT("%s @LoadVersusHUD No InputStreamWidgetClass set, InputStreamWidget was not added to viewport"), *GetName());
	}


	AddPlayerSpecificDynamics(Cast<ASamplePlayerController>(player1));
}

void AVersusHUD::AddPlayerSpecificDynamics(ASamplePlayerController * controller)
{
	if (controller == nullptr) { return; }

	controller->OnInputStreamUpdate.AddDynamic(this, &AVersusHUD::UpdateInputStream);
}


bool AVersusHUD::AddInputStreamToView(APlayerController* player)
{
	bool retflag = false;
	if (InputStreamWidgetClass)
	{
		InputStreamWidget = CreateWidget<UInputStreamWidget>(player, this->InputStreamWidgetClass);
		InputStreamWidget->AddToViewport();
		InputStreamWidget->Setup(UITextureCollection);
		retflag = true;
	}
	return retflag;
}

void AVersusHUD::UpdateInputStream(ASamplePlayerController * player)
{
	if (!InputStreamWidget) { return; }
	if (!InputFrameWidgetClass) { return; }
	InputStreamWidget->UpdateInputGraphics(player, InputFrameWidgetClass);
}

