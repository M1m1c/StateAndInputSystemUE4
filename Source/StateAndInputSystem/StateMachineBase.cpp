// Fill out your copyright notice in the Description page of Project Settings.

#include "StateMachineBase.h"
#include "SampleCharacter.h"
#include "SamplePlayerController.h"


// Sets default values for this component's properties
UStateMachineBase::UStateMachineBase()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UStateMachineBase::BeginPlay()
{
	Super::BeginPlay();
	MyCharacter = Cast<ASampleCharacter>(this->GetOwner());

	if (!MyCharacter)
	{
		UE_LOG(LogTemp, Error, TEXT("%s @BeginPlay failed to cast owner to ASampleCharacter, destroying this component"), *GetName());
		this->DestroyComponent();
	}

	DefaultLink.NextState = MyCharacter->GetDefaultState();
	// ...

}

//----------------------------------CHECK STATE LINKS---------------------------------------------------------------------------
//This is called in PlayerPawns tick function (Called each frame), all it does is go through all possible moves from the current state to see if one is accepted.
//Takes in a reference to the playerpawn, A list of all the inputs performed within a timespan, , all the StateLinks this state has to other states, 
void UStateMachineBase::CheckAllStateLinks(UStateBase* currentState, const TArray<FInputFrame> &InputStream, float deltaTime)//, TArray<FStateLink> StateLinks)
{
	
	if (MyCharacter->QuedState != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s @CheckStateLinks Allready has queued state returning, no need to check links"), *GetName());
		return;
	}

	if (currentState->StateLinks.Num())
		// || (SharedStateLinks && SharedStateLinks->StateLinks.Num())
	{
		if (InputStream.Num())
		{
			if (DoesLastElementOfInputstreamContainAcitveButtons(InputStream))
			{
				StateToSwitchTo = nullptr;
				DeltaTime = deltaTime;
				// SET LINKS
				if (CheckStateLinks(currentState->StateName.ToString(), InputStream, currentState->StateLinks)) return;

				// SHARED LINKS					
				/*if (SharedStateLinks)
				{
					if (CheckStateLinks(Character, InputStream, SharedStateLinks->StateLinks)) return;
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("%s @CheckStateLinks No SharedLinks Set"), *GetName());
				}*/
			}
		}
	}
	else
	{
		//UE_LOG(LogTemp, Warning, TEXT("%s @CheckStateLinks had no StateLinks returning to default state"), *GetName());
		//QueueState( MyCharacter->GetDefaultState(), DefaultLink);
		return;
	}
}

bool UStateMachineBase::CheckStateLinks(FString currentStateName, const TArray<FInputFrame> & InputStream, TArray<FStateLink> StateLinksToCheck)
{
	auto retflag = false;
	if (StateLinksToCheck.Num())
	{
		for (int32 i = 0; i < StateLinksToCheck.Num(); i++)
		{
			if (!StateLinksToCheck.IsValidIndex(i))
			{
				UE_LOG(LogTemp, Warning, TEXT("%s @CheckStateLinks LINK %d =i was not a valid index in StateLinks"), *currentStateName, i);
				return retflag;
			}

			if (!StateLinksToCheck[i].NextState)
			{
				UE_LOG(LogTemp, Warning, TEXT("%s @CheckStateLinks LINK %d had no next State"), *currentStateName, i);
				return retflag;
			}

			if(CheckOneStateLink(InputStream, StateLinksToCheck[i]))
			{
				Cast<ASamplePlayerController>(MyCharacter->GetController())->ClearInputStream();
				UE_LOG(LogTemp, Warning, TEXT("@CheckStateLinks %s LINK Accepted"), *StateToSwitchTo->StateName.ToString());
				QueueState(StateToSwitchTo, StateLinksToCheck[i]);
				retflag = true;
				break;
			}
		}
	}
	return retflag;
}



