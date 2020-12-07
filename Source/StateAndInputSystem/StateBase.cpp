
// Fill out your copyright notice in the DisplayName page of Project Settings.

#include "StateBase.h"
#include "SampleCharacter.h"
#include "SamplePlayerController.h"
//#include "BBSharedStateLinks.h"
//#include "BBSpecialLinkCondition.h"



//----------------------------------INITIALIZE THIS STATE---------------------------------------------------------------------------
void UStateBase::InitializeThisState(ASampleCharacter * Character)
{
	ResetTimeInThisState();
}
//----------------------------------RESET TIME IN THIS STATE---------------------------------------------------------------------------
void UStateBase::ResetTimeInThisState()
{
	TimeInThisState = 0.0f;
}
//----------------------------------UPDATE TIME IN THIS STATE---------------------------------------------------------------------------
void UStateBase::UpdateTimeInThisState(float TimeInState)
{
	TimeInThisState += TimeInState;
	UE_LOG(LogTemp, Warning, TEXT("%f @UpdateTimeInThisState"), TimeInThisState);
}

