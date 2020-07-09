// Copyright © 2020 Spencer Melnick

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "THCharacterAnim.generated.h"

/**
 * 
 */
UCLASS()
class THRESHOLD_API UTHCharacterAnim : public UAnimInstance
{
	GENERATED_BODY()

public:
	// Engine overrides

	virtual void NativeInitializeAnimation() override;

	virtual void NativeUpdateAnimation(float DeltaSeconds) override;



	

	// Custom functions (deprecated!)
	
	UFUNCTION(BlueprintImplementableEvent, Category="Character")
	void Dodge();

	UFUNCTION(BlueprintCallable, Category="Character")
	void EndDodge();

	UFUNCTION(BlueprintImplementableEvent, Category="Character")
	void CheckIsDodging(bool& bIsDodging);



	

	// Accessors

	// Returns the desired location of the foot IK bone after applying
	// the raycasted adjustment
	UFUNCTION(BlueprintCallable, Category="Foot Control")
	FVector GetLeftFootEffector() const;

	// Returns the desired location of the foot IK bone after applying
	// the raycasted adjustment
	UFUNCTION(BlueprintCallable, Category="Foot Control")
	FVector GetRightFootEffector() const;



	
	// Public properties

	// How high above the default position the foot can be raised due
	// to IK adjustments
	UPROPERTY(EditAnywhere, Category="Foot Control")
	float MaxFootLift = 20.f;

	// How far below the default position the character can be lowered due
	// to IK adjustments
	UPROPERTY(EditAnywhere, Category="Foot Control")
	float MaxFootDrop = 20.f;

	// The collision channel to trace against for foot IK adjustments
	UPROPERTY(EditAnywhere, Category="Foot Control")
	TEnumAsByte<ECollisionChannel> FootTraceChannel;

	UPROPERTY(EditAnywhere, Category="Foot Control")
	FName LeftFootIKSocketName;

	UPROPERTY(EditAnywhere, Category="Foot Control")
	FName RightFootIKSocketName;

	UPROPERTY(EditAnywhere, Category="Foot Control")
	float FootAdjustSpeed = 200.f;

	UPROPERTY(EditAnywhere, Category="Foot Control")
	float BodyAdjustSpeed = 200.f;

private:
	// Helper functions

	// Returns the required vertical adjustment for the
	// given socket using raycasting
	float GetSocketAdjustment(FName SocketName);

	
	

	// Cached values
	
	FVector BaseMeshOffset;
	class ATHCharacter* CharacterOwner = nullptr;
	class USkeletalMeshComponent* SkeletalMesh = nullptr;


	
	// Interpolated foot values
	
	float LeftFootOffset = 0.f;
	float RightFootOffset = 0.f;
	float BodyOffset = 0.f;
};
