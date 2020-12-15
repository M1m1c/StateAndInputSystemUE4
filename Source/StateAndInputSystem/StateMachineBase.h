// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "StateBase.h"
#include "InputClasses.h"
#include "StateMachineBase.generated.h"

class ASampleCharacter;



UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class STATEANDINPUTSYSTEM_API UStateMachineBase : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UStateMachineBase();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	

	//TODO move everything below this line to StateMachineBase----------------------------------------------------------------------------
//TODO make sure that StateMachineBase has a permanent referne to its owner character, that wya the functions below don't have to take in the character

// Loops through all state links in the state link list.
	virtual void CheckAllStateLinks(UStateBase* currentState, const TArray<FInputFrame> &InputStream);



	bool DoesLastElementOfInputstreamContainAcitveButtons(const TArray<FInputFrame> & InputStream);


private:

	bool CheckStateLinks(FString currentStateName, const TArray<FInputFrame> & InputStream, TArray<FStateLink> StateLinksToCheck);

	// Checks one state link and returns a state machine completion type if the input matches the conditions
	virtual bool CheckOneStateLink( const TArray<FInputFrame> &InputStream, FStateLink OneStateLink);

	int32 FindRequiredDirectionsInInputStream(FStateLink &OneStateLink, const TArray<FInputFrame> & InputStream, bool allowButtons);

	//Loops through InputStream comparing the inputs to TempRequierdDirections, when a match is found breaks and returns true.
	//"Start" is start point of loop and "End" is the end point unless "ReverseLoop" is true, in which case they are reversed
	bool FoundRequiredDirectionIndexInInputStream(
		TArray<FLinkConditonDirection> &TempRequierdDirections,
		const TArray<FInputFrame> & InputStream,
		bool ReverseLoop,
		int32 RequiredDirIndex,
		int32 Start,
		int32 End,
		int32 &TempReturnIndex,
		float PreviousTimeStamp,
		float TimeFail,
		int32 frameFail,
		bool allowButtons);

	bool IsButtonInFrameJustPressed(const FInputFrame & InputFrame);

	int32 SetFoundCorrectDirectional(int32 CorrectDirectionalInputs, int32 Index, TArray<FLinkConditonDirection> &TempRequierdDirections);

	int32 FindRequiredButtonsInInputStream(const TArray<FInputFrame> & InputStream, FStateLink &OneStateLink);

	//bool CheckSpecialLinkCondition(FStateLink &OneStateLink, ASampleCharacter * BBCharacter);

	// Sets QueuedState to DestinationState
	virtual void QueueState( UStateBase* DestiantionState, FStateLink OneStateLink);

	virtual int32 CheckHowManyBitFlagsSet(int32 FlagsToCheck, int32 EnumCount);

	//Used for passing the next state to run state
	UStateBase* StateToSwitchTo = nullptr;

	FStateLink DefaultLink;

	ASampleCharacter* MyCharacter;
	
};
