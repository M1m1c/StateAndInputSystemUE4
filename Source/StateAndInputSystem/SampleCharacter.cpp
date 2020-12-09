// Fill out your copyright notice in the DisplayName page of Project Settings.

#include "SampleCharacter.h"
#include "StateMachineBase.h"
#include "StateBase.h"
#include "InputClasses.h"
#include "StateParametersBase.h"


// Sets default values
ASampleCharacter::ASampleCharacter(const FObjectInitializer& ObjectInitializer)
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MyStateMachine = ObjectInitializer.CreateDefaultSubobject<UStateMachineBase>(this, TEXT("StateMachine"));

}

// Called when the game starts or when spawned
void ASampleCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	if (!DefaultState)
	{
		UE_LOG(LogTemp, Error, TEXT("%s @beginPlay no DefaultState set, destroying this character"), *GetName());
		Destroy();
	}

	SetCurrentState(DefaultState);
}

// Called every frame
void ASampleCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (QuedState) { SetCurrentState(QuedState); }
}

void ASampleCharacter::ForwardInputStreamToStateMachine(const TArray<struct FInputFrame>& InputStream)
{
	MyStateMachine->CheckAllStateLinks(CurrentState, InputStream);
}

//----------------------------------SET CURRENT STATE---------------------------------------------------------------------------
//Sets the current state to the new state and initalizes the new state
void ASampleCharacter::SetCurrentState(UStateBase * NewState)
{
	if (NewState == nullptr)
	{		
			NewState = DefaultState;
	}

	//this->StopAnimMontage();
	//this->GetCharacterMovement()->Velocity = FVector::ZeroVector;
	//this->GetRootMotionAnimMontageInstance()->
	CurrentState = NewState;
	CurrentState->InitializeThisState(this);
	QuedState = nullptr;

	bAllowQueuedStateSwitching = false;

	OnStateInfoUpdate.Broadcast(FText::FromName(CurrentState->StateName), CurrentState->ThisStatesParams);

	UE_LOG(LogTemp, Warning, TEXT("%s @SetCurrentState Switching state to: %s"), *GetName(), *NewState->StateName.ToString());
}

//----------------------------------GET CURRENT STATE---------------------------------------------------------------------------
UStateBase * ASampleCharacter::GetCurrentState()
{
	return CurrentState;
}
//----------------------------------GET DEFAULT STATE---------------------------------------------------------------------------
UStateBase* ASampleCharacter::GetDefaultState()
{
	return DefaultState;
}

