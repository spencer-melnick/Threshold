// Copyright (c) 2020 Spencer Melnick


#include "ThresholdGame/Character/Animation/THCharacterAnim.h"

#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "ThresholdGame/Character/BaseCharacter.h"


// Engine overrides

void UTHCharacterAnim::NativeInitializeAnimation()
{
	// Cache properties
	APawn* PawnOwner = TryGetPawnOwner();

	if (PawnOwner == nullptr)
	{
		return;
	}
	
	CharacterOwner = Cast<ABaseCharacter>(PawnOwner);

	if (CharacterOwner == nullptr)
	{
		return;
	}

	SkeletalMesh = CharacterOwner->GetMesh();

	if (SkeletalMesh == nullptr)
	{
		return;
	}

	BaseMeshOffset = SkeletalMesh->GetRelativeLocation();
}

void UTHCharacterAnim::NativeUpdateAnimation(float DeltaSeconds)
{
	if (SkeletalMesh == nullptr)
	{
		return;
	}

	float DesiredLeftFootOffset = GetBoneAdjustment(LeftFootIKBoneName);
	float DesiredRightFootOffset = GetBoneAdjustment(RightFootIKBoneName);

	// Body should be adjusted so that the most extended leg doesn't need be extended
	// further downward than it's original position
	float DesiredBodyOffset = FMath::Min(DesiredLeftFootOffset, DesiredRightFootOffset);

	DesiredLeftFootOffset -= DesiredBodyOffset;
	DesiredRightFootOffset -= DesiredBodyOffset;

	// Lerp towards desired offsets
	LeftFootOffset = FMath::FInterpConstantTo(LeftFootOffset,
		DesiredLeftFootOffset, DeltaSeconds, FootAdjustSpeed);
	RightFootOffset = FMath::FInterpConstantTo(RightFootOffset,
		DesiredRightFootOffset, DeltaSeconds, FootAdjustSpeed);
	BodyOffset = FMath::FInterpConstantTo(BodyOffset, DesiredBodyOffset,
		DeltaSeconds, BodyAdjustSpeed);

	// Adjust mesh location, feet positions will need to be adjusted by animation
	// blueprint
	UWorld* World = GetWorld();
	if (World != nullptr && GetWorld()->IsGameWorld())
	{
		SkeletalMesh->SetRelativeLocation(BaseMeshOffset + FVector::UpVector * BodyOffset);
	}
}




// Helper functions

float UTHCharacterAnim::GetBoneAdjustment(FName BoneName)
{
	if (SkeletalMesh == nullptr || CharacterOwner == nullptr || BoneName.IsNone())
	{
		return 0.f;
	}

	// Find the target's location projected onto the ground plane as a center for our offset
	FVector BoneLocation = SkeletalMesh->GetBoneLocation(BoneName);
	FVector GroundLocation = CharacterOwner->GetActorLocation() +
		FVector::DownVector * CharacterOwner->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	FVector SocketGroundLocation = FVector(BoneLocation.X, BoneLocation.Y, GroundLocation.Z);

	FVector TraceStart = SocketGroundLocation + FVector::UpVector * MaxFootLift;
	FVector TraceEnd = SocketGroundLocation + FVector::DownVector * MaxFootDrop;

	FHitResult HitResult;
	FCollisionQueryParams CollisionParams = FCollisionQueryParams::DefaultQueryParam;
	CollisionParams.AddIgnoredActor(CharacterOwner);

	if (!GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd,
		FootTraceChannel, CollisionParams))
	{
		// No hit! Return the maximum distance
		return -MaxFootDrop;
	}

	// Return the distance required to move our ground position to the impact
	return HitResult.ImpactPoint.Z - SocketGroundLocation.Z;
}

