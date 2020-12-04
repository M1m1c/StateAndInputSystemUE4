
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

	


	/*if ((FrameSetWindow.X != 0.0f || FrameSetWindow.Y != 0.0f) && ShouldCountTimeInMove == false)
	{
		UE_LOG(LogTemp, Error, TEXT("%s @InitializeThisState Had specified frameset but will not count the time in move becasue it was false"), *GetName());
	}*/

	/*if (!ThisStatesMove)
	{
		UE_LOG(LogTemp, Error, TEXT("%s @InitializeThisState State had No Move returning"), *GetName());
		return;
	}
*/

	
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

