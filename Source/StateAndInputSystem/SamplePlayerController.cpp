// Fill out your copyright notice in the DisplayName page of Project Settings.

#include "SamplePlayerController.h"

#include "Runtime/Engine/Classes/Kismet/KismetSystemLibrary.h"
#include "SampleCharacter.h"
#include "StateBase.h"


ASamplePlayerController::ASamplePlayerController()
{
}

//----------------------------------BEGIN PLAY---------------------------------------------------------------------------
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

	//bActivated = true;
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
		//maybe we need to register tempinput atom before we add it to the list
		ButtonAtoms.Add(TempInputAtom);
		TempInputAtom = NULL;
	}
}

//----------------------------------SETUP INPUT COMPONENT---------------------------------------------------------------------------
void ASamplePlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	UE_LOG(LogTemp, Warning, TEXT("%s @SetupInputComponent was called"), *GetName());

	//InputComponent->BindAxis("XAxis", this, &ASamplePlayerController::ReadXAxis);
	//InputComponent->BindAxis("YAxis", this, &ASamplePlayerController::ReadYAxis);

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


//----------------------------------TICK---------------------------------------------------------------------------
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
		CalculateButtonInput(DebuggInpputStream);
		AddInputToInputStream(DebuggInpputStream);

		RemoveOldInputAtoms();

		//would like to move this to the character, however it seems that the game crashes if we try to cast the inout stream,
		//the crash is really weird too, since there is not error message and the engien does not close it self, so it is like an uncaught nullptr
		if (myCharacter == nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("%s @Tick my pawn was null"), *GetName());
			return;
		}

	/*	myCharacter->MoveYAxis(YAxis);
		myCharacter->MoveXAxis(XAxis);
		myCharacter->SetDirectionMagnitude(XAxis);*/

		if (myCharacter->GetCurrentState() == nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("%s @Tick had no curent state"), *GetName());
			return;
		}

		myCharacter->ForwardInputStreamToStateMachine(InputStream);
		
	}
	else
	{
	/*	myCharacter->MoveYAxis(0.0f);
		myCharacter->MoveXAxis(0.0f);*/
	}
}

void ASamplePlayerController::AxisDirectionToggle(float axis, int32 possitiveDirectionBit, int32 negativeDirectionBit)
{
	if (axis > StickSensitivityThreshold)
	{
		//axis is possitive toggle bit in enum
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
		//axis is not large enought to count as an input
		DirectionFlags &= ~(1 << negativeDirectionBit);
		DirectionFlags &= ~(1 << possitiveDirectionBit);
	}
}

//----------------------------------CALUCLATE DIRECTIONAL INPUT---------------------------------------------------------------------------
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
			//UE_LOG(LogTemp, Warning, TEXT("No Direction"));
			CurrentInputFrame.DirectionalInput = EInputDirections::None;
			break;
		case (1 << (int32)EAxisDirections::XPossitive):
			//Right direction
			UE_LOG(LogTemp, Warning, TEXT("Right Direction"));
			CurrentInputFrame.DirectionalInput = EInputDirections::Right;
			break;
		case (1 << (int32)EAxisDirections::XNegative):
			//Left direction
			UE_LOG(LogTemp, Warning, TEXT("Left Direction"));
			CurrentInputFrame.DirectionalInput = EInputDirections::Left;
			break;
		case (1 << (int32)EAxisDirections::YPossitive):
			//Up direction
			UE_LOG(LogTemp, Warning, TEXT("Up Direction"));
			CurrentInputFrame.DirectionalInput = EInputDirections::Up;
			break;
		case (1 << (int32)EAxisDirections::YNegative):
			//Down direction
			UE_LOG(LogTemp, Warning, TEXT("Down Direction"));
			CurrentInputFrame.DirectionalInput = EInputDirections::Down;
			break;
			case (1 << (int32)EAxisDirections::XPossitive) + (1 << (int32)EAxisDirections::YPossitive) :
				//UpRight direction
				UE_LOG(LogTemp, Warning, TEXT("UpRight Direction"));
				CurrentInputFrame.DirectionalInput = EInputDirections::UpRight;
				break;
				case (1 << (int32)EAxisDirections::XPossitive) + (1 << (int32)EAxisDirections::YNegative) :
					//DownRight direction
					UE_LOG(LogTemp, Warning, TEXT("DownRight Direction"));
					CurrentInputFrame.DirectionalInput = EInputDirections::DownRight;
					break;
					case (1 << (int32)EAxisDirections::XNegative) + (1 << (int32)EAxisDirections::YPossitive) :
						//UpLeft direction
						UE_LOG(LogTemp, Warning, TEXT("UpLeft Direction"));
						CurrentInputFrame.DirectionalInput = EInputDirections::UpLeft;
						break;
						case (1 << (int32)EAxisDirections::XNegative) + (1 << (int32)EAxisDirections::YNegative) :
							//DownLeft direction
							UE_LOG(LogTemp, Warning, TEXT("DownLeft Direction"));
							CurrentInputFrame.DirectionalInput = EInputDirections::DownLeft;
							break;
		}
	}
}

