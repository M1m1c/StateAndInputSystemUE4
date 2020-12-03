// Fill out your copyright notice in the DisplayName page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "InputClasses.generated.h"

/**
 * 
 */
UCLASS()
class STATEANDINPUTSYSTEM_API UInputClasses : public UDataAsset
{
	GENERATED_BODY()
	
	
	
	
};

#pragma region ENUMS
UENUM(Blueprintable, Meta = (Bitflags))
enum class EButtonState : uint8
{
	// Button is in the up state, the neutral state that does not count as an input
	Up,
	// The button has just been pressed, counts as an input.
	JustPressed,
	// The button is being held down, counts as an input.
	HeldDown,
	// The button has been released, Counts as an input.
	Released,
	// Used for counting the differnet alternatives in this enum and as an inbetween JustPresed and held down button states.
	Count UMETA(Hidden)
};

UENUM(Blueprintable, Meta = (Bitflags))
enum class EInputButtons : uint8
{
	LeftFace,
	TopFace,
	RightFace,
	BottomFace,
	RightBumper,
	LeftBumper,
	Count UMETA(Hidden)
};

UENUM(Meta = (Bitflags))
enum class EInputDirections : uint32 {
	None,
	Up,
	Down,
	Right,
	Left,
	UpRight,
	UpLeft,
	DownRight,
	DownLeft,
	Count

};
#pragma endregion ENUMS


UCLASS()
class STATEANDINPUTSYSTEM_API UInputButtonClass : public UDataAsset
{
	GENERATED_BODY()

public:
	// Display name for this input atom, mainly for debugging purposes.
	UPROPERTY(EditAnywhere)
	FName DisplayName;

	// The Button this input refers to
	UPROPERTY(EditAnywhere)
	EInputButtons ButtonInput;

	// State of button being described
	UPROPERTY(EditAnywhere)
	EButtonState ButtonState = EButtonState::Up;

	// the state that this button was in in the previous frame
	EButtonState PreviousButtonState;

	// The time at which this input was created.
	float InputTimeStamp;

	// Used to determine if a button should transition into helddown state, initiated uppon button press.
	float BeginHoldingTimeStamp;

	// Used to gate a button so that it has to be released in order to count ass pushed again, so one press will count as one press.
	bool bButtonPushedGate = true;

	// Used to determine if a button is being held down by the player
	bool bButtonHolding = false;

};

#pragma region STRUCTS
USTRUCT()
struct STATEANDINPUTSYSTEM_API FInputButtonStruct
{
	GENERATED_BODY()
		FInputButtonStruct() {};
	FInputButtonStruct(UInputButtonClass* inputButton)
	{
		SetValuesFromClass(inputButton);
	};
public:
	FName DisplayName;

	UPROPERTY(VisibleInstanceOnly)
	EInputButtons ButtonInput;

	UPROPERTY(VisibleInstanceOnly)
	EButtonState ButtonState = EButtonState::Up;

	EButtonState PreviousButtonState;

	UPROPERTY(VisibleInstanceOnly)
	float InputTimeStamp;

	float BeginHoldingTimeStamp;

	bool bButtonPushedGate = true;
	bool bButtonHolding = false;

	void SetValuesFromClass(UInputButtonClass* inputButton);
	void ResetToDefault(EInputButtons button);
};

USTRUCT()
struct STATEANDINPUTSYSTEM_API FInputFrame
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleInstanceOnly)
		TArray<FInputButtonStruct> ContainedButtons;

	UPROPERTY(VisibleInstanceOnly)
		EInputDirections DirectionalInput;

	UPROPERTY(VisibleInstanceOnly)
		float TimeStamp;
};
#pragma endregion STRUCTS