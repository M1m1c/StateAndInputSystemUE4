// © 2018, Ludvig Baummann Olsson. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "StateBase.h"
#include "InputClasses.h"
#include "StateMachineBase.generated.h"

class ASampleCharacter;
class ASamplePlayerController;



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

	//This is called in PlayerPawns tick function (Called each frame), all it does is go through all possible moves from the current state to see if one is accepted.
	//Takes in a reference to the playerpawn, A list of all the inputs performed within a timespan and all the StateLinks this state has to other states. 
	virtual void CheckAllStateLinks(UStateBase* currentState, const TArray<FInputFrame> &InputStream,float deltaTime);

	bool DoesLastElementOfInputstreamContainAcitveButtons(const TArray<FInputFrame> & InputStream);

private:

	bool CheckStateLinks(FString currentStateName, const TArray<FInputFrame> & InputStream, TArray<FStateLink> StateLinksToCheck);

	// This is the bulk of the state machine, this is where the comparison betwwen input stream and move conditions are made
	//Returns true if there is a match
	//returns false if no match
	virtual bool CheckOneStateLink( const TArray<FInputFrame> &InputStream, FStateLink OneStateLink);


	//Takes the last inputframe's buttons and compares them to the buttons set as requirements in OneStateLink.
	//Returns amount of correct buttons that were found. 
	int32 FindRequiredButtonsInInputStream(const TArray<FInputFrame> & InputStream, FStateLink &OneStateLink);

	virtual int32 CheckHowManyBitFlagsSet(int32 FlagsToCheck, int32 EnumCount);

	//Loops forwards and backwards to find valid and correct directionals in inputstream as fast as it is able.
	//Finds the first required directional in inputstream by looping forwards using CouldFindValidDirectionAndIndex().
	//Then loops backwards through both the required directionals and the inputstream until it finds matches for all required,
	// or it runs out of dirChecksteps.
	//If the InputStream is more than half full it will only loop backwards.
	int32 FindRequiredDirectionsInInputStream(FStateLink &OneStateLink, const TArray<FInputFrame> & InputStream, bool allowButtons);

	//Loops through inputstream either forwards or reverse from loopStart to loopStop.
	//Returns true if it manages to match the correct direction with a valid input in the inputstream.
	//Uses TempReturnIndex as an out parameter to determine which index the correct direction was found on,
	//so that next cycle around that can be used to determine where to start the loop.
	bool CouldFindValidDirectionAndIndex(
		FLinkConditonDirection &TempRequierdDirection,
		const TArray<FInputFrame> & InputStream,
		bool ReverseLoop,
		int32 Start,
		int32 End,
		int32 &TempReturnIndex,
		float PreviousTimeStamp,
		float TimeFail,
		int32 frameFail,
		bool allowButtons);


	// Sets QueuedState to DestinationState
	virtual void QueueState( UStateBase* DestiantionState, FStateLink OneStateLink);

	//Used for passing the next state to run state
	UStateBase* StateToSwitchTo = nullptr;

	FStateLink DefaultLink;

	ASampleCharacter* MyCharacter;

	ASamplePlayerController* MySamplePlayerController;

	float DeltaTime;

	int32 HalfInputBufferSize =1;
	
};
