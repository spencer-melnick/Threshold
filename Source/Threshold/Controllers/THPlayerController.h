// Copyright © 2020 Spencer Melnick

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
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


	
	// Engine overrides
	
	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;
	virtual void SetupInputComponent() override;
	virtual void Tick(float DeltaTime) override;


	
	// Movement
	
	void MoveForward(float Scale);
	void MoveRight(float Scale);
	void Dodge();


	
	// Camera controls
	
	void LookUp(float Scale);
	void Turn(float Scale);
	void ToggleTarget();
	void SetTarget(AActor* NewTarget);
	void NextTarget();
	void PreviousTarget();

	
	// Public properties

	// The rotational offset of the camera relative to facing directly
	// at the target
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Targeting")
	FRotator LockonOffsetRotation;
	
	// How fast the camera can rotate towards a locked on target
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Targeting")
	float LockonRotationSpeed = 500.f;

	// The tag of any actor that can be locked on to
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Targeting")
	FName LockonTargetTag = TEXT("lockon");

	// The maximum distance for a valid lockon target
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Targeting")
	float MaxTargetDistance = 500.f;

	// The actor class for the target indicator
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Targeting")
	TSubclassOf<AActor> TargetIndicatorClass;
	

	
	// Holds information about potential lockon targets
	struct FTarget
	{
		AActor* TargetActor;
		FVector2D ScreenPosition;
		float Distance;

		bool operator==(const FTarget& OtherTarget) const;
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
	// Cached actors/components
	
	class ATHCharacter* PossessedCharacter = nullptr;
	AActor* TargetIndicatorActor = nullptr;



	// Camera control properties
	
	AActor* LockonTarget = nullptr;
	
};
