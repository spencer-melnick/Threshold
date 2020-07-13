// Copyright © 2020 Spencer Melnick

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "THCharacterMovement.generated.h"

/**
 * 
 */

UENUM()
enum ETHCustomMovementTypes
{
	CUSTOMMOVE_None,
	CUSTOMMOVE_Dodge,
	CUSTOMMOVE_MAX
};



UCLASS()
class THRESHOLD_API UTHCharacterMovement : public UCharacterMovementComponent
{
	GENERATED_BODY()


public:
	// Engine overrides

	virtual bool IsMoveInputIgnored() const override;
	virtual bool IsMovingOnGround() const override;



	// Accessors
	
	FVector GetDodgeVector() const;
	bool IsDodging() const;
	

	
	
	// Public properties

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Dodge")
	class UCurveFloat* DodgeMovementCurve;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Dodge")
	float DodgeTime = 2.f;

	// The percentage of velocity after triggering a fall.
	// This slows the character when falling to prevent them from
	// being launched off ledges
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Dodge")
	float DodgeVelocityFallFactor = 0.5f;
	

protected:
	// Engine overrides
	
	virtual void OnMovementModeChanged(EMovementMode PreviousMovementMode,
		uint8 PreviousCustomMode) override;
	virtual void PhysCustom(float DeltaTime, int32 Iterations) override;




	// Movement
	
	virtual void PhysDodging(float DeltaTime, int32 Iterations);

private:
	FVector DodgeVector;
	float DodgeTimeElapsed = 0.f;
};
