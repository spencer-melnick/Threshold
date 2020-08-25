// Copyright (c) 2020 Spencer Melnick

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Threshold/Combat/Teams.h"
#include "BaseCharacter.generated.h"




// Forward declarations

class USkeletalMeshSocket;
class UTHGameplayAbility;
class UTHAbilitySystemComponent;



/**
 * This is the custom base class for all characters used in the game!
 */
UCLASS()
class THRESHOLD_API ABaseCharacter : public ACharacter, public ICombatant
{
	GENERATED_BODY()

public:
	ABaseCharacter(const FObjectInitializer& ObjectInitializer);



	// Engine overrides

	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void Tick(float DeltaSeconds) override;

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	virtual void PossessedBy(AController* NewController) override;



	// Combatant overrides

	virtual TSubclassOf<UTeam> GetTeam() const override;
	
	virtual FVector GetTargetLocation() const override;

	virtual void AttachTargetIndicator(AActor* TargetIndicator) override;



	// Accessors

	const USkeletalMeshSocket* GetTargetSocket() const;

	// Returns the character's movement speed in actor space, scaled so that a magnitude of 1 corresponds to the
	// character moving at their max walk speed
	UFUNCTION(BlueprintCallable)
	FVector GetLocalMovementVectorScaled() const;

	// Returns the character's normalized movement speed in actor space
	UFUNCTION(BlueprintCallable)
	FVector GetLocalMovementVectorNormalized() const;

	// Returns the location in world space for targeting look calculations
	UFUNCTION(BlueprintCallable)
	FVector GetWorldLookLocation() const;



	// Editor properties
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Abilities")
	TArray<TSubclassOf<UTHGameplayAbility>> StartingAbilities;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Combat")
	TSubclassOf<UTeam> TeamClass;

	// If set to true, the target location and attachment point will be set based on the skeletal mesh socket named
	// TargetSocketName. Otherwise the target location will use target offset location.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Combat")
	bool bUseSocketForTargetLocation = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Combat", meta=(EditCondition="!bUseSocketForTargetLocation"))
	FVector RelativeTargetLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Combat", meta=(EditCondition="bUseSocketForTargetLocation"))
	FName TargetSocketName;

	// This location is used for determining the direction of the camera when locking on. It doesn't support sockets
	// because sockets move a lot during animation and using fast changing positions can cause unnecessary camera shake.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Combat")
	FVector RelativeLookLocation;



	// Accessors
	
	UTHAbilitySystemComponent* GetAbilitySystemComponent() const
	{
		return AbilitySystemComponent;
	}
	


	// Component name constants - useful for overriding in derived classes

	static FName AbilitySystemComponentName;


	
private:

	// Helper functions

	void GrantStartingAbilities();

	

	// Components

	UPROPERTY(Category=Character, VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	UTHAbilitySystemComponent* AbilitySystemComponent = nullptr;


	
	// Private members
	
	bool bWasGrantedStartingAbilities = false;
};
