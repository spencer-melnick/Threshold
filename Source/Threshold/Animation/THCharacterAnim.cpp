// Copyright © 2020 Spencer Melnick


#include "THCharacterAnim.h"

#include "Threshold/Character/THCharacter.h"


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

