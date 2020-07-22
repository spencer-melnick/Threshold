// Copyright � 2020 Spencer Melnick

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "THCharacter.generated.h"

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

	// Triggered by animation blueprint to stop motion
	void EndDodge();
	


	
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
	// Cached components

	class UTHCharacterMovement* CustomCharacterMovement = nullptr;
	

	
	// Private members
	
	FVector2D DodgeDirection;
};
