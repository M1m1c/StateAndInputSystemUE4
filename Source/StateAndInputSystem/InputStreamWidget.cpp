// Fill out your copyright notice in the Description page of Project Settings.

#include "InputStreamWidget.h"
#include "SamplePlayerController.h"
#include "Runtime/UMG/Public/UMG.h"
#include "Runtime/UMG/Public/UMGStyle.h"
#include "Runtime/UMG/Public/Slate/SObjectWidget.h"
#include "Runtime/UMG/Public/IUMGModule.h"
#include "Runtime/UMG/Public/Blueprint/UserWidget.h"
#include "Styling/SlateBrush.h"
#include "InputFrameWidget.h"
#include "InputWidget.h"
#include "UITextureCollection.h"



void UInputStreamWidget::Setup(UUITextureCollection * textureCollection)
{
	UITextureCollection = textureCollection;
}

void UInputStreamWidget::UpdateInputGraphics(ASamplePlayerController * player, UClass* inputFrameWidgetClass)
{
	if (player == nullptr) { return; }
	if (UITextureCollection == nullptr) { return; }

	TArray<FInputFrame>& inputStream = player->GetInputStreamReference();

	if (inputStreamBox->GetChildrenCount() >= 16)
	{
		inputStreamBox->ClearChildren();
	}

	if (inputStream.Num())
	{
		auto frameWidget = ConstructFrameWidget(player, inputFrameWidgetClass, inputStream.Last());
		inputStreamBox->AddChild(frameWidget);
	}

}

bool UInputStreamWidget::ShouldClearInputStreamWidget(TArray<FInputFrame> & inputStream)
{
	bool retflag = true;
	if (inputStream.Num())
	{
		for (int32 g = 0; g < 2; g++)
		{
			if (!inputStream.IsValidIndex(g)) { continue; }
			if (!inputStream[g].ContainedButtons.Num()) { continue; }

			for (int32 i = 0; i < inputStream[g].ContainedButtons.Num(); i++)
			{
				if (inputStream[g].ContainedButtons[i].ButtonState == EButtonState::Released)
				{
					retflag = false;
					break;
				}
			}
		}

	}
	return retflag;
}

UInputFrameWidget* UInputStreamWidget::ConstructFrameWidget(
	ASamplePlayerController * player,
	UClass* inputFrameWidgetClass,
	FInputFrame& inputFrame)
{
	auto frameWidget = CreateWidget<UInputFrameWidget>(player, inputFrameWidgetClass);
	frameWidget->Setup();
	auto slots = frameWidget->Slots;
	bool directionDone = false;

	for (int32 i = 0; i < slots.Num(); i++)
	{
		if (slots[i] == nullptr) { continue; }
		auto inputImage = slots[i]->InputImage;
		if (inputImage == nullptr) { continue; }


		if (inputFrame.DirectionalInput != EInputDirections::None && directionDone == false)
		{
			directionDone = true;
			SetDirectionalTexture(inputFrame, inputImage);
		}

		switch (i)
		{
		case 1:
			inputImage->Brush.SetResourceObject(UITextureCollection->Img_Input_Pressed_LeftFaceButton);
			SetActiveButton(inputFrame, inputImage, EInputButtons::LeftFace);
			break;
		case 2:
			inputImage->Brush.SetResourceObject(UITextureCollection->Img_Input_Pressed_TopFaceButton);
			SetActiveButton(inputFrame, inputImage, EInputButtons::TopFace);
			break;
		case 3:
			inputImage->Brush.SetResourceObject(UITextureCollection->Img_Input_Pressed_RightFaceButton);
			SetActiveButton(inputFrame, inputImage, EInputButtons::RightFace);
			break;
		case 4:
			inputImage->Brush.SetResourceObject(UITextureCollection->Img_Input_Pressed_BottomFaceButton);
			SetActiveButton(inputFrame, inputImage, EInputButtons::BottomFace);
			break;
		}

		if (inputImage->Brush.GetResourceObject() == nullptr)
			slots[i]->InputImage->SetOpacity(0.0f);
	}

	return frameWidget;
}

