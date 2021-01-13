// © 2018, Ludvig Baummann Olsson. All rights reserved.

#include "SamplePlayerController.h"

#include "Runtime/Engine/Classes/Kismet/KismetSystemLibrary.h"
#include "SampleCharacter.h"
#include "StateBase.h"


ASamplePlayerController::ASamplePlayerController()
{
}

void ASamplePlayerController::BeginPlay()
{
	Super::BeginPlay();

	InitaliseButtonAtoms();

	CurrentInputFrame.DirectionalInput = EInputDirections::None;

	int32 ButtonInputCount = (int32)EInputButtons::Count;
	for (int32 i = 0; i < ButtonInputCount; i++)
	{
		if (!ButtonAtoms.IsValidIndex(i) || !ButtonAtoms[i])
		{
			UE_LOG(LogTemp, Warning, TEXT("%s @BeginPlay Not enough button input atoms or a NULL entry was found in the list"), *GetName());
			Destroy();
			return;
		}

	}

	myCharacter = Cast<ASampleCharacter>(GetPawn());
}

void ASamplePlayerController::InitaliseButtonAtoms()
{
	for (int32 i = 0; i < (int32)EInputButtons::Count; i++)
	{
		UInputButtonClass * TempInputAtom = NULL;
		switch (i)
		{
		case 0:
			TempInputAtom = NewObject<UInputButtonClass>(this, FName("LeftFace"));
			TempInputAtom->DisplayName = FName("LeftFace");
			TempInputAtom->ButtonInput = EInputButtons::LeftFace;
			TempInputAtom->ButtonState = EButtonState::Up;
			break;
		case 1:
			TempInputAtom = NewObject<UInputButtonClass>(this, FName("RightFace"));
			TempInputAtom->DisplayName = FName("RightFace");
			TempInputAtom->ButtonInput = EInputButtons::RightFace;
			TempInputAtom->ButtonState = EButtonState::Up;
			break;
		case 2:
			TempInputAtom = NewObject<UInputButtonClass>(this, FName("TopFace"));
			TempInputAtom->DisplayName = FName("TopFace");
			TempInputAtom->ButtonInput = EInputButtons::TopFace;
			TempInputAtom->ButtonState = EButtonState::Up;
			break;
		case 3:
			TempInputAtom = NewObject<UInputButtonClass>(this, FName("BottomFace"));
			TempInputAtom->DisplayName = FName("BottomFace");
			TempInputAtom->ButtonInput = EInputButtons::BottomFace;
			TempInputAtom->ButtonState = EButtonState::Up;
			break;
		case 4:
			TempInputAtom = NewObject<UInputButtonClass>(this, FName("LeftBumper"));
			TempInputAtom->DisplayName = FName("LeftBumper");
			TempInputAtom->ButtonInput = EInputButtons::LeftBumper;
			TempInputAtom->ButtonState = EButtonState::Up;
			break;
		case 5:
			TempInputAtom = NewObject<UInputButtonClass>(this, FName("RightBumper"));
			TempInputAtom->DisplayName = FName("RightBumper");
			TempInputAtom->ButtonInput = EInputButtons::RightBumper;
			TempInputAtom->ButtonState = EButtonState::Up;
			break;
		}
		ButtonAtoms.Add(TempInputAtom);
		TempInputAtom = NULL;
	}
}

void ASamplePlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	UE_LOG(LogTemp, Warning, TEXT("%s @SetupInputComponent was called"), *GetName());

	InputComponent->BindAxis("LeftStickValue_Y", this, &ASamplePlayerController::ReadYAxis);
	InputComponent->BindAxis("LeftStickValue_X", this, &ASamplePlayerController::ReadXAxis);

	InputComponent->BindAction("LeftFaceButton", IE_Pressed, this, &ASamplePlayerController::LeftFaceButtonPressed);
	InputComponent->BindAction("LeftFaceButton", IE_Released, this, &ASamplePlayerController::LeftFaceButtonReleased);

	InputComponent->BindAction("TopFaceButton", IE_Pressed, this, &ASamplePlayerController::TopFaceButtonPressed);
	InputComponent->BindAction("TopFaceButton", IE_Released, this, &ASamplePlayerController::TopFaceButtonReleased);

	InputComponent->BindAction("RightFaceButton", IE_Pressed, this, &ASamplePlayerController::RightFaceButtonPressed);
	InputComponent->BindAction("RightFaceButton", IE_Released, this, &ASamplePlayerController::RightFaceButtonReleased);

	InputComponent->BindAction("BottomFaceButton", IE_Pressed, this, &ASamplePlayerController::BottomFaceButtonPressed);
	InputComponent->BindAction("BottomFaceButton", IE_Released, this, &ASamplePlayerController::BottomFaceButtonReleased);

	InputComponent->BindAction("RightBumper", IE_Pressed, this, &ASamplePlayerController::RightBumperPressed);
	InputComponent->BindAction("RightBumper", IE_Released, this, &ASamplePlayerController::RightBumperReleased);

	InputComponent->BindAction("LeftBumper", IE_Pressed, this, &ASamplePlayerController::LeftBumperPressed);
	InputComponent->BindAction("LeftBumper", IE_Released, this, &ASamplePlayerController::LeftBumperReleased);
}


void ASamplePlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (bActivated)
	{
		DebuggTimer += DeltaSeconds;

		CurrentTime = UKismetSystemLibrary::GetGameTimeInSeconds(this);

		AxisDirectionToggle(XAxis, (int32)EAxisDirections::XPossitive, (int32)EAxisDirections::XNegative);
		AxisDirectionToggle(YAxis, (int32)EAxisDirections::YPossitive, (int32)EAxisDirections::YNegative);
		CalculateDirectionalInput();

		FString DebuggInpputStream = "";
		CalculateButtonInput();
		AddInputToInputStream();

		RemoveOldInputAtoms();

		if (myCharacter == nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("%s @Tick my pawn was null"), *GetName());
			return;
		}

		if (myCharacter->GetCurrentState() == nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("%s @Tick had no curent state"), *GetName());
			return;
		}

		myCharacter->ForwardInputStreamToStateMachine(InputStream);
		
	}
}

void ASamplePlayerController::AxisDirectionToggle(float axis, int32 possitiveDirectionBit, int32 negativeDirectionBit)
{
	if (axis > StickSensitivityThreshold)
	{
		//axis is positive toggle bit in enum
		DirectionFlags |= 1 << possitiveDirectionBit;
		DirectionFlags &= ~(1 << negativeDirectionBit);

	}
	else if (axis < -StickSensitivityThreshold)
	{
		//axis is negative toggle bit in enum
		DirectionFlags |= 1 << negativeDirectionBit;
		DirectionFlags &= ~(1 << possitiveDirectionBit);
	}
	else
	{
		//axis is not large enough to count as an input
		DirectionFlags &= ~(1 << negativeDirectionBit);
		DirectionFlags &= ~(1 << possitiveDirectionBit);
	}
}

void ASamplePlayerController::CalculateDirectionalInput()
{
	if (PreviousDirection != DirectionFlags)
	{
		PreviousDirection = DirectionFlags;
		PassOneDirInput = true;

		switch (DirectionFlags)
		{
		case 0:
			//no direction is being input
			CurrentInputFrame.DirectionalInput = EInputDirections::None;
			break;
		case (1 << (int32)EAxisDirections::XPossitive):
			//Right direction
			CurrentInputFrame.DirectionalInput = EInputDirections::Right;
			break;
		case (1 << (int32)EAxisDirections::XNegative):
			//Left direction
			CurrentInputFrame.DirectionalInput = EInputDirections::Left;
			break;
		case (1 << (int32)EAxisDirections::YPossitive):
			//Up direction
			CurrentInputFrame.DirectionalInput = EInputDirections::Up;
			break;
		case (1 << (int32)EAxisDirections::YNegative):
			//Down direction
			CurrentInputFrame.DirectionalInput = EInputDirections::Down;
			break;
		case (1 << (int32)EAxisDirections::XPossitive) + (1 << (int32)EAxisDirections::YPossitive) :
			//UpRight direction
			CurrentInputFrame.DirectionalInput = EInputDirections::UpRight;
			break;
		case (1 << (int32)EAxisDirections::XPossitive) + (1 << (int32)EAxisDirections::YNegative) :
			//DownRight direction
			CurrentInputFrame.DirectionalInput = EInputDirections::DownRight;
			break;
		case (1 << (int32)EAxisDirections::XNegative) + (1 << (int32)EAxisDirections::YPossitive) :
			//UpLeft direction
			CurrentInputFrame.DirectionalInput = EInputDirections::UpLeft;
			break;
		case (1 << (int32)EAxisDirections::XNegative) + (1 << (int32)EAxisDirections::YNegative) :
			//DownLeft direction
			CurrentInputFrame.DirectionalInput = EInputDirections::DownLeft;
			break;
		}
	}
}

