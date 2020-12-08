// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Engine/Texture.h"
#include "UITextureCollection.generated.h"

/**
 * 
 */
UCLASS()
class STATEANDINPUTSYSTEM_API UUITextureCollection : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere)
		UTexture* Img_Input_Pressed_TopFaceButton;
	UPROPERTY(EditAnywhere)
		UTexture* Img_Input_Pressed_BottomFaceButton;
	UPROPERTY(EditAnywhere)
		UTexture* Img_Input_Pressed_LeftFaceButton;
	UPROPERTY(EditAnywhere)
		UTexture* Img_Input_Pressed_RightFaceButton;
	UPROPERTY(EditAnywhere)
		UTexture* Img_Input_Released_TopFaceButton;
	UPROPERTY(EditAnywhere)
		UTexture* Img_Input_Released_BottomFaceButton;
	UPROPERTY(EditAnywhere)
		UTexture* Img_Input_Released_LeftFaceButton;
	UPROPERTY(EditAnywhere)
		UTexture* Img_Input_Released_RightFaceButton;
	UPROPERTY(EditAnywhere)
		UTexture* Img_Input_Held_TopFaceButton;
	UPROPERTY(EditAnywhere)
		UTexture* Img_Input_Held_BottomFaceButton;
	UPROPERTY(EditAnywhere)
		UTexture* Img_Input_Held_LeftFaceButton;
	UPROPERTY(EditAnywhere)
		UTexture* Img_Input_Held_RightFaceButton;
	UPROPERTY(EditAnywhere)
		UTexture* Img_Input_UpDirection;
	UPROPERTY(EditAnywhere)
		UTexture* Img_Input_UpLeftDirection;
	UPROPERTY(EditAnywhere)
		UTexture* Img_Input_UpRightDirection;
	UPROPERTY(EditAnywhere)
		UTexture* Img_Input_DownDirection;
	UPROPERTY(EditAnywhere)
		UTexture* Img_Input_DownLeftDirection;
	UPROPERTY(EditAnywhere)
		UTexture* Img_Input_DownRightDirection;
	UPROPERTY(EditAnywhere)
		UTexture* Img_Input_LeftDirection;
	UPROPERTY(EditAnywhere)
		UTexture* Img_Input_RightDirection;
	
	
};
