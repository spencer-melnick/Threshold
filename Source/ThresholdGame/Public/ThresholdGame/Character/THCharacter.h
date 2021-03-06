// Copyright (c) 2020 Spencer Melnick

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ThresholdGame/Combat/Teams.h"
#include "THCharacter.generated.h"



// Forward declare the enum for Weapon Moves
// Probably not necessary as the header is pretty small, but
// better safe than sorry
enum class EWeaponMoveType : int8;



UENUM()
enum class ECharacterLifeState : int8
{
	Alive,
	Dead,
	Despawning
};


/**
 * This is the old base character class. It is now deprecated, but kept for reference while I move some functionality
 * over to the new base character class. DO NOT USE!
 */
UCLASS()
class THRESHOLDGAME_API ATHCharacter : public ACharacter, public ICombatant
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ATHCharacter(const class FObjectInitializer& ObjectInitializer);


	
	//Engine overrides
	
	virtual void Tick(float DeltaTime) override;

	virtual void PostInitializeComponents() override;

	virtual void PossessedBy(AController* NewController) override;

	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent,
		AController* EventInstigator, AActor* DamageCauser) override;



	// Movement

	void Dodge(FVector DodgeVector);



	// Actions

	void PrimaryAttack();



	// Combat

	UFUNCTION(BlueprintCallable, Category="Combat")
	void ChangeActiveWeapon(class UPrimitiveComponent* NewWeapon);

	

	// Animation Responses

	// Should be called by the attack animation blueprint when
	// the attack becomes damaging (e.g. on the start of a
	// sword swing)
	UFUNCTION(BlueprintCallable, Category="AnimationResponse")
	void OnAttackDamageStart();

	// Called when the current attack becomes damaging
	// (e.g. on the start of a sword swing)
	UFUNCTION(BlueprintImplementableEvent, Category="AnimationResponse")
	void OnAttackDamageStartBP();

	// Should be called by the attack animation blueprint when
	// the attack becomes non-damaging and the next combo can
	// chained (e.g. on the end of the swing, but during the
	// "follow-through" of the animation)
	UFUNCTION(BlueprintCallable, Category="AnimationResponse")
	void OnAttackDamageEnd();

	// Called when the current attack becomes non-damaging
	// (e.g. at the end of a sword swing)
	UFUNCTION(BlueprintImplementableEvent, Category="AnimationResponse")
	void OnAttackDamageEndBP();

	// Called at the very end of an active attack animation
	// to let the character know that they are ready to
	// move again
	UFUNCTION(BlueprintCallable, Category="AnimationResponse")
	void OnAttackEnd();
	


	// Accessors

	// Returns the position of the character's head
	FVector GetHeadPosition() const;

	// Returns the character's velocity relative to their
	// heading, and scaled by max movement speed so that
	// a value of 1 indicates walking at max speed in the
	// given direction
	UFUNCTION(BlueprintCallable, Category="Movement")
	FVector2D GetMovementVelocity() const;

	// Returns the dodge direction, usually the current
	// movement direction unless the velocity is
	// sufficiently small, in that case a backwards
	// dodge direction is returned
	UFUNCTION(BlueprintCallable, Category="Movement")
	FVector2D GetDodgeDirection(float Threshold = 0.001f) const;

	UFUNCTION(BlueprintCallable, Category="Movement")
	bool GetIsDodging() const;

	UFUNCTION(BlueprintCallable, Category="Movement")
	bool GetCanDodge() const;

	bool GetCanWalk() const;

	bool GetCanAttack() const;

	class UTHCharacterAnim* GetCharacterAnim() const;

	class UPrimitiveComponent* GetActiveWeapon() const;

	bool GetCharacterIsAlive() const;



	// Interface overrides
	
	virtual TSubclassOf<UTeam> GetTeam() const override;

	virtual bool GetCanBeTargeted() const override;

	virtual bool GetCanBeDamaged() const override;

	virtual FVector GetTargetLocation() const override;

	virtual void AttachTargetIndicator(AActor* ActorToBeAttached) override;


	
	// Actor properties

	// Constants

	static FName DodgeMotionName;
	static FName AbilitySystemComponentName;
	

	// How quickly the character can rotate
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Movement")
	float CharacterRotationSpeed = 90.f;

	// Threshold at which the character is
	// considered not to be moving
	// relative to movement velocity
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Movement")
	float MovementThreshold = 0.01f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Movement")
	float DodgeDistance = 1.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Movement")
	class UCurveFloat* DodgePositionCurve = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Movement")
	float DodgeDuration = 1.f;
	

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Combat")
	class UWeaponMoveset* ActiveWeaponMoveset = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Combat")
	FName DamageableActorTag;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Combat")
	class UCurveFloat* HitSlowdownCurve = nullptr;

	// The maximum amount of time (in seconds) after a hit that the
	// hit slowdown curve will be evaluated at
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Combat")
	float MaxHitSlowdownTime = 1.f;
	
	// The minimum amount of time that a hit slowdown will occur
	// for after hitting an enemy. This ensures that even glancing
	// attacks will apply a hit slowdown for a certain amount of time
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Combat")
	float MinHitSlowdownTime = 0.5f;

	// Names of the slots from where the weapon collision sweep checks
	// will originate
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Combat")
	TArray<FName> WeaponSweepSockets;

	// Radius of the sphere used in weapon sweep
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Combat")
	float WeaponSweepRadius = 10.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Combat")
	TEnumAsByte<ECollisionChannel> WeaponSweepChannel;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Combat")
	float MaxHealth = 100.f;

	// The amount of health that will be given to the character when
	// they are spawned into the world
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Combat")
	float StartingHealth = 100.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Combat")
	TSubclassOf<UTeam> Team;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Combat", meta=(MakeEditWidget))
	FName TargetSocketName;


	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Effects")
	float HitShakeAmplitude = 10.f;


	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Abilities")
	TArray<TSubclassOf<class UTHGameplayAbility>> StartingAbilities;
	
	
	
	// Default components

	// The third person camera spring arm
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	class USpringArmComponent* CameraArm;

	// The third person camera component
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	class UCameraComponent* ThirdPersonCamera;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	class UTHAbilitySystemComponent* AbilitySystemComponent;
	
	

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	// Called whenever a new actor overlaps the weapon
	UFUNCTION()
	virtual void OnAttackingActor(AActor* OtherActor, FHitResult HitResult, FVector HitVelocity);

	// Blueprint implementable call when a new actor
	// overlaps the weapon
	UFUNCTION(BlueprintImplementableEvent, Category="Combat")
	void OnAttackingActorBP(AActor* OtherActor, FHitResult HitResult, FVector HitVelocity);

	virtual void OnDeath();

	UFUNCTION(BlueprintImplementableEvent, Category="Combat")
    void OnDeathBP();

	virtual float CalculateBaseDamage();
	
