// Copyright (c) 2020 Spencer Melnick

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Character.h"
#include "Containers/CircularQueue.h"
#include "GameFramework/Character.h"
#include "Threshold/Character/THCharacter.h"
#include "THPlayerController.generated.h"

/**
 * 
 */



UCLASS()
class THRESHOLD_API ATHPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ATHPlayerController();
	ATHPlayerController(FVTableHelper& Helper);


	
	// Engine overrides
	
	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;
	virtual void SetupInputComponent() override;
	virtual void Tick(float DeltaTime) override;
	virtual void AcknowledgePossession(APawn* P) override;


	
	// Movement
	
	void MoveForward(float Scale);
	void MoveRight(float Scale);
	void Dodge();



	// Actions

	void PrimaryAttack();


	
	// Camera controls
	
	void LookUp(float Scale);
	void Turn(float Scale);
	void ToggleTarget();
	void SetTarget(class ICombatant* NewTarget);
	void NextTarget();
	void PreviousTarget();



	// Camera effects

	void ApplyHitShake(FVector Direction, float Amplitude);



	// Accessors

	inline ATHCharacter* GetTHCharacter() const
	{
		return Cast<ATHCharacter>(GetCharacter());
	}
	

	
	// Public properties

	// The rotational offset of the camera relative to facing directly
	// at the target
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Targeting")
	FRotator LockonOffsetRotation;
	
	// How fast the camera can rotate towards a locked on target
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Targeting")
	float LockonRotationSpeed = 500.f;

	// The maximum distance for a valid lockon target
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Targeting")
	float MaxTargetDistance = 500.f;

	// The actor class for the target indicator
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Targeting")
	TSubclassOf<AActor> TargetIndicatorClass;

	// How long an action will stay in the input buffer before being
	// flushed
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Input")
	float ActionBufferTime = 0.5f;

	const int32 InputBufferSize = 2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Effects")
	class UCurveFloat* HitShakeCurve = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Effects")
	float HitShakeDuration = 0.2f;
	

	
	// Holds information about potential lockon targets
	struct FTarget
	{
		class ICombatant* TargetActor;
		FVector2D ScreenPosition;
		float Distance;

		bool operator==(const FTarget& OtherTarget) const;
	};



	// Holds buffered inputs
	struct FBufferedInput
	{
		enum class EInputAction
		{
			Dodge,
			PrimaryAttack
		};

		EInputAction ActionType;
		FVector RecordedInputVector = FVector::ZeroVector;
		float BufferedTime = 0.f;
	};

protected:
	// Helper functions
	
	TArray<FTarget> GetLockonTargets();
	TArray<FTarget> GetSortedLockonTargets();
	TArray<FTarget> GetSortedLockonTargets(int32& CurrentTargetIndex);
	void RotateTowardsTarget(float DeltaTime);


	
	// Virtual functions
	
	virtual bool GetCameraIsDirectlyControlled();

private:
	// Helper functions

	void QueuePlayerInput(FBufferedInput NewInput);
	bool TryConsumePlayerInput(const FBufferedInput* ConsumedInput);


	
	// Cached actors/components

	UPROPERTY()
	AActor* TargetIndicatorActor = nullptr;

	class ATHPlayerCameraManager* ThresholdCameraManager = nullptr;



	// Camera control properties

	UPROPERTY()
	TScriptInterface<ICombatant> LockonTarget = nullptr;



	// Input buffering data

	TCircularQueue<FBufferedInput> InputBuffer;
	
};