//TODO remove Debug Messages
//----------------------------------CALCULATE BUTTON INPUT---------------------------------------------------------------------------
//Measures the state of buttons and adds them to the input stream
void ASamplePlayerController::CalculateButtonInput(FString DebuggInpputStream)
{
	CurrentInputFrame.ContainedButtons.Reset();

	for (int32 i = 0; i < (int32)EInputButtons::Count; i++)
	{
		if (ButtonAtoms.IsValidIndex(i) == false)
		{
			break;
		}

		DebuggInpputStream += "| " + ButtonAtoms[i]->DisplayName.ToString() + " State-";

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
			DebuggInpputStream += "HeldDown";

		}
		//Button Pushed
		else if (ButtonAtoms[i]->bButtonPushedGate == false)
		{

			ButtonAtoms[i]->bButtonPushedGate = true;
			ButtonAtoms[i]->bButtonHolding = true;
			ButtonAtoms[i]->PreviousButtonState = EButtonState::JustPressed;
			ButtonAtoms[i]->ButtonState = EButtonState::JustPressed;
			DebuggInpputStream += "JustPressed";


		}
		//Button Released
		else if ((previousButtonState == EButtonState::JustPressed || 
			previousButtonState == EButtonState::HeldDown) 
			&& bButtonHolding == false)
		{

			ButtonAtoms[i]->ButtonState = EButtonState::Released;
			ButtonAtoms[i]->PreviousButtonState = EButtonState::Released;

			DebuggInpputStream += "Released";
		}
		//Button Up
		else if (previousButtonState == EButtonState::Released)
		{

			ButtonAtoms[i]->ButtonState = EButtonState::Up;
			ButtonAtoms[i]->PreviousButtonState = EButtonState::Up;
			ButtonAtoms[i]->bButtonHolding = false;
			ButtonAtoms[i]->bButtonPushedGate = true;
			DebuggInpputStream += "Up";

		}
		// This is essentially an inbetween state, it is used to catch the button state when it is not pressed or held down and has not been released yet.
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

void ASamplePlayerController::AddInputToInputStream(FString & DebuggInpputStream)
{
	if ((DoesButtonAtomsHaveActiveButtons() && !IsAButtonHeldInInputStream()) ||
		(PassOneDirInput && CurrentInputFrame.DirectionalInput != (EInputDirections)0))
	{
		PassOneDirInput = false;
		CurrentInputFrame.TimeStamp = CurrentTime;

		if (InputStream.Num() < InputBufferSize)
		{
			InputStream.Add(CurrentInputFrame);
			UE_LOG(LogTemp, Warning, TEXT("Input stream: %s"), *DebuggInpputStream);
		}
		else
		{
			InputStream.RemoveAt(0, 1, false);
			InputStream.Add(CurrentInputFrame);
			UE_LOG(LogTemp, Warning, TEXT("Input stream: %s"), *DebuggInpputStream);
		}
		OnInputStreamUpdate.Broadcast(this);
	}
}

//----------------------------------REMOVE OLD INPUT ATOMS---------------------------------------------------------------------------
//Removes inputs in the input stream that have lived to the expiration time.
void ASamplePlayerController::RemoveOldInputAtoms()
{
	//TODO make so that held buttons don't get removed unless they change state to released or up
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

//----------------------------------DOES BUTTON ATOMS HAVE ACTIVE BUTTONS---------------------------------------------------------------------------
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

//----------------------------------IS A BUTTON HELD IN INPUTSTREAM---------------------------------------------------------------------------
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

//----------------------------------IS A BUTTON ALREADY HELD---------------------------------------------------------------------------
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

//----------------------------------FIND SPECIFIC BUTTON IN ARRAY---------------------------------------------------------------------------
UInputButtonClass* ASamplePlayerController::FindSpecificButtonInArray(TArray<UInputButtonClass*>& collection, EInputButtons button)
{
	return *collection.FindByPredicate([&](const UInputButtonClass* InItem)
	{
		return InItem->ButtonInput == button;
	});
}

//----------------------------------READ AXIS---------------------------------------------------------------------------
void ASamplePlayerController::ReadYAxis(float value)
{

	YAxis = value;

}

void ASamplePlayerController::ReadXAxis(float value)
{

	XAxis = value;

}

//----------------------------------CLEAR INPUT STREAM---------------------------------------------------------------------------
//Clears the input stream when called
void ASamplePlayerController::ClearInputStream()
{
	UE_LOG(LogTemp, Warning, TEXT("%s @ClearInputStream Clear inpustream was called"), *GetName());
	InputStream.Empty();
}

//----------------------------------CONSUME ONE INPUT---------------------------------------------------------------------------
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

//----------------------------------GET INPUT STREAM REFERENCE---------------------------------------------------------------------------
TArray<FInputFrame>& ASamplePlayerController::GetInputStreamReference()
{
	return InputStream;;
}



UInputButtonClass* ASamplePlayerController::PressButton(EInputButtons type, float time)
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
	return retValue;
}

UInputButtonClass * ASamplePlayerController::ReleaseButton(EInputButtons type, float time)
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
	return retValue;
}

