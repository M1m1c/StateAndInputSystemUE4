// © 2018, Ludvig Baummann Olsson. All rights reserved.

#include "StateMachineBase.h"
#include "SampleCharacter.h"
#include "SamplePlayerController.h"
#include "SharedStateLinks.h"


// Sets default values for this component's properties
UStateMachineBase::UStateMachineBase()
{
	PrimaryComponentTick.bCanEverTick = false;
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

	MySamplePlayerController = Cast<ASamplePlayerController>(MyCharacter->GetController());

	if (!MySamplePlayerController)
	{
		UE_LOG(LogTemp, Error, TEXT("%s @BeginPlay failed to cast owner to ASamplePlayerController, destroying this component"), *GetName());
		this->DestroyComponent();
	}

	HalfInputBufferSize = (MySamplePlayerController->GetInputBufferSize()/2);
	DefaultLink.NextState = MyCharacter->GetDefaultState();
}

void UStateMachineBase::CheckAllStateLinks(UStateBase* currentState, const TArray<FInputFrame> &InputStream, float deltaTime)
{

	if (MyCharacter->QuedState != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s @CheckStateLinks Allready has queued state returning, no need to check links"), *GetName());
		return;
	}

	if (currentState->StateLinks.Num())
	{
		if (InputStream.Num())
		{
			if (DoesLastElementOfInputstreamContainAcitveButtons(InputStream))
			{
				DeltaTime = deltaTime;
				// SET LINKS
				if (CheckStateLinks(currentState->StateName.ToString(), InputStream, currentState->StateLinks)) return;

				//SHARED LINKS
				auto shared = currentState->SharedStateLinks;
				if (shared)
				{
					if (CheckStateLinks(currentState->StateName.ToString(), InputStream, shared->StateLinks)) return;
				}
			}

		}
	}
	else
	{
		return;
	}
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
			auto stateToSwitchTo = CheckOneStateLink(InputStream, StateLinksToCheck[i]);
			if (stateToSwitchTo)
			{
				Cast<ASamplePlayerController>(MyCharacter->GetController())->ClearInputStream();
				UE_LOG(LogTemp, Warning, TEXT("@CheckStateLinks %s LINK Accepted"), *stateToSwitchTo->StateName.ToString());
				QueueState(stateToSwitchTo, StateLinksToCheck[i]);
				retflag = true;
				break;
			}
		}
	}
	return retflag;
}

UStateBase* UStateMachineBase::CheckOneStateLink(const TArray<FInputFrame> &InputStream, FStateLink OneStateLink)
{

	UStateBase* stateToSwitchTo = nullptr;
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
		//Buttons and Directions
		CorrectButtonInputs = FindRequiredButtonsInInputStream(InputStream, OneStateLink);
		CorrectDirectionInputs = FindRequiredDirectionsInInputStream(OneStateLink, InputStream, true);

		if (CorrectButtonInputs == amountOfButtonBitflags &&
			(CorrectDirectionInputs + OneStateLink.MissTolerance) >= OneStateLink.RequieredDirections.Num())
		{
			stateToSwitchTo = OneStateLink.NextState;
		}
	}
	else if (amountOfButtonBitflags > 0 && !OneStateLink.RequieredDirections.Num())
	{
		//Buttons Only
		CorrectButtonInputs = FindRequiredButtonsInInputStream(InputStream, OneStateLink);

		if (CorrectButtonInputs == amountOfButtonBitflags)
		{
			stateToSwitchTo = OneStateLink.NextState;
		}
	}
	else if (OneStateLink.RequieredDirections.Num() > 0 && amountOfButtonBitflags <= 0)
	{
		//Directions Only
		CorrectDirectionInputs = FindRequiredDirectionsInInputStream(OneStateLink, InputStream, false);

		if ((CorrectDirectionInputs + OneStateLink.MissTolerance) >= OneStateLink.RequieredDirections.Num())
		{
			stateToSwitchTo = OneStateLink.NextState;
		}
	}
	else
	{
		stateToSwitchTo = OneStateLink.NextState;
	}

	return stateToSwitchTo;
}

