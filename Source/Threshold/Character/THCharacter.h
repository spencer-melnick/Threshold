// Copyright � 2020 Spencer Melnick

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "THCharacter.generated.h"

// Forward declare the enum for Weapon Moves
// Probably not necessary as the header is pretty small, but
// better safe than sorry
enum class EWeaponMoveType : int8;

UCLASS()
class THRESHOLD_API ATHCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ATHCharacter(const class FObjectInitializer& ObjectInitializer);

	
	//Engine overrides
	
	virtual void Tick(float DeltaTime) override;

	virtual void PostInitializeComponents() override;



	// Movement

	void Dodge();



	// Actions

	void PrimaryAttack();

	


	// Animation Responses

	// Should be called by the attack animation blueprint when
	// the attack becomes damaging (e.g. on the start of a
	// sword swing)
	UFUNCTION(BlueprintCallable, Category="AnimationResponse")
	void OnAttackDamageStart();

	// Should be called by the attack animation blueprint when
	// the attack becomes non-damaging and the next combo can
	// chained (e.g. on the end of the swing, but during the
	// "follow-through" of the animation)
	UFUNCTION(BlueprintCallable, Category="AnimationResponse")
	void OnAttackDamageEnd();

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


	
	// Actor properties

	// How quickly the character can rotate
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Movement")
	float CharacterRotationSpeed = 90.f;

	// Threshold at which the character is
	// considered not to be moving
	// relative to movement velocity
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Movement")
	float MovementThreshold = 0.01f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Combat")
	class UWeaponMoveset* ActiveWeaponMoveset = nullptr;
	

	
	// Default components

	// The third person camera spring arm
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	class USpringArmComponent* CameraArm;

	// The third person camera component
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	class UCameraComponent* ThirdPersonCamera;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	// Helper functions

	void PerformNextAttack(EWeaponMoveType MoveType);

	// Clears attack data after an attack (e.g. a single
	// sword swing) is completed
	void ResetAttack();


	
	// Cached components

	class UTHCharacterMovement* CustomCharacterMovement = nullptr;
	class UTHCharacterAnim* CharacterAnim = nullptr;
	

	
	// Private members

	bool bIsAttacking = false;
	FVector2D DodgeDirection;


	// Attack information

	// Actors who were damaged by the active attack.
	// Used to prevent damaging continuously while the
	// attack is active. Should be reset upon new
	// attack.
	TArray<AActor*> CurrentlyDamagedActors;

	// Whether the weapon should be attempting to deal
	// damage to actors
	bool bIsAttackDamaging = false;

	// If our attack just finished recently enough to
	// trigger a combo on the next attack press
	bool bCanComboAttack = false;

	// The index of the last move 
	struct FWeaponMove* ActiveWeaponMove = nullptr;
};