//----------------------------------CHECK ONE STATE LINK---------------------------------------------------------------------------
// This is the bulk of the state machine, this is where the comparison betwwen input stream and move conditions are made
//Returns accepted if there is a match
//returns not accepted if no match
bool UStateMachineBase::CheckOneStateLink(const TArray<FInputFrame> &InputStream, FStateLink OneStateLink)
{

	bool ReturnCompletionType = false;
	int32 CorrectButtonInputs = 0;
	int32 CorrectDirectionInputs = 0;

	if (!OneStateLink.NextState)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s @CheckOneStatelink One State Link had no next state to go to"), *GetName());
		return false;
	}

	auto amountOfButtonBitflags = CheckHowManyBitFlagsSet(OneStateLink.RequiredButtons.RequiredButton, (int32)EInputButtons::Count);

	if (amountOfButtonBitflags > 0 && OneStateLink.RequieredDirections.Num())
	{
		CorrectButtonInputs = FindRequiredButtonsInInputStream(InputStream, OneStateLink);
		CorrectDirectionInputs = FindRequiredDirectionsInInputStream(OneStateLink, InputStream, true);

		if (CorrectButtonInputs == amountOfButtonBitflags &&
			(CorrectDirectionInputs + OneStateLink.MissTolerance) >= OneStateLink.RequieredDirections.Num())
		{
			StateToSwitchTo = OneStateLink.NextState;
			ReturnCompletionType = true;
		}
	}
	else if (amountOfButtonBitflags > 0 && !OneStateLink.RequieredDirections.Num())
	{
		CorrectButtonInputs = FindRequiredButtonsInInputStream(InputStream, OneStateLink);

		if (CorrectButtonInputs == amountOfButtonBitflags)
		{
			StateToSwitchTo = OneStateLink.NextState;
			ReturnCompletionType = true;
		}
	}
	else if (OneStateLink.RequieredDirections.Num() > 0 && amountOfButtonBitflags <= 0)
	{
		CorrectDirectionInputs = FindRequiredDirectionsInInputStream(OneStateLink, InputStream, false);

		if ((CorrectDirectionInputs + OneStateLink.MissTolerance) >= OneStateLink.RequieredDirections.Num())
		{
			StateToSwitchTo = OneStateLink.NextState;
			ReturnCompletionType = true;
		}
	}
	else
	{
		StateToSwitchTo = OneStateLink.NextState;
	}

	return ReturnCompletionType;
}

int32 UStateMachineBase::FindRequiredDirectionsInInputStream(FStateLink &OneStateLink, const TArray<FInputFrame> & InputStream, bool allowButtons)
{
	TArray<FLinkConditonDirection> TempRequierdDirections = OneStateLink.RequieredDirections;
	int32 dirCheckSteps = TempRequierdDirections.Num() + 1;
	int32 CorrectDirectionalInputs = 0;
	int32 FirstDirInputIndex = 0;
	int32 LastDirInputIndex = InputStream.Num();
	int32 TempReturnIndex = 0;
	//TODO bör nog enegentligen gångras med deltatime istället för et tkonstant värde
	float sequenceLengthFail = (float)OneStateLink.SequenceLengthFailThreshold * DeltaTime;// 0.016f;
	float LastTimeStamp = InputStream.Last().TimeStamp;
	int32 frameGapFail = OneStateLink.FrameGapFailThreshold;

	while (dirCheckSteps > 0)
	{
		if (CorrectDirectionalInputs == 0)
		{
			//Find the first correct direction that is within the SequenceLength threshold
			if (TempRequierdDirections.IsValidIndex(0))
			{
				if (FoundRequiredDirectionIndexInInputStream(
					TempRequierdDirections,
					InputStream,
					false,
					0,
					FirstDirInputIndex,
					InputStream.Num(),
					TempReturnIndex,
					LastTimeStamp,
					sequenceLengthFail,
					frameGapFail,
					allowButtons))
				{

					TempRequierdDirections[0].FoundThisDirInput = true;
					CorrectDirectionalInputs++;
					FirstDirInputIndex = TempReturnIndex;
				}
				else
				{
					FirstDirInputIndex++;
				}
			}
		}
		else if (CorrectDirectionalInputs < TempRequierdDirections.Num())
		{
			//Finds the correct directions that are between the last found index and the first found index
			for (int32 z = TempRequierdDirections.Num() - 1; -1 < z; z--)
			{
				if (TempRequierdDirections.IsValidIndex(z) &&
					InputStream.IsValidIndex(FirstDirInputIndex) &&
					(InputStream.IsValidIndex(LastDirInputIndex) || InputStream.Num() == LastDirInputIndex))
				{

					if (FoundRequiredDirectionIndexInInputStream(
						TempRequierdDirections,
						InputStream,
						true,
						z,
						FirstDirInputIndex,
						LastDirInputIndex,
						TempReturnIndex,
						LastTimeStamp,
						sequenceLengthFail,
						frameGapFail,
						allowButtons))
					{
						TempRequierdDirections[z].FoundThisDirInput = true;
						dirCheckSteps--;
						CorrectDirectionalInputs++;
						LastDirInputIndex = TempReturnIndex;
					}
				}
			}
		}
		dirCheckSteps--;
	}

	return CorrectDirectionalInputs;
}

