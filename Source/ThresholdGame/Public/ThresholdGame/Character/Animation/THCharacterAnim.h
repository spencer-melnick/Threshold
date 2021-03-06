// Copyright (c) 2020 Spencer Melnick

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"

#include <atomic>

#include "THCharacterAnim.generated.h"

/**
 * 
 */
UCLASS()
class THRESHOLDGAME_API UTHCharacterAnim : public UAnimInstance
{
	GENERATED_BODY()

public:
	// Engine overrides

	virtual void NativeInitializeAnimation() override;

	virtual void NativeUpdateAnimation(float DeltaSeconds) override;



	

	// Custom functions
	
	UFUNCTION(BlueprintImplementableEvent, Category="Character")
	void Attack(class UAnimSequenceBase* AttackAnimation);
	
	UFUNCTION(BlueprintImplementableEvent, Category="Character")
	void ReactToHit(FVector2D HitVector);



	

	// Accessors

	// Returns the desired location of the foot IK bone after applying
	// the raycasted adjustment
	UFUNCTION(BlueprintPure, Category="Foot Control", meta=(AnimGetter="True"))
	FVector GetLeftFootOffset(int32 AssetPlayerIndex) const
	{
		return FVector::UpVector * LeftFootOffset;
	}

	// Returns the desired location of the foot IK bone after applying
	// the raycasted adjustment
	UFUNCTION(BlueprintPure, Category="Foot Control", meta=(AnimGetter="True"))
	FVector GetRightFootOffset(int32 AssetPlayerIndex) const
	{
		return FVector::UpVector * RightFootOffset;
	}



	
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
	FName LeftFootIKBoneName;

	UPROPERTY(EditAnywhere, Category="Foot Control")
	FName RightFootIKBoneName;

	UPROPERTY(EditAnywhere, Category="Foot Control")
	float FootAdjustSpeed = 200.f;

	UPROPERTY(EditAnywhere, Category="Foot Control")
	float BodyAdjustSpeed = 200.f;

private:
	// Helper functions

	// Returns the required vertical adjustment for the
	// given IK bone using raycasting
	float GetBoneAdjustment(FName BoneName);

	
	

	// Cached values
	
	FVector BaseMeshOffset;
	class ABaseCharacter* CharacterOwner = nullptr;
	class USkeletalMeshComponent* SkeletalMesh = nullptr;


	
	// Interpolated foot values
	
	std::atomic<float> LeftFootOffset = 0.f;
	std::atomic<float> RightFootOffset = 0.f;
	std::atomic<float> BodyOffset = 0.f;
};
