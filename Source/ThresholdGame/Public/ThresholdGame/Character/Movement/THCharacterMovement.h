// Copyright (c) 2020 Spencer Melnick

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "THCharacterMovement.generated.h"

/**
 * 
 */


UCLASS()
class THRESHOLDGAME_API UTHCharacterMovement : public UCharacterMovementComponent
{
	GENERATED_BODY()

public:
	virtual FRotator GetDeltaRotation(float DeltaTime) const override;



	// If true, the rate of rotation is proportional to the character movement speed divided by the max walk speed
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Rotation")
	bool bRotationProportionalToSpeed = true;

	// If false, the character will not rotate at all when in the air
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Rotation")
	bool bShouldRotateInAir = false;
};
