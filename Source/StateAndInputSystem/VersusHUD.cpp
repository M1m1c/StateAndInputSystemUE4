// © 2018, Ludvig Baummann Olsson. All rights reserved.

#include "VersusHUD.h"
#include "Runtime/UMG/Public/UMG.h"
#include "Runtime/UMG/Public/UMGStyle.h"
#include "Runtime/UMG/Public/Slate/SObjectWidget.h"
#include "Runtime/UMG/Public/IUMGModule.h"
#include "Runtime/UMG/Public/Blueprint/UserWidget.h"
#include "Blueprint/UserWidget.h"
#include "UITextureCollection.h"
#include "InputStreamWidget.h"
#include "StateInfoWidget.h"
#include "SamplePlayerController.h"
#include "SampleCharacter.h"
#include "StateParametersBase.h"

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

	static ConstructorHelpers::FClassFinder<UUserWidget> StateInfoWidgetObj(TEXT("/Game/UMG/BP_StateInfoWidget"));
	if (StateInfoWidgetObj.Succeeded())
	{
		StateInfoWidgetClass = StateInfoWidgetObj.Class;
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

	if (!AddStateInfoWidgetToView(player1))
	{
		UE_LOG(LogTemp, Warning, TEXT("%s @LoadVersusHUD No StateInfoWidgetClass set, StateInfoWidget was not added to viewport"), *GetName());
	}


	AddPlayerSpecificDynamics(Cast<ASamplePlayerController>(player1));
}

void AVersusHUD::AddPlayerSpecificDynamics(ASamplePlayerController * controller)
{
	if (controller == nullptr) { return; }

	controller->OnInputStreamUpdate.AddDynamic(this, &AVersusHUD::UpdateInputStream);

	auto character = Cast<ASampleCharacter>(controller->GetCharacter());
	if (character == nullptr) { return; }

	character->OnStateInfoUpdate.AddDynamic(this, &AVersusHUD::UpdateStateInfo);

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

bool AVersusHUD::AddStateInfoWidgetToView(APlayerController * player)
{
	bool retflag = false;
	if (StateInfoWidgetClass)
	{
		StateInfoWidget = CreateWidget<UStateInfoWidget>(player, this->StateInfoWidgetClass);
		StateInfoWidget->AddToViewport();
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

void AVersusHUD::UpdateStateInfo(FText stateNameText, UStateParametersBase * params)
{
	if (!StateInfoWidget) { return; }
	StateInfoWidget->StateName->SetText(stateNameText);

	if (!params) { return; }
	FString dmg = "Damage: " + FString::FromInt(params->Damage);
	StateInfoWidget->StateInfo_1->SetText(FText::FromString(dmg));
	StateInfoWidget->StateInfo_2->SetText(FText::FromName(params->Info));
}

