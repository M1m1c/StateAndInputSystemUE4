// Fill out your copyright notice in the DisplayName page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SampleCharacter.generated.h"

class UStateBase;

UCLASS()
class STATEANDINPUTSYSTEM_API ASampleCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ASampleCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//  This is the idle state for the character, this is needed so that each character gets a character specific state machine.
	UPROPERTY(EditAnywhere)
		UStateBase* DefaultState;

	// This is the state that the character is in.
	UPROPERTY(VisibleAnywhere)
		UStateBase* CurrentState;

	bool bAllowQueuedStateSwitching = false;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void SetCurrentState(UStateBase* NewState);
	virtual UStateBase* GetCurrentState();
	virtual UStateBase* GetDefaultState();

	//this is the state that will be switched to when a UBBSwitchQueuedStateThresholdNotify is hit, unlesss it is the defualt state.
	//this gets set in the StateBase class.
	UStateBase * QuedState;


	
};
