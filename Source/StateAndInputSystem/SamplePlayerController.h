// © 2018, Ludvig Baummann Olsson. All rights reserved.

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
	//Clears the input stream when called
	virtual void ClearInputStream();
	//Removes one input in the input stream
	virtual void ConsumeOneInput(int32 InputListPosition);


	virtual TArray<FInputFrame>& GetInputStreamReference();

	bool bActivated = false;


protected:


	//Measures the state of buttons and adds them to the current Input Frame
	void CalculateButtonInput();

	//Adds a reference to the current inputframe to the inputstream array
	void AddInputToInputStream();

	//Removes inputs in the input stream that have lived to the expiration time.
	void RemoveOldInputAtoms();

	//Called twice in tick, for X- and Y-Axis respectively.
	//Toggles EAxisDirections DirectionFlags based on if axis is posiive, negative or neutral.
	void AxisDirectionToggle(float axis, int32 possitiveDirectionBit, int32 negativeDirectionBit);

	//Measures the state of directions based of the toggled bitflags in DirectionalFlags Variable
	void CalculateDirectionalInput();

	bool DoesButtonAtomsHaveActiveButtons();
	bool IsAButtonHeldInInputStream();

	//Returns true if a held button in frameButtons array is the same in ButtonAtoms (Current active butons)  
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

	// How long an input frame lives for in seconds.
	UPROPERTY(EditAnywhere)
		float InputExpirationTime = 0.5f;

	// The inputStream will never exceede this number.
	UPROPERTY(EditAnywhere)
		int32 InputBufferSize = 10;

	//an array used for tracking the buttons current states
	UPROPERTY(VisibleAnywhere, Category = "Input Atoms")
		TArray<UInputButtonClass*> ButtonAtoms;

	// How long a player needs to hold down a button until it counts as being held.
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

