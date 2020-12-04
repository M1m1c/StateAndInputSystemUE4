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
void UStateMachineBase::CheckAllStateLinks(UStateBase* currentState, const TArray<FInputFrame> &InputStream)//, TArray<FStateLink> StateLinks)
{
	//TODO (I think this is done) Make it so that we only startchecking for input if we are in a frame set window,- 
	//TODO (However this is not really) -otherwise allow the inputs to build up in the input stream. Currently every input gets killed instantly.
	

	if (MyCharacter->QuedState != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s @CheckStateLinks Allready has queued state returning, no need to check links"), *GetName());
		return;
	}

	if (currentState->StateLinks.Num())
		//|| InputlessStateLinks.Num() ||
		//(SharedStateLinks && SharedStateLinks->StateLinks.Num()) ||
		//(InputlessSharedStateLinks && InputlessSharedStateLinks->StateLinks.Num()))
	{
		//TODO remove these frame set windows we don't use them, instead we use notifies
		/*if ((FrameSetWindow.X == 0.0f && FrameSetWindow.Y == 0.0f) || (TimeInThisState > FrameSetWindow.X && TimeInThisState < FrameSetWindow.Y))
		{
		}*/

		//TODO Go through the entire input stream per state link until we find somehting that matches
		//TODO setup so that links have FrameFailThreshold, InputFailThreshold and TimeFailThreshold
		//TODO if one of the fail thresholds is met or we run the entire input stream, then return not accepted and go to the next statelink
		if (InputStream.Num())
		{
			if (DoesLastElementOfInputstreamContainAcitveButtons(InputStream))
			{
				//int32 NotAcceptedLinkTries = 0;
				StateToSwitchTo = nullptr;

				// SET LINKS
				if (CheckStateLinks(currentState->StateName.ToString(), InputStream, currentState->StateLinks)) return;

				//Maybe we should go through shared links before set links, so that we can prioratise blocking, bursting and dashing.
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

		//--------------------------------INPUTLESS STATE LINKS--------------------------------------------
		/*if (CheckStateLinks(Character, InputStream, InputlessStateLinks)) return;

		//--------------------------------INPUTLESS SHARED STATE LINKS--------------------------------------------
		if (InputlessSharedStateLinks)
		{
			if (CheckStateLinks(Character, InputStream, InputlessSharedStateLinks->StateLinks)) return;
		}
		*/
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("%s @CheckStateLinks had no StateLinks returning to default state"), *GetName());

		QueueState( MyCharacter->GetDefaultState(), DefaultLink);
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


			switch (CheckOneStateLink(InputStream, StateLinksToCheck[i]))
			{
			case EStateMachineCompletionType::Accepted:

				//TODO if it is the default state that we are switching to then probably don't clear the input stream
				//TODO clear insputstream widget in UI of character
				Cast<ASamplePlayerController>(MyCharacter->GetController())->ClearInputStream();
				UE_LOG(LogTemp, Warning, TEXT("@CheckStateLinks %s LINK Accepted"), *StateToSwitchTo->StateName.ToString());
				QueueState( StateToSwitchTo, StateLinksToCheck[i]);
				retflag = true;
				return retflag;
				break;
			case EStateMachineCompletionType::NotAccepted:
				// Decrese remaining steps
				//NotAcceptedLinkTries++;
				// or do nothing
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
EStateMachineCompletionType UStateMachineBase::CheckOneStateLink( const TArray<FInputFrame> &InputStream, FStateLink OneStateLink)
{

	EStateMachineCompletionType ReturnCompletionType = EStateMachineCompletionType::NotAccepted;
	int32 CorrectButtonInputs = 0;
	int32 CorrectDirectionInputs = 0;

	if (!OneStateLink.NextState)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s @CheckOneStatelink One State Link had no next state to go to"), *GetName());
		return EStateMachineCompletionType::NotAccepted;
	}

	/*if (!OneStateLink.NextState->ThisStatesMove)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s @CheckOneStatelink One State Link had no move to use"), *GetName());
		return EStateMachineCompletionType::NotAccepted;
	}*/

	/*if (!CheckSpecialLinkCondition(OneStateLink, Character))
	{
		//Special Link Condition requirments failed
		return EStateMachineCompletionType::NotAccepted;
	}*/

	auto amountOfButtonBitflags = CheckHowManyBitFlagsSet(OneStateLink.RequiredButtons.RequiredButton, (int32)EInputButtons::Count);

	if (amountOfButtonBitflags > 0 && OneStateLink.RequieredDirections.Num())
	{
		CorrectButtonInputs = FindRequiredButtonsInInputStream(InputStream, OneStateLink);
		CorrectDirectionInputs = FindRequiredDirectionsInInputStream(OneStateLink, InputStream, true);

		if (CorrectButtonInputs == amountOfButtonBitflags &&
			(CorrectDirectionInputs + OneStateLink.MissTolerance) >= OneStateLink.RequieredDirections.Num())
		{
			StateToSwitchTo = OneStateLink.NextState;
			ReturnCompletionType = EStateMachineCompletionType::Accepted;
		}
		else
		{
			ReturnCompletionType = EStateMachineCompletionType::NotAccepted;
		}
	}
	else if (amountOfButtonBitflags > 0 && !OneStateLink.RequieredDirections.Num())
	{
		CorrectButtonInputs = FindRequiredButtonsInInputStream(InputStream, OneStateLink);

		if (CorrectButtonInputs == amountOfButtonBitflags)
		{
			StateToSwitchTo = OneStateLink.NextState;
			ReturnCompletionType = EStateMachineCompletionType::Accepted;
		}
		else
		{
			ReturnCompletionType = EStateMachineCompletionType::NotAccepted;
		}
	}
	else if (OneStateLink.RequieredDirections.Num() > 0 && amountOfButtonBitflags <= 0)
	{
		CorrectDirectionInputs = FindRequiredDirectionsInInputStream(OneStateLink, InputStream, false);
		//TODO only for this case, do not accept it if the input frame with the direciton also contains a button.
		if ((CorrectDirectionInputs + OneStateLink.MissTolerance) >= OneStateLink.RequieredDirections.Num())
		{
			StateToSwitchTo = OneStateLink.NextState;
			ReturnCompletionType = EStateMachineCompletionType::Accepted;
		}
		else
		{
			ReturnCompletionType = EStateMachineCompletionType::NotAccepted;
		}
	}
	else
	{
		StateToSwitchTo = OneStateLink.NextState;
		ReturnCompletionType = EStateMachineCompletionType::Accepted;
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
	float TimeFail = (float)OneStateLink.TimeFailThreshold * 0.016f;
	float PreviousTimeStamp = InputStream.Last().TimeStamp;
	int32 frameFail = OneStateLink.FrameFailThreshold;

	while (dirCheckSteps > 0)
	{
		if (CorrectDirectionalInputs == 0)
		{

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
					PreviousTimeStamp,
					TimeFail,
					frameFail,
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
						PreviousTimeStamp,
						TimeFail,
						frameFail,
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


bool UStateMachineBase::FoundRequiredDirectionIndexInInputStream(
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
	bool allowButtons)
{

	bool RetFlag = false;
	int LoopStart = ((ReverseLoop) ? End : Start);
	int LoopStop = ((ReverseLoop) ? Start : End);
	int LoopStep = ((ReverseLoop) ? -1 : 1);
	int32 InputStreamFrameCounter = 0;

	for (int32 i = LoopStart; i != LoopStop; i += LoopStep)
	{
		if (InputStreamFrameCounter > frameFail)
		{
			break;
		}

		if (InputStream.IsValidIndex(i))
		{
			if (TempRequierdDirections[RequiredDirIndex].FoundThisDirInput == false)
			{
				if (TempRequierdDirections[RequiredDirIndex].RequieredDirection & 1 << (int32)InputStream[i].DirectionalInput)
				{
					if (PreviousTimeStamp >= InputStream[i].TimeStamp)
					{
						if (PreviousTimeStamp - InputStream[i].TimeStamp <= TimeFail)
						{
							if (allowButtons &&IsButtonInFrameJustPressed(InputStream[i]))
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
					}
				}
			}
		}
		InputStreamFrameCounter++;
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
/*
bool UStateMachineBase::CheckSpecialLinkCondition(FStateLink &OneStateLink, ASampleCharacter * Character)
{
	bool SLC_Status = false;
	if (!OneStateLink.SpecialLinkCondition)
	{
		SLC_Status = true;
	}
	else
	{
		if (OneStateLink.ReverseSpecialLinkCondition == false)
		{
			if (OneStateLink.SpecialLinkCondition->IsSpecialConditionTrue(Character))
			{
				SLC_Status = true;
			}
			else
			{
				SLC_Status = false;
			}
		}
		else
		{
			if (OneStateLink.SpecialLinkCondition->IsSpecialConditionTrue(BBCharacter))
			{
				SLC_Status = false;
			}
			else
			{
				SLC_Status = true;
			}
		}
	}

	return SLC_Status;
}
*/
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
void UStateMachineBase::QueueState(UStateBase * DestiantionState, FStateLink OneStateLink)
{
	//When We Switch state we reset a timer based on animation
	//We also return out of this function
	//CurrentState =StateToSwitchTo

	if (DestiantionState != nullptr)
	{
			//Cast<ABladeBreakerCharacter>(BBCharacter)->SetCurrentState(DestiantionState);

			/*if (BBCharacter->GetCurrentState() == BBCharacter->GetDefaultState())
			{
				BBCharacter->SetCurrentState(DestiantionState);
			}
			else*/
			//if ((ThisStatesMove) && OneStateLink.DontUseNotifyStateSwitching == true)
			if (OneStateLink.DontUseNotifyStateSwitching == true)
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

