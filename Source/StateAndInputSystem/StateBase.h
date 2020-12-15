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
	UPROPERTY(EditAnywhere, Meta = (Bitmask, BitmaskEnum = "EInputButtons"))
		int32 RequiredButton;

	//Set only one state, unless multiple states are acceptable, this state is the state that will be needed for thw button set in the RequierdButton variable.
	UPROPERTY(EditAnywhere, Meta = (Bitmask, BitmaskEnum = "EButtonState"))
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

	//Simple version: How many frames you have available to perform the input sequence.
	//Detailed version: The total amount of frames that the input sequence will allow, if inputs are outside of this amount then they will be ignored.
	UPROPERTY(EditAnywhere)
		int32 SequenceLengthFailThreshold;

	//Simple version: How many frames of incorrect input are allowed between correct inputs in the sequence.
	//Detailed version: How large of an input frame gap is acceptable, if an input is beyond this many frames in distance from the previously accpeted input then this link will fail.  
	UPROPERTY(EditAnywhere)
		int32 FrameGapFailThreshold;

	//Simple version: How many directional inputs can the player mess up but still succeed with.
	//Detailed version:How many directional inputs of the requiered inputs that the player can miss and still progress with the link, the first and last input in an input sequence always need to be correct regardless of miss tolerance.
	UPROPERTY(EditAnywhere)
		int32 MissTolerance;

	// if true will reverse the result of the special link conditon, so that this link only becomes available when the special link condition is false.
	/*UPROPERTY(EditAnywhere)
		bool ReverseSpecialLinkCondition = false;*/

	// Character specific SpecialLinkCondition, used for character specific mechanincs that gate certain moves.
	/*UPROPERTY(EditAnywhere)
		UBBSpecialLinkCondition* SpecialLinkCondition;
		*/
	//Set this to true if this link relies on anim notifes to switch state.
	//NOTE, requires animation to play and hit custom notify to switch state, allows for queueing of states while in animation.
	UPROPERTY(EditAnywhere)
		bool UseNotifyStateSwitching = false;


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
	UPROPERTY(EditAnywhere)
		class UStateParametersBase* ThisStatesParams;

	// This is a class that contains a list of StateLinks that should always be available in every move, such as jump, dodge and dash.
	// Make sure to set up the list for each characters specific SharedLinks.
	/*UPROPERTY(EditAnywhere)
		class UBBSharedStateLinks* SharedStateLinks;*/

	// These are the states that this state can link to. They are in priority order,
	//Place the hardest to perform at the top, so that they are more likley to happen.
	UPROPERTY(EditAnywhere)
		TArray<FStateLink> StateLinks;


	virtual void InitializeThisState(ASampleCharacter * Character);
	virtual void ResetTimeInThisState();
	virtual void UpdateTimeInThisState(float TimeInState);


private:
	float TimeInThisState = 0.0f;



};
