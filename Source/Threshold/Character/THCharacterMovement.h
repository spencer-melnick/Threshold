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

	virtual FVector ConsumeInputVector() override;



	// Accessors
	
	FVector GetDodgeVector() const;
	

	
	
	// Public properties
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Dodge")
	float DodgeSpeed = 1000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Dodge")
	float DodgeAcceleration = 4096.f;
	

protected:
	// Engine overrides
	
	virtual void OnMovementModeChanged(EMovementMode PreviousMovementMode,
		uint8 PreviousCustomMode) override;
	virtual void PhysCustom(float DeltaTime, int32 Iterations) override;
	virtual float GetMaxAcceleration() const override;
	virtual float GetMaxSpeed() const override;


private:
	FVector DodgeVector;
};