//----------------------------------LEFT FACE BUTTON---------------------------------------------------------------------------
void ASamplePlayerController::LeftFaceButtonPressed()
{
	DebuggTimer = 0;
	float TimeWhenEntered = CurrentTime;
	LeftFaceButton = PressButton(EInputButtons::LeftFace, TimeWhenEntered);
}

void ASamplePlayerController::LeftFaceButtonReleased()
{
	float TimeWhenEntered = CurrentTime;
	LeftFaceButton = ReleaseButton(EInputButtons::LeftFace, TimeWhenEntered);
}

//----------------------------------TOP FACE BUTTON---------------------------------------------------------------------------
void ASamplePlayerController::TopFaceButtonPressed()
{
	DebuggTimer = 0;
	float TimeWhenEntered = CurrentTime;
	TopFaceButton = PressButton(EInputButtons::TopFace, TimeWhenEntered);
}

void ASamplePlayerController::TopFaceButtonReleased()
{
	float TimeWhenEntered = CurrentTime;
	TopFaceButton = ReleaseButton(EInputButtons::TopFace, TimeWhenEntered);
}

//----------------------------------RIGHT FACE BUTTON---------------------------------------------------------------------------
void ASamplePlayerController::RightFaceButtonPressed()
{
	DebuggTimer = 0;
	float TimeWhenEntered = CurrentTime;
	RightFaceButton = PressButton(EInputButtons::RightFace, TimeWhenEntered);
}

void ASamplePlayerController::RightFaceButtonReleased()
{
	float TimeWhenEntered = CurrentTime;
	RightFaceButton = ReleaseButton(EInputButtons::RightFace, TimeWhenEntered);
}

//----------------------------------BOTTOM FACE BUTTON---------------------------------------------------------------------------
void ASamplePlayerController::BottomFaceButtonPressed()
{
	DebuggTimer = 0;
	float TimeWhenEntered = CurrentTime;
	BottomFaceButton = PressButton(EInputButtons::BottomFace, TimeWhenEntered);
}

void ASamplePlayerController::BottomFaceButtonReleased()
{
	float TimeWhenEntered = CurrentTime;
	BottomFaceButton = ReleaseButton(EInputButtons::BottomFace, TimeWhenEntered);
}

//----------------------------------RIGHT BUMPER---------------------------------------------------------------------------
void ASamplePlayerController::RightBumperPressed()
{
	DebuggTimer = 0;
	float TimeWhenEntered = CurrentTime;
	RightBumper = PressButton(EInputButtons::RightBumper, TimeWhenEntered);
}

void ASamplePlayerController::RightBumperReleased()
{
	float TimeWhenEntered = CurrentTime;
	RightBumper = ReleaseButton(EInputButtons::RightBumper, TimeWhenEntered);
}

//----------------------------------LEFT BUMPER---------------------------------------------------------------------------
void ASamplePlayerController::LeftBumperPressed()
{
	DebuggTimer = 0;
	float TimeWhenEntered = CurrentTime;
	LeftBumper = PressButton(EInputButtons::LeftBumper, TimeWhenEntered);
}

void ASamplePlayerController::LeftBumperReleased()
{
	float TimeWhenEntered = CurrentTime;
	LeftBumper = ReleaseButton(EInputButtons::LeftBumper, TimeWhenEntered);
}


//----------------------------------GET INPUT BUTTON PRESSED---------------------------------------------------------------------------
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