//Measures the state of buttons and adds them to the current Input Frame
void ASamplePlayerController::CalculateButtonInput()
{
	CurrentInputFrame.ContainedButtons.Reset();

	for (int32 i = 0; i < (int32)EInputButtons::Count; i++)
	{
		if (ButtonAtoms.IsValidIndex(i) == false)
		{
			break;
		}


		auto previousButtonState = ButtonAtoms[i]->PreviousButtonState;
		auto bButtonHolding = ButtonAtoms[i]->bButtonHolding;
		auto beginHoldingTimeStamp = ButtonAtoms[i]->BeginHoldingTimeStamp;

		if (bButtonHolding == false)
		{
			ButtonAtoms[i]->BeginHoldingTimeStamp = CurrentTime;
		}

		//Button Held Down
		if ((CurrentTime > beginHoldingTimeStamp + TimeTilButtonHeldDown) && bButtonHolding == true)
		{
			ButtonAtoms[i]->PreviousButtonState = EButtonState::HeldDown;
			ButtonAtoms[i]->ButtonState = EButtonState::HeldDown;

		}
		//Button Pushed
		else if (ButtonAtoms[i]->bButtonPushedGate == false)
		{

			ButtonAtoms[i]->bButtonPushedGate = true;
			ButtonAtoms[i]->bButtonHolding = true;
			ButtonAtoms[i]->PreviousButtonState = EButtonState::JustPressed;
			ButtonAtoms[i]->ButtonState = EButtonState::JustPressed;
		}
		//Button Released
		else if ((previousButtonState == EButtonState::JustPressed || 
			previousButtonState == EButtonState::HeldDown) 
			&& bButtonHolding == false)
		{

			ButtonAtoms[i]->ButtonState = EButtonState::Released;
			ButtonAtoms[i]->PreviousButtonState = EButtonState::Released;

		}
		//Button Up
		else if (previousButtonState == EButtonState::Released)
		{
			ButtonAtoms[i]->ButtonState = EButtonState::Up;
			ButtonAtoms[i]->PreviousButtonState = EButtonState::Up;
			ButtonAtoms[i]->bButtonHolding = false;
			ButtonAtoms[i]->bButtonPushedGate = true;
		}
		// This is essentially an inbetween state, it is used to catch the button state when it is not pressed or held down,
		// and has not been released yet.
		// A button in a Count state must never be added to the input stream or bad things might happen.
		else
		{
			ButtonAtoms[i]->ButtonState = EButtonState::Count;
		}

		if (ButtonAtoms[i]->ButtonState != EButtonState::Count)
		{
			ButtonAtoms[i]->InputTimeStamp = CurrentTime;
			CurrentInputFrame.ContainedButtons.Add(FInputButtonStruct(ButtonAtoms[i]));
		}
	}
}

