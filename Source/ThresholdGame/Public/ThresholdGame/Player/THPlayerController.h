// Copyright (c) 2020 Spencer Melnick

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "UObject/WeakInterfacePtr.h"
#include "ThresholdGame/Character/BaseCharacter.h"
#include "ThresholdGame/Effects/Camera/THPlayerCameraManager.h"
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
class THRESHOLDGAME_API ATHPlayerController : public APlayerController
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
	virtual void InitPlayerState() override;
	virtual void OnRep_PlayerState() override;



	// HUD controls

	void ToggleMenu();



	// Camera effects

	void ApplyHitShake(FVector Direction, float Amplitude);



	// Input controls

	void SetPawnInputEnabled(const bool bNewPawnInputEnabled);



	// Accessors

	ABaseCharacter* GetBaseCharacter() const { return Cast<ABaseCharacter>(GetCharacter()); }
	ATHPlayerCameraManager* GetTHPlayerCameraManager() const { return Cast<ATHPlayerCameraManager>(PlayerCameraManager); }
	bool IsPawnInputEnabled() const { return bPawnInputEnabled; }
	TWeakInterfacePtr<IInteractiveObject> GetCurrentInteractiveObject() const { return CurrentInteractiveObject; }


	
	// Public properties

	// The maximum distance for a valid lockon target
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Targeting")
	float MaxTargetDistance = 500.f;	

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Effects")
	class UCurveFloat* HitShakeCurve = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Effects")
	float HitShakeDuration = 0.2f;
	

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Interaction")
	float MaxInteractionDistance = 100.f;
	
	// Actor class for interaction indicator
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Interaction")
	TSubclassOf<AActor> InteractionIndicatorClass;
	
	

protected:
	// Helper functions

	void CheckInteractiveObjects();
	void SetCurrentInteractiveObject(TWeakInterfacePtr<IInteractiveObject> NewObject);

	
	// UI initialization

	void InitializePlayerStateUI();



private:
	// Spawned actors

	UPROPERTY()
	AActor* InteractionIndicatorActor = nullptr;



	// Interaction control

	TWeakInterfacePtr<IInteractiveObject> CurrentInteractiveObject;



	// Input control

	bool bPawnInputEnabled = true;
};
