// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InputClasses.h"
#include "InputStreamWidget.generated.h"

class UImage;
struct FInputFrame;
class ASamplePlayerController;
class UInputFrameWidget;
class UUITextureCollection;
class UVerticalBox;
class UTexture;

UCLASS()
class STATEANDINPUTSYSTEM_API UInputStreamWidget : public UUserWidget
{
	GENERATED_BODY()
public:

		UPROPERTY(meta = (BindWidget))
		UVerticalBox* inputStreamBox;
	

	void Setup(UUITextureCollection* textureCollection);
	void  UpdateInputGraphics(ASamplePlayerController* player, UClass* inputFrameWidgetClass);

private:
	UUITextureCollection* UITextureCollection;

	UInputFrameWidget* ConstructFrameWidget(
		ASamplePlayerController * player,
		UClass* inputFrameWidgetClass,
		FInputFrame& inputFrame);

	void SetDirectionalTexture(FInputFrame & inputFrame, UImage * inputImage);

	UImage* SetActiveButton(FInputFrame & inputFrame, UImage * inputImage, EInputButtons inputButton);
	UImage* SetButtonTexture(UImage * image, FInputButtonStruct & button, UTexture* pressed, UTexture* held, UTexture* released);

	bool ShouldClearInputStreamWidget(TArray<FInputFrame> & inputStream);
	
};