//Adds a reference to the current inputframe to the inputstream array
void ASamplePlayerController::AddInputToInputStream()
{
	if ((DoesButtonAtomsHaveActiveButtons() && !IsAButtonHeldInInputStream()) ||
		(PassOneDirInput && CurrentInputFrame.DirectionalInput != (EInputDirections)0))
	{
		PassOneDirInput = false;
		CurrentInputFrame.TimeStamp = CurrentTime;

		if (InputStream.Num() < InputBufferSize)
		{
			InputStream.Add(CurrentInputFrame);
		}
		else
		{
			InputStream.RemoveAt(0, 1, false);
			InputStream.Add(CurrentInputFrame);
		}
		OnInputStreamUpdate.Broadcast(this);
	}
}

//Removes inputs in the input stream that have lived to the expiration time.
void ASamplePlayerController::RemoveOldInputAtoms()
{

	if (InputStream.Num() <= 0)
	{
		return;
	}

	for (int32 i = InputStream.Num() - 1; i > -1; i--)
	{
		if (InputStream.IsValidIndex(i) == false)
		{
			break;
		}
		auto inputFrame = InputStream[i];

		if (inputFrame.TimeStamp + InputExpirationTime < CurrentTime &&
			!IsACurrentButtonHeldInArray(inputFrame.ContainedButtons))
		{
			InputStream.RemoveAt(i, 1, false);
			break;
		}
	}

}

bool ASamplePlayerController::DoesButtonAtomsHaveActiveButtons()
{
	bool retflag = false;
	for (int32 i = 0; i < (int32)EInputButtons::Count; i++)
	{
		auto buttonState = ButtonAtoms[i]->ButtonState;
		auto previousButtonState = ButtonAtoms[i]->PreviousButtonState;

		if (buttonState != EButtonState::Up && previousButtonState != EButtonState::Up && buttonState != EButtonState::Count)
		{
			retflag = true;
			break;
		}
	}
	return retflag;
}

bool ASamplePlayerController::IsAButtonHeldInInputStream()
{
	bool retflag = false;
	if (InputStream.Num())
	{
		for (int32 i = 0; i < InputStream.Num(); i++)
		{
			if (InputStream.IsValidIndex(i))
			{
				retflag = IsACurrentButtonHeldInArray(InputStream[i].ContainedButtons);
			}
		}
	}
	return retflag;
}

// returns true if a held button in frameButtons array is the same in ButtonAtoms (Current active butons)  
bool ASamplePlayerController::IsACurrentButtonHeldInArray(TArray<FInputButtonStruct>& frameButtons)
{
	bool retflag = false;

	for (int32 g = 0; g < frameButtons.Num(); g++)
	{
		if (frameButtons[g].ButtonState != EButtonState::HeldDown)
		{
			continue;
		}

		auto FoundButton = FindSpecificButtonInArray(ButtonAtoms, frameButtons[g].ButtonInput);
		if (FoundButton != nullptr)
		{
			if (FoundButton->ButtonState == EButtonState::HeldDown)
			{
				retflag = true;
				break;
			}
		}
	}

	return retflag;
}

UInputButtonClass* ASamplePlayerController::FindSpecificButtonInArray(TArray<UInputButtonClass*>& collection, EInputButtons button)
{
	return *collection.FindByPredicate([&](const UInputButtonClass* InItem)
	{
		return InItem->ButtonInput == button;
	});
}

void ASamplePlayerController::ReadYAxis(float value)
{

	YAxis = value;

}

void ASamplePlayerController::ReadXAxis(float value)
{

	XAxis = value;

}

//Clears the input stream when called
void ASamplePlayerController::ClearInputStream()
{
	UE_LOG(LogTemp, Warning, TEXT("%s @ClearInputStream Clear inpustream was called"), *GetName());
	InputStream.Empty();
}

//Removes one input in the input stream
void ASamplePlayerController::ConsumeOneInput(int32 InputListPosition)
{
	if (InputStream.IsValidIndex(InputListPosition))
	{
		InputStream.RemoveAt(InputListPosition, 1, false);
		UE_LOG(LogTemp, Warning, TEXT("%s @ConsumeOneInput %d Input was consummed"), *GetName(), InputListPosition);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("%s @ConsumeOneInput %d was not a valid index in InputStream"), *GetName(), InputListPosition);
		return;
	}

}

TArray<FInputFrame>& ASamplePlayerController::GetInputStreamReference()
{
	return InputStream;;
}

