﻿// Copyright (c) 2020 Spencer Melnick

#pragma once

#include "CoreMinimal.h"
#include "Threshold/Character/BaseCharacter.h"
#include "PlayerCharacter.generated.h"



// Forward declarations

class USpringArmComponent;
class UCameraComponent;



/**
 * This is the base class for any character that will primarily be controlled by a player, either networked or locally.
 * Note that a character does not need to be derived from this class to be controlled, but this class is preferred
 * except in special cases.
 */
UCLASS()
class THRESHOLD_API APlayerCharacter : public ABaseCharacter
{
	GENERATED_BODY()

public:
	APlayerCharacter(const FObjectInitializer& ObjectInitializer);


	
	// Component name overrides

	static FName SpringArmComponentName;
	static FName ThirdPersonCameraComponentName;

	

	// Components
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USpringArmComponent* SpringArmComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UCameraComponent* ThirdPersonCameraComponent;
};
