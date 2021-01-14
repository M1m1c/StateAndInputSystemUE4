// © 2018, Ludvig Baummann Olsson. All rights reserved.

#include "SampleCharacter.h"
#include "StateMachineBase.h"
#include "StateBase.h"
#include "InputClasses.h"
#include "StateParametersBase.h"
#include "SamplePlayerController.h"


// Sets default values
ASampleCharacter::ASampleCharacter(const FObjectInitializer& ObjectInitializer)
{
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

	MySamplePlayerController = Cast<ASamplePlayerController>(GetController());

	SetCurrentState(DefaultState);
}

// Called every frame
void ASampleCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	deltaTime = DeltaTime;
	if (QuedState) { SetCurrentState(QuedState); }
}

void ASampleCharacter::ForwardInputStreamToStateMachine(const TArray<struct FInputFrame>& InputStream)
{
	MyStateMachine->CheckAllStateLinks(CurrentState, InputStream, deltaTime);
}

void ASampleCharacter::SetCurrentState(UStateBase * NewState)
{
	if (NewState == nullptr)
	{		
			NewState = DefaultState;
	}

	MySamplePlayerController->ClearInputStream();

	CurrentState = NewState;
	QuedState = nullptr;

	bAllowQueuedStateSwitching = false;

	OnStateInfoUpdate.Broadcast(FText::FromName(CurrentState->StateName), CurrentState->ThisStatesParams);

	UE_LOG(LogTemp, Warning, TEXT("%s @SetCurrentState Switching state to: %s"), *GetName(), *NewState->StateName.ToString());

	//This is where one would call a state initalise function to have the character recive the properties of the new state.
	//An example could be to run initalise state in order to play a new animation on the character.
}

UStateBase * ASampleCharacter::GetCurrentState()
{
	return CurrentState;
}

UStateBase* ASampleCharacter::GetDefaultState()
{
	return DefaultState;
}

