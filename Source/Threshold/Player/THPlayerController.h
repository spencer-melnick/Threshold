// Copyright (c) 2020 Spencer Melnick

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "UObject/WeakInterfacePtr.h"
#include "Threshold/Character/BaseCharacter.h"
#include "Threshold/Effects/Camera/THPlayerCameraManager.h"
#include "THPlayerController.generated.h"



// Forward declarations

class ICombatant;
class IInteractiveObject;



/**
 * This is the base class for the player character controller. It has minimal functionality, providing a targeting
 * system that sets control rotation, and movement inputs. The rest of the character input is handled via gameplay
 * abilities, that are bound in the ability system component
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
	virtual void AcknowledgePossession(APawn* P) override;


	
	// Movement
	
	void MoveForward(float Scale);
	void MoveRight(float Scale);


	
	// Camera controls
	
	void LookUp(float Scale);
	void Turn(float Scale);
	void ToggleTarget();
	void SetTarget(TWeakInterfacePtr<ICombatant> NewTarget);
	
	void SetTarget(ICombatant* NewTarget)
	{
		SetTarget(TWeakInterfacePtr<ICombatant>(*NewTarget));
	}
	
	void NextTarget();
	void PreviousTarget();



	// HUD controls

	void ToggleMenu();



	// Camera effects

	void ApplyHitShake(FVector Direction, float Amplitude);



	// Accessors

	ABaseCharacter* GetBaseCharacter() const
	{
		return Cast<ABaseCharacter>(GetCharacter());
	}

	ATHPlayerCameraManager* GetTHPlayerCameraManager() const
	{
		return Cast<ATHPlayerCameraManager>(PlayerCameraManager);
	}

	TWeakInterfacePtr<IInteractiveObject> GetCurrentInteractiveObject() const
	{
		return CurrentInteractiveObject;
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
	

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Effects")
	class UCurveFloat* HitShakeCurve = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Effects")
	float HitShakeDuration = 0.2f;
	

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Interaction")
	float MaxInteractionDistance = 100.f;
	
	// Actor class for interaction indicator
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Interaction")
	TSubclassOf<AActor> InteractionIndicatorClass;


	
	// Holds information about potential lockon targets
	struct FTarget
	{
		TWeakInterfacePtr<ICombatant> Combatant;
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

	void CheckInteractiveObjects();
	void SetCurrentInteractiveObject(TWeakInterfacePtr<IInteractiveObject> NewObject);


	
	// Virtual functions
	
	virtual bool GetCameraIsDirectlyControlled();

private:
	// Spawned actors
	
	UPROPERTY()
	AActor* TargetIndicatorActor = nullptr;

	UPROPERTY()
	AActor* InteractionIndicatorActor = nullptr;


	
	// Camera control members

	TWeakInterfacePtr<ICombatant> LockonTarget;



	// Interaction control

	TWeakInterfacePtr<IInteractiveObject> CurrentInteractiveObject;
};
