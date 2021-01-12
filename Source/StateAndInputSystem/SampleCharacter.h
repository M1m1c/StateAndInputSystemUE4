// © 2018, Ludvig Baummann Olsson. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SampleCharacter.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FStateInfoUIDelegate, FText, stateNameText, class UStateParametersBase *,params);

class UStateBase;

UCLASS()
class STATEANDINPUTSYSTEM_API ASampleCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	FStateInfoUIDelegate OnStateInfoUpdate;

	// Sets default values for this character's properties
	ASampleCharacter(const FObjectInitializer& ObjectInitializer);

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

	float deltaTime = 0.016f;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void ForwardInputStreamToStateMachine(const TArray<struct FInputFrame> &InputStream);

	virtual void SetCurrentState(UStateBase* NewState);
	virtual UStateBase* GetCurrentState();
	virtual UStateBase* GetDefaultState();

	//this is the state that will be switched to when a UBBSwitchQueuedStateThresholdNotify is hit, unlesss it is the defualt state.
	//this gets set in the StateBase class.
	UStateBase * QuedState;

	class UStateMachineBase * MyStateMachine;

	
};