void UInputStreamWidget::SetDirectionalTexture(FInputFrame & inputFrame, UImage * inputImage)
{
	switch (inputFrame.DirectionalInput)
	{
	case EInputDirections::Up:
		inputImage->Brush.SetResourceObject(UITextureCollection->Img_Input_UpDirection);
		break;
	case EInputDirections::UpLeft:
		inputImage->Brush.SetResourceObject(UITextureCollection->Img_Input_UpLeftDirection);
		break;
	case EInputDirections::UpRight:
		inputImage->Brush.SetResourceObject(UITextureCollection->Img_Input_UpRightDirection);
		break;
	case EInputDirections::Down:
		inputImage->Brush.SetResourceObject(UITextureCollection->Img_Input_DownDirection);
		break;
	case EInputDirections::DownLeft:
		inputImage->Brush.SetResourceObject(UITextureCollection->Img_Input_DownLeftDirection);
		break;
	case EInputDirections::DownRight:
		inputImage->Brush.SetResourceObject(UITextureCollection->Img_Input_DownRightDirection);
		break;
	case EInputDirections::Left:
		inputImage->Brush.SetResourceObject(UITextureCollection->Img_Input_LeftDirection);
		break;
	case EInputDirections::Right:
		inputImage->Brush.SetResourceObject(UITextureCollection->Img_Input_RightDirection);
		break;
	}
}

UImage* UInputStreamWidget::SetActiveButton(FInputFrame & inputFrame, UImage * inputImage, EInputButtons inputButton)
{
	UImage* image = inputImage;
	image->Brush.TintColor = FSlateColor(FLinearColor(0.5f, 0.5f, 0.5f, 0.3f));
	if (inputFrame.ContainedButtons.Num())
	{
		for (int32 g = 0; g < inputFrame.ContainedButtons.Num(); g++)
		{
			auto button = inputFrame.ContainedButtons[g];
			if (button.ButtonInput == inputButton)
			{
				image->Brush.TintColor = FSlateColor(FLinearColor(1.0f, 1.0f, 1.0f, 1.0f));
				switch (inputButton)
				{
				case EInputButtons::LeftFace:
					SetButtonTexture(
						image,
						button,
						UITextureCollection->Img_Input_Pressed_LeftFaceButton,
						UITextureCollection->Img_Input_Held_LeftFaceButton,
						UITextureCollection->Img_Input_Released_LeftFaceButton);
					break;
				case EInputButtons::TopFace:
					SetButtonTexture(
						image,
						button,
						UITextureCollection->Img_Input_Pressed_TopFaceButton,
						UITextureCollection->Img_Input_Held_TopFaceButton,
						UITextureCollection->Img_Input_Released_TopFaceButton);
					break;
				case EInputButtons::RightFace:
					SetButtonTexture(
						image,
						button,
						UITextureCollection->Img_Input_Pressed_RightFaceButton,
						UITextureCollection->Img_Input_Held_RightFaceButton,
						UITextureCollection->Img_Input_Released_RightFaceButton);
					break;
				case EInputButtons::BottomFace:
					SetButtonTexture(
						image,
						button,
						UITextureCollection->Img_Input_Pressed_BottomFaceButton,
						UITextureCollection->Img_Input_Held_BottomFaceButton,
						UITextureCollection->Img_Input_Released_BottomFaceButton);
					break;
				case EInputButtons::RightBumper:
					break;
				case EInputButtons::LeftBumper:
					break;
				default:
					break;
				}
				break;
			}
		}
	}
	return image;
}

UImage * UInputStreamWidget::SetButtonTexture(UImage * image, FInputButtonStruct & button, UTexture * pressed, UTexture * held, UTexture * released)
{
	auto img = image;
	if (button.ButtonState == EButtonState::HeldDown)
	{
		img->Brush.SetResourceObject(held);
	}
	else if (button.ButtonState == EButtonState::Released)
	{
		img->Brush.SetResourceObject(released);
	}
	return img;
}