private:
	// Helper functions

	void PerformNextAttack(EWeaponMoveType MoveType);

	// Clears attack data after an attack (e.g. a single
	// sword swing) is completed
	void ResetAttack();

	void ApplyHitSlowdown(float DeltaTime);

	void PlayScreenShake(FVector ShakeDirection);

	// Check to see if the weapon sweep sockets
	// overlap any damageable actors and trigger the
	// appropriate responses
	void SweepWeaponCollision(float DeltaTime);

	void GrantDefaultAbilities();



	// Delegates

	UFUNCTION()
	void OnWeaponOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnWeaponEndOverlap(UPrimitiveComponent* OverlappedComp,  AActor* OtherActor,
		class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);


	
	// Cached components

	class UTHCharacterMovement* CustomCharacterMovement = nullptr;
	class UTHCharacterAnim* CharacterAnim = nullptr;

	

	
	// Private members

	bool bIsAttacking = false;
	FVector DodgeDirection;
	float CurrentHealth = 0.f;
	ECharacterLifeState LifeState = ECharacterLifeState::Alive;
	bool bWasGrantedStaringAbilities = false;


	// Attack information

	// Actors who were damaged by the active attack.
	// Used to prevent damaging continuously while the
	// attack is active. Should be reset upon new
	// attack.
	UPROPERTY()
	TArray<AActor*> CurrentlyDamagedActors;

	// Actors who are currently overlapping the weapon
	UPROPERTY()
	TArray<AActor*> CurrentlyWeaponOverlappingActors;

	// Last world positions of the sockets in the weapon
	// for weapon sweep. Cleared at the start of each
	// new attack
	TArray<FVector> LastWeaponSweepPositions;

	// Whether the weapon should be attempting to deal
	// damage to actors
	bool bIsAttackDamaging = false;

	// If our attack just finished recently enough to
	// trigger a combo on the next attack press
	bool bCanComboAttack = false;

	// The index of the last move 
	struct FWeaponMove* ActiveWeaponMove = nullptr;
	
	// The currently active weapon component
	UPROPERTY()
	class UPrimitiveComponent* ActiveWeapon;

	float TimeSinceLastHit = 0.f;
};
