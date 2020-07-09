// Copyright © 2020 Spencer Melnick


#include "THCharacterAnim.h"

#include "Components/SkeletalMeshComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Components/CapsuleComponent.h"

#include "DrawDebugHelpers.h"

#include "Threshold/Character/THCharacter.h"


// Engine overrides

void UTHCharacterAnim::NativeInitializeAnimation()
{
	// Cache properties
	APawn* PawnOwner = TryGetPawnOwner();

	if (PawnOwner == nullptr)
	{
		return;
	}
	
	CharacterOwner = Cast<ATHCharacter>(PawnOwner);

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

	float DesiredLeftFootOffset = GetSocketAdjustment(LeftFootIKSocketName);
	float DesiredRightFootOffset = GetSocketAdjustment(RightFootIKSocketName);

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
	SkeletalMesh->SetRelativeLocation(BaseMeshOffset + FVector::UpVector * BodyOffset);
}






// Accessors

FVector UTHCharacterAnim::GetLeftFootEffector() const
{
	if (SkeletalMesh == nullptr || LeftFootIKSocketName.IsNone())
	{
		return FVector::ZeroVector;
	}

	const USkeletalMeshSocket* Socket = SkeletalMesh->GetSocketByName(LeftFootIKSocketName);

	if (Socket == nullptr)
	{
		return FVector::ZeroVector;
	}

	FVector SocketLocation = Socket->GetSocketLocation(SkeletalMesh);
	return SocketLocation + FVector::UpVector * LeftFootOffset;
}

FVector UTHCharacterAnim::GetRightFootEffector() const
{
	if (SkeletalMesh == nullptr || RightFootIKSocketName.IsNone())
	{
		return FVector::ZeroVector;
	}

	const USkeletalMeshSocket* Socket = SkeletalMesh->GetSocketByName(RightFootIKSocketName);

	if (Socket == nullptr)
	{
		return FVector::ZeroVector;
	}

	FVector SocketLocation = Socket->GetSocketLocation(SkeletalMesh);
	return SocketLocation + FVector::UpVector * RightFootOffset;
}









// Deprecated functions!

void UTHCharacterAnim::EndDodge()
{
	APawn* Pawn = TryGetPawnOwner();

	if (Pawn == nullptr)
	{
		return;
	}

	ATHCharacter* Character = Cast<ATHCharacter>(Pawn);

	if (Character == nullptr)
	{
		return;
	}

	Character->EndDodge();
}





// Helper functions

float UTHCharacterAnim::GetSocketAdjustment(FName SocketName)
{
	if (SkeletalMesh == nullptr || CharacterOwner == nullptr || SocketName.IsNone())
	{
		return 0.f;
	}

	const USkeletalMeshSocket* Socket = SkeletalMesh->GetSocketByName(SocketName);
	
	if (Socket == nullptr)
	{
		return 0.f;
	}

	// Find the socket's location projected onto the ground plane as a center for our offset
	FVector SocketLocation = Socket->GetSocketLocation(SkeletalMesh);
	FVector GroundLocation = CharacterOwner->GetActorLocation() +
		FVector::DownVector * CharacterOwner->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	FVector SocketGroundLocation = FVector(SocketLocation.X, SocketLocation.Y, GroundLocation.Z);

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

