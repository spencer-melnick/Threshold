// Copyright (c) 2020 Spencer Melnick

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameplayTags.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "AbilitySystemInterface.h"
#include "Threshold/Combat/Teams.h"
#include "BaseCharacter.generated.h"




// Forward declarations

class USkeletalMeshSocket;
class UCurveFloat;
class UTHGameplayAbility;
class UTHAbilitySystemComponent;
class UBaseAttributeSet;
class ABaseWeapon;



/**
 * This is the custom base class for all characters used in the game!
 */
UCLASS()
class THRESHOLD_API ABaseCharacter : public ACharacter, public ICombatant, public IAbilitySystemInterface
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
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;



	// Combatant overrides

	virtual TSubclassOf<UTeam> GetTeam() const override;
	virtual FVector GetTargetLocation() const override;
	virtual void AttachTargetIndicator(AActor* TargetIndicator) override;



	// Weapon controls

	void EquipWeapon(AActor* NewWeapon);
	void UnequipWeapon();
	


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
	
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	UTHAbilitySystemComponent* GetTHAbilitySystemComponent() const
	{
		return AbilitySystemComponent;
	}

	UFUNCTION(BlueprintCallable, Category="BaseCharacter")
    bool GetIsDodging() const;

	UFUNCTION(BlueprintCallable, Category="BaseCharacter")
	ABaseWeapon* GetEquippedWeapon() const
	{
		return EquippedWeapon;
	}



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

	// This is the tag we check against to see if we are dodging for animation
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Combat")
	FGameplayTag DodgeTag;

	// This is the tag we check against to see if our attack is damaging for local logic
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Combat")
	FGameplayTag DamagingTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Combat")
	TSubclassOf<ABaseWeapon> StartingWeaponClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Combat")
	FName WeaponSocketName = NAME_None;

	// This is the tag for weapon hit events
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Combat")
	FGameplayTag HitEventTag;

	// After this amount of time we stop evaluating hit slowdown
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Effects")
	bool bEnableHitSlowdown = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Effects")
	UCurveFloat* HitSlowdownCurve = nullptr;

	// After this amount of time we stop evaluating hit slowdown
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Effects")
	float MaxHitSlowdownTime = 0.3f;
	


	// Component name constants - useful for overriding in derived classes

	static FName AbilitySystemComponentName;
	static FName BaseAttributeSetName;





protected:
	// Helper functions

	void StartHitSlowdown();
	void EvaluateHitSlowdown(float DeltaTime);
	

	
	// Gameplay tag responses

	virtual void OnDamagingTagChanged(const FGameplayTag CallbackTag, int32 NewCount);
	virtual void OnHitGameplayEvent(FGameplayTag GameplayTag, const FGameplayEventData* EventData);

	
	
	// Network replication functions

	UFUNCTION()
	void OnRep_EquippedWeapon();
	
	
	

	// Replicated variables

	UPROPERTY(ReplicatedUsing = OnRep_EquippedWeapon)
	ABaseWeapon* EquippedWeapon = nullptr;
	
	
	
private:
	// Internal delegates

	void OnDamagingTagChanged_Internal(const FGameplayTag CallbackTag, int32 NewCount)
	{
		OnDamagingTagChanged(CallbackTag, NewCount);
	}

	void OnHitGameplayEvent_Internal(FGameplayTag GameplayTag, const FGameplayEventData* EventData)
	{
		OnHitGameplayEvent(GameplayTag, EventData);
	}
	

	
	// Helper functions

	void GrantStartingAbilities();

	

	// Components

	UPROPERTY(Category=Character, VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	UTHAbilitySystemComponent* AbilitySystemComponent = nullptr;



	// Attribute sets

	UPROPERTY()
	UBaseAttributeSet* BaseAttributeSet = nullptr;


	
	// Private members
	
	bool bWasGrantedStartingAbilities = false;
	bool bHitSlowdownActive = false;
	float AccumulatedHitSlowdownTime = 0.f;
	UPROPERTY()
	TArray<FGameplayAbilitySpecHandle> WeaponAbilitySpecHandles;
};