//Loops through inputstream either forwards or reverse from loopStart to loopStop.
//Returns true if it manages to match the correct direction with one in the inputstream.
//Uses TempReturnIndex as an out parameter to determine which index the correct direction was found on,
//so that next cycle around that can be used to determine where to start the loop.
bool UStateMachineBase::FoundRequiredDirectionIndexInInputStream(
	TArray<FLinkConditonDirection> &TempRequierdDirections,
	const TArray<FInputFrame> & InputStream,
	bool ReverseLoop,
	int32 RequiredDirIndex,
	int32 Start,
	int32 End,
	int32 &TempReturnIndex,
	float LastTimeStamp,
	float sequenceLengthFail,
	int32 frameGapFail,
	bool allowButtons)
{

	bool RetFlag = false;
	int LoopStart = ((ReverseLoop) ? End : Start);
	int LoopStop = ((ReverseLoop) ? Start : End);
	int LoopStep = ((ReverseLoop) ? -1 : 1);
	int32 InputStreamFrameCounter = 0;

	for (int32 i = LoopStart; i != LoopStop; i += LoopStep)
	{
		if (InputStreamFrameCounter > frameGapFail)
		{
			break;
		}
		InputStreamFrameCounter++;

		if (!InputStream.IsValidIndex(i))
		{
			continue;
		}

		bool isThisInputAlreadyFound = TempRequierdDirections[RequiredDirIndex].FoundThisDirInput == true;
		if (isThisInputAlreadyFound)
		{
			continue;
		}

		bool isRequiredDirectionNotInInputFrame = !(TempRequierdDirections[RequiredDirIndex].RequieredDirection & 1 << (int32)InputStream[i].DirectionalInput);
		if (isRequiredDirectionNotInInputFrame)
		{
			continue;
		}

		if (!(LastTimeStamp >= InputStream[i].TimeStamp))
		{
			continue;
		}

		bool isInputFrameNotInSequenceLength = !(LastTimeStamp - InputStream[i].TimeStamp <= sequenceLengthFail);
		if (isInputFrameNotInSequenceLength)
		{
			continue;
		}

		if (allowButtons && IsButtonInFrameJustPressed(InputStream[i]))
		{
			RetFlag = true;
			TempReturnIndex = i;
			break;
		}
		else if (InputStream[i].ContainedButtons.Num() == 0)
		{
			RetFlag = true;
			TempReturnIndex = i;
			break;
		}	
	}
	return RetFlag;
}

bool UStateMachineBase::IsButtonInFrameJustPressed(const FInputFrame & InputFrame)
{
	bool retflag = false;
	for (int32 i = 0; i < InputFrame.ContainedButtons.Num(); i++)
	{
		auto buttonFrame = InputFrame.ContainedButtons;
		for (int32 g = 0; g < buttonFrame.Num(); g++)
		{
			if (buttonFrame[g].ButtonState == EButtonState::JustPressed)
			{
				retflag = true;
				break;
			}
		}
	}
	return retflag;
}

int32 UStateMachineBase::SetFoundCorrectDirectional(int32 CorrectDirectionalInputs, int32 Index, TArray<FLinkConditonDirection> &TempRequierdDirections)
{
	TempRequierdDirections[Index].FoundThisDirInput = true;
	return CorrectDirectionalInputs += 1;
}

int32 UStateMachineBase::FindRequiredButtonsInInputStream(const TArray<FInputFrame> & InputStream, FStateLink &OneStateLink)
{
	int correctButtons = 0;
	auto buttons = InputStream.Last().ContainedButtons;
	for (int32 z = 0; z < buttons.Num(); z++)
	{
		if (buttons.IsValidIndex(z))
		{
			auto requiredButtons = OneStateLink.RequiredButtons;
			auto amountOfBitFlags = CheckHowManyBitFlagsSet(requiredButtons.RequiredButton, (int32)EInputButtons::Count);
			for (int32 i = 0; i < amountOfBitFlags; i++)
			{
				if (requiredButtons.RequiredButton & 1 << (int32)buttons[z].ButtonInput)
				{
					if (requiredButtons.RequiredButtonState & 1 << (int32)buttons[z].ButtonState)
					{
						correctButtons++;
						break;
					}

				}
			}
		}
	}
	return correctButtons;
}

bool UStateMachineBase::DoesLastElementOfInputstreamContainAcitveButtons(const TArray<FInputFrame> & InputStream)
{
	auto lastButtons = InputStream.Last().ContainedButtons;
	for (int32 i = 0; i < lastButtons.Num(); i++)
	{
		if (lastButtons.IsValidIndex(i))
		{
			if (lastButtons[i].ButtonState != EButtonState::Up &&
				lastButtons[i].ButtonState != EButtonState::Up &&
				lastButtons[i].ButtonState != EButtonState::Count)
			{
				return true;
			}
		}

	}
	return false;
}

//----------------------------------QUEUE STATE---------------------------------------------------------------------------
//TODO remove one statelink form parameter, itis not used here
void UStateMachineBase::QueueState(UStateBase * DestiantionState, FStateLink OneStateLink)
{
	//When We Switch state we reset a timer based on animation
	//We also return out of this function
	//CurrentState = StateToSwitchTo

	if (DestiantionState != nullptr)
	{
		if (OneStateLink.UseNotifyStateSwitching == false)
		{
			MyCharacter->SetCurrentState(DestiantionState);
		}
		else
		{
			MyCharacter->QuedState = DestiantionState;
		}


	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("%s @QueueState DestinationState was Null"), *GetName());
		return;
	}
}

int32 UStateMachineBase::CheckHowManyBitFlagsSet(int32 FlagsToCheck, int32 EnumCount)
{
	int32 ReturnValue = 0;
	for (int32 i = 0; i < EnumCount; i++)
	{
		if (FlagsToCheck & 1 << i)
		{
			ReturnValue++;
		}
	}
	return ReturnValue;
}