int32 UStateMachineBase::FindRequiredButtonsInInputStream(const TArray<FInputFrame> & InputStream, FStateLink &OneStateLink)
{
	int correctButtons = 0;
	auto frameButtons = InputStream.Last().ContainedButtons;
	for (int32 z = 0; z < frameButtons.Num(); z++)
	{
		if (frameButtons.IsValidIndex(z))
		{
			auto requiredButtons = OneStateLink.RequiredButtons;
			auto amountOfBitFlags = CheckHowManyBitFlagsSet(requiredButtons.RequiredButton, (int32)EInputButtons::Count);
			for (int32 i = 0; i < amountOfBitFlags; i++)
			{
				if (requiredButtons.RequiredButton & 1 << (int32)frameButtons[z].ButtonInput)
				{
					if (requiredButtons.RequiredButtonState & 1 << (int32)frameButtons[z].ButtonState)
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

int32 UStateMachineBase::FindRequiredDirectionsInInputStream(FStateLink &OneStateLink, const TArray<FInputFrame> & InputStream, bool allowButtons)
{
	TArray<FLinkConditonDirection> TempRequierdDirections = OneStateLink.RequieredDirections;
	int32 dirCheckSteps = TempRequierdDirections.Num() + 1;
	int32 CorrectDirectionalInputs = 0;
	int32 FirstDirInputIndex = 0;
	int32 LastDirInputIndex = InputStream.Num() - 1;
	int32 TempReturnIndex = 0;
	float LastIndexTimeStamp = InputStream.Last().TimeStamp;
	float previousTimeStamp = 0.0f;

	while (dirCheckSteps > 0)
	{

		if (CorrectDirectionalInputs == 0 && InputStream.Num() <= HalfInputBufferSize)
		{
			//Find the first correct direction that is within the SequenceLength threshold
			if (TempRequierdDirections.IsValidIndex(0))
			{
				if (CouldFindValidDirectionAndIndex(
					OneStateLink,
					TempRequierdDirections[0],
					InputStream,
					false,
					FirstDirInputIndex,
					InputStream.Num(),
					TempReturnIndex,
					LastIndexTimeStamp,
					previousTimeStamp,
					allowButtons))
				{

					TempRequierdDirections[0].FoundThisDirInput = true;
					CorrectDirectionalInputs++;
					FirstDirInputIndex = TempReturnIndex;
					previousTimeStamp = InputStream[TempReturnIndex].TimeStamp;
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
					(InputStream.IsValidIndex(LastDirInputIndex)))
				{

					if (CouldFindValidDirectionAndIndex(
						OneStateLink,
						TempRequierdDirections[z],
						InputStream,
						true,
						FirstDirInputIndex,
						LastDirInputIndex,
						TempReturnIndex,
						LastIndexTimeStamp,
						previousTimeStamp,
						allowButtons))
					{
						TempRequierdDirections[z].FoundThisDirInput = true;
						dirCheckSteps--;
						CorrectDirectionalInputs++;
						LastDirInputIndex = TempReturnIndex;
						previousTimeStamp = InputStream[TempReturnIndex].TimeStamp;
					}
				}
			}
		}
		dirCheckSteps--;
	}

	return CorrectDirectionalInputs;
}

bool UStateMachineBase::CouldFindValidDirectionAndIndex(
	FStateLink &OneStateLink,
	FLinkConditonDirection &TempRequierdDirection,
	const TArray<FInputFrame> & InputStream,
	bool ReverseLoop,
	int32 Start,
	int32 End,
	int32 &TempReturnIndex,
	float LastIndexTimeStamp,
	float PreviousTimeStamp,
	bool allowButtons)
{

	bool RetFlag = false;
	int LoopStart = ((ReverseLoop) ? End : Start);
	int LoopStop = ((ReverseLoop) ? Start : End);
	int LoopStep = ((ReverseLoop) ? -1 : 1);
	int32 InputStreamFrameCounter = 0;

	float sequenceLengthFail = (float)OneStateLink.SequenceLengthFailThreshold * DeltaTime;
	int32 frameGapFail = OneStateLink.FrameGapFailThreshold;

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

		bool isThisInputAlreadyFound = TempRequierdDirection.FoundThisDirInput == true;
		if (isThisInputAlreadyFound)
		{
			continue;
		}

		bool isRequiredDirectionNotInInputFrame = !(TempRequierdDirection.RequieredDirection & 1 << (int32)InputStream[i].DirectionalInput);
		if (isRequiredDirectionNotInInputFrame)
		{
			continue;
		}

		if (PreviousTimeStamp > 0.0f)
		{
			auto epsilon = 0.002f;
			auto DeltaTimestamp = PreviousTimeStamp - InputStream[i].TimeStamp;
			bool hasInputAlreadyBeenUsed = DeltaTimestamp < epsilon && DeltaTimestamp > -epsilon;
			if (hasInputAlreadyBeenUsed)
			{
				continue;
			}
		}

		if (!(LastIndexTimeStamp >= InputStream[i].TimeStamp))
		{
			continue;
		}

		bool isInputFrameNotInSequenceLength = !(LastIndexTimeStamp - InputStream[i].TimeStamp <= sequenceLengthFail);
		if (isInputFrameNotInSequenceLength)
		{
			continue;
		}

		if (allowButtons || InputStream[i].ContainedButtons.Num() == 0)
		{
			RetFlag = true;
			TempReturnIndex = i;
			break;
		}
	}
	return RetFlag;
}


void UStateMachineBase::QueueState(UStateBase * DestiantionState, FStateLink OneStateLink)
{
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