void ASamplePlayerController::PressButton(EInputButtons type, float time)
{
	auto retValue = GetInputButtonPressed(type);
	if (retValue != false)
	{
		retValue->bButtonPushedGate = false;
		retValue->InputTimeStamp = time;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("%s @PressButton could not find desired button in list of input atoms"), *GetName());
	}
}

//Takes in a button type, gets the button from ButtonAtoms via GetInputButtonPressed and toggles the bool values of the button to reflect that it is released
void ASamplePlayerController::ReleaseButton(EInputButtons type, float time)
{
	auto retValue = GetInputButtonPressed(type);
	if (retValue != false)
	{
		retValue->bButtonHolding = false;
		retValue->bButtonPushedGate = true;
		retValue->InputTimeStamp = time;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("%s @ReleaseButton could not find desired button in list of input atoms"), *GetName());
	}
}

void ASamplePlayerController::LeftFaceButtonPressed()
{
	DebuggTimer = 0;
	float TimeWhenEntered = CurrentTime;
	PressButton(EInputButtons::LeftFace, TimeWhenEntered);
}

void ASamplePlayerController::LeftFaceButtonReleased()
{
	float TimeWhenEntered = CurrentTime;
	ReleaseButton(EInputButtons::LeftFace, TimeWhenEntered);
}

void ASamplePlayerController::TopFaceButtonPressed()
{
	DebuggTimer = 0;
	float TimeWhenEntered = CurrentTime;
	PressButton(EInputButtons::TopFace, TimeWhenEntered);
}

void ASamplePlayerController::TopFaceButtonReleased()
{
	float TimeWhenEntered = CurrentTime;
	ReleaseButton(EInputButtons::TopFace, TimeWhenEntered);
}

void ASamplePlayerController::RightFaceButtonPressed()
{
	DebuggTimer = 0;
	float TimeWhenEntered = CurrentTime;
	PressButton(EInputButtons::RightFace, TimeWhenEntered);
}

void ASamplePlayerController::RightFaceButtonReleased()
{
	float TimeWhenEntered = CurrentTime;
	ReleaseButton(EInputButtons::RightFace, TimeWhenEntered);
}

void ASamplePlayerController::BottomFaceButtonPressed()
{
	DebuggTimer = 0;
	float TimeWhenEntered = CurrentTime;
	PressButton(EInputButtons::BottomFace, TimeWhenEntered);
}

void ASamplePlayerController::BottomFaceButtonReleased()
{
	float TimeWhenEntered = CurrentTime;
	ReleaseButton(EInputButtons::BottomFace, TimeWhenEntered);
}

void ASamplePlayerController::RightBumperPressed()
{
	DebuggTimer = 0;
	float TimeWhenEntered = CurrentTime;
	PressButton(EInputButtons::RightBumper, TimeWhenEntered);
}

void ASamplePlayerController::RightBumperReleased()
{
	float TimeWhenEntered = CurrentTime;
	ReleaseButton(EInputButtons::RightBumper, TimeWhenEntered);
}

void ASamplePlayerController::LeftBumperPressed()
{
	DebuggTimer = 0;
	float TimeWhenEntered = CurrentTime;
	PressButton(EInputButtons::LeftBumper, TimeWhenEntered);
}

void ASamplePlayerController::LeftBumperReleased()
{
	float TimeWhenEntered = CurrentTime;
	ReleaseButton(EInputButtons::LeftBumper, TimeWhenEntered);
}


//Gets the UInputButtonClass of the button sent in
UInputButtonClass * ASamplePlayerController::GetInputButtonPressed(EInputButtons ButtonInput)
{

	UInputButtonClass* ReturnPointer = nullptr;

	for (int32 i = 0; i < ButtonAtoms.Num(); i++)
	{
		if (ButtonAtoms[i]->ButtonInput == ButtonInput)
		{
			ReturnPointer = ButtonAtoms[i];
			break;
		}
		else
		{
			ReturnPointer = nullptr;
		}

	}
	return ReturnPointer;
}


