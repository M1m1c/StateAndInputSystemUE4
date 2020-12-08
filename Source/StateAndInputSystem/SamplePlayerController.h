// Fill out your copyright notice in the DisplayName page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "InputClasses.h"
#include "SamplePlayerController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInputStreamUIDelegate, ASamplePlayerController*, player);

class UInputButtonClass;
class ASampleCharacter;

UENUM(Blueprintable, Meta = (Bitflags))
enum class EAxisDirections : uint8 {
	XPossitive,
	XNegative,
	YPossitive,
	YNegative,

};

UCLASS(Blueprintable)
class  STATEANDINPUTSYSTEM_API ASamplePlayerController : public APlayerController
{
	GENERATED_BODY()


public:
	
	FInputStreamUIDelegate OnInputStreamUpdate;

	float DebuggTimer;
	float StickSensitivityThreshold = 0.45f;

	ASamplePlayerController();
	void InitaliseButtonAtoms();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void SetupInputComponent() override;
	virtual void ClearInputStream();
	virtual void ConsumeOneInput(int32 InputListPosition);


	virtual TArray<FInputFrame>& GetInputStreamReference();

	bool bActivated = false;


protected:



	void CalculateButtonInput(FString DebuggInpputStream);
	void AddInputToInputStream(FString & DebuggInpputStream);

	void RemoveOldInputAtoms();

	void CalculateDirectionalInput();

	void AxisDirectionToggle(float axis, int32 possitiveDirectionBit, int32 negativeDirectionBit);

	bool DoesButtonAtomsHaveActiveButtons();
	bool IsAButtonHeldInInputStream();
	bool IsACurrentButtonHeldInArray(TArray<FInputButtonStruct>& frameButtons);

	UInputButtonClass * FindSpecificButtonInArray(TArray<UInputButtonClass*>& collection, EInputButtons button);

	void ReadYAxis(float value);
	void ReadXAxis(float value);

	float YAxis = 0.0f;
	float XAxis = 0.0f;

	UInputButtonClass* PressButton(EInputButtons type, float time);
	UInputButtonClass* ReleaseButton(EInputButtons type, float time);

	void LeftFaceButtonPressed();
	void LeftFaceButtonReleased();

	void TopFaceButtonPressed();
	void TopFaceButtonReleased();

	void RightFaceButtonPressed();
	void RightFaceButtonReleased();

	void BottomFaceButtonPressed();
	void BottomFaceButtonReleased();

	void RightBumperPressed();
	void RightBumperReleased();

	void LeftBumperPressed();
	void LeftBumperReleased();

	UInputButtonClass* LeftFaceButton;
	UInputButtonClass* TopFaceButton;
	UInputButtonClass* RightFaceButton;
	UInputButtonClass* BottomFaceButton;
	UInputButtonClass* RightBumper;
	UInputButtonClass* LeftBumper;

	UPROPERTY(EditAnywhere)
		float InputExpirationTime = 0.5f;

	// the inputStream will never exceede this number
	UPROPERTY(EditAnywhere)
		int32 InputBufferSize = 10;

	UPROPERTY(VisibleAnywhere, Category = "Input Atoms")
		TArray<UInputButtonClass*> ButtonAtoms;

	UPROPERTY(EditAnywhere)
		float TimeTilButtonHeldDown = 0.3f;

	float CurrentTime;

private:

	//How many buttons are activated and which ones.
	UPROPERTY(VisibleInstanceOnly)
		FInputFrame CurrentInputFrame;

	//How many frames there are input for in the input buffer
	UPROPERTY(VisibleInstanceOnly)
		TArray<FInputFrame> InputStream;


	UInputButtonClass* GetInputButtonPressed(EInputButtons ButtonInput);

	ASampleCharacter* myCharacter;

	int32 DirectionFlags = 0;
	int32 PreviousDirection = 0;
	bool PassOneDirInput = false;
};

