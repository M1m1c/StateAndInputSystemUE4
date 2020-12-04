// Fill out your copyright notice in the DisplayName page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "InputClasses.h"
#include "StateBase.generated.h"


struct FInputFrame;
class UBBSpecialLinkCondition;
class ASampleCharacter;



USTRUCT()
struct STATEANDINPUTSYSTEM_API FLinkConditonButton
{
	GENERATED_BODY()
		FLinkConditonButton() {}
public:

	//TODO Create variable to check if the index value is sequenced, 
	//that is to say they are not supposed to be on the same frame so that button sequences can be required for a link

	//Set the button that needs to be inputed , if buttons are supposed to be multiple input at the same time then add more entries in the RequierdButtons List.
	UPROPERTY(EditAnywhere, Meta = (Bitmask, BitmaskEnum = "EBBInputButtons"))
		int32 RequiredButton;

	//Set only one state, unless multiple states are acceptable, this state is the state that will be needed for thw button set in the RequierdButton variable.
	UPROPERTY(EditAnywhere, Meta = (Bitmask, BitmaskEnum = "EBBButtonState"))
		int32 RequiredButtonState;
};

USTRUCT()
struct STATEANDINPUTSYSTEM_API FLinkConditonDirection
{
	GENERATED_BODY()
		FLinkConditonDirection() {}
public:



	//Set the direction that needs to be inputed , also the order here top to bottom determines the order that the input needs to be in.
	UPROPERTY(EditAnywhere, Meta = (Bitmask, BitmaskEnum = "EInputDirections"))
		int32 RequieredDirection;

	bool FoundThisDirInput = false;

};


USTRUCT()
struct STATEANDINPUTSYSTEM_API FStateLink
{
	GENERATED_BODY()
		FStateLink()
	{

	}
public:
	// This is the state that this link will lead to
	UPROPERTY(EditAnywhere)
		class UStateBase* NextState;


	//protected:

		// these are the inputs in sequence requierd to progress with this link, if all inputs are correct and found within the same frame set in the same order as the list it can be accepted, if one is correct it cannot progress.
	UPROPERTY(EditAnywhere)
		FLinkConditonButton RequiredButtons;

	//The directions (in order) that are requiered to progress with this link, if
	UPROPERTY(EditAnywhere)
		TArray<FLinkConditonDirection> RequieredDirections;

	// How large of an input frame gap is acceptable, if an input is beyond this many frames in distance from the previously accpeted input then this link will fail.  
	UPROPERTY(EditAnywhere)
		int32 FrameFailThreshold;

	// How Many frames of time is acceptable between inputs, if an input is this much older than the previous input then this link will fail.
	UPROPERTY(EditAnywhere)
		int32 TimeFailThreshold;

	//How many inputs in the requiered inputs that the player can miss and still progress with the link, the first and last input in a input span always need to be correct regardless of miss tolerance.
	UPROPERTY(EditAnywhere)
		int32 MissTolerance;

	// if true will reverse the result of the special link conditon, so that this link only becomes available when the special link condition is false.
	UPROPERTY(EditAnywhere)
		bool ReverseSpecialLinkCondition = false;

	// Character specific SpecialLinkCondition, used for character specific mechanincs that gate certain moves.
	/*UPROPERTY(EditAnywhere)
		UBBSpecialLinkCondition* SpecialLinkCondition;
		*/
	//Set this to true if this link does not rely on notifes for its state switch timing and should instead just switch as soon as this link becomes valid.
	UPROPERTY(EditAnywhere)
		bool DontUseNotifyStateSwitching = false;


};

UCLASS()
class  STATEANDINPUTSYSTEM_API UStateBase : public UDataAsset
{
	GENERATED_BODY()

public:
	// This is the name of this state
	UPROPERTY(EditAnywhere)
		FName StateName;

	//TODO Change variable type to the correct variable once it is created.
	//This is the move that is performed when this state is entered.
	/*UPROPERTY(EditAnywhere)
		class UBBMoveBase* ThisStatesMove;*/

	//True will start counting the time once we entered this state. False will not count the time since we entered.
	//Used if the state has a determined FrameSetWindow.
	/*UPROPERTY(EditAnywhere)
		bool ShouldCountTimeInMove;*/

	//// make only two enteries, X is start of the window and Y is the end of the window.
	//// if the frame count is within this frame set then inputs in the input stream will be checked for acceptable inputs, if the frame count is outside the frame set window then this move can no longer link transtition.
	//// Leave at 0,0 if there is no window needed.
	//UPROPERTY(EditAnywhere)
	//	FVector2D FrameSetWindow;

	//// make only two enteries, X is start of the window and Y is the end of the window.
	//// this is where the move will transtition into the next linked move if a link was accepted
	//UPROPERTY(EditAnywhere)
	//	FVector2D MoveTransitionWindow;

	//// In this window the move can be canceled, aborting the the animation and returning to a controleld state (most likley cancelable through dash or block input, wich would also leave you in one of those states).
	//UPROPERTY(EditAnywhere)
	//	FVector2D CancelWindow;

	// This is a class that contains a list of StateLinks that should always be available in every move, such as jump, dodge and dash.
	// Make sure to set up the list for each characters specific SharedLinks.
	/*UPROPERTY(EditAnywhere)
		class UBBSharedStateLinks* SharedStateLinks;*/

	// These are the states that this state can link to. They are in priority order,
	//Place the hardest to perform at the top, so that they are more likley to happen.
	UPROPERTY(EditAnywhere)
		TArray<FStateLink> StateLinks;


	// These are the inputless state links of this state, they get checked even if there is no input being generated by the player. Get checked after statelinks and shared state links.
	//They are in priority order, place the hardest to perform at the top, so that they are more likley to happen.
	//WARNING!!! Most of these links should probably have SpecialLinkConditions, otherwise they will just pass through on the first loop, ignoring any other potential links.
	//UPROPERTY(EditAnywhere)
	//	TArray<FStateLink> InputlessStateLinks;


	// This is a class that contains a list of inputless StateLinks that should always be available in every move, grounded idle, air idle or knocked down.
	// Make sure to set up the list for each characters specific inputless SharedLinks.
	/*UPROPERTY(EditAnywhere)
		class UBBSharedStateLinks* InputlessSharedStateLinks;*/

	//TODO I think we need a bool or something that can be checked in case the current state is loopable,
	// so that moves like block will continue to be performed untill the player lets go of the block button.
	// this might also mean that we need a function/ some logic to make it so that a state can repeat or restart.

	virtual void InitializeThisState(ASampleCharacter * Character);
	virtual void ResetTimeInThisState();
	virtual void UpdateTimeInThisState(float TimeInState);


private:
	float TimeInThisState = 0.0f;



};
