// Copyright © 2020 Spencer Melnick

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
	ATHCharacter();

	
	//Engine overrides
	
	virtual void Tick(float DeltaTime) override;


	
	// Accessors

	// Returns the position of the character's head
	const FVector GetHeadPosition() const;


	
	// Actor properties

	// How quickly the character can rotate
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float CharacterRotationSpeed = 90.f;


	
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

};
