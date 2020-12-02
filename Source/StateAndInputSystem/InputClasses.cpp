// Fill out your copyright notice in the Description page of Project Settings.

#include "InputClasses.h"

void FInputButtonStruct::SetValuesFromClass(UInputButtonClass *inputButton)
{
	DisplayName = inputButton->DisplayName;
	ButtonInput = inputButton->ButtonInput;
	ButtonState = inputButton->ButtonState;
	PreviousButtonState = inputButton->PreviousButtonState;
	InputTimeStamp = inputButton->InputTimeStamp;
	BeginHoldingTimeStamp = inputButton->BeginHoldingTimeStamp;
	bButtonHolding = inputButton->bButtonHolding;
	bButtonPushedGate = inputButton->bButtonPushedGate;
}

void FInputButtonStruct::ResetToDefault(EInputButtons button)
{
	DisplayName = "none";
	ButtonInput = button;
	ButtonState = EButtonState::Up;
	InputTimeStamp = 0.0f;
	BeginHoldingTimeStamp = 0.0f;
	bButtonPushedGate = true;
	bButtonHolding = false;
}