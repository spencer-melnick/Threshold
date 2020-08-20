// Copyright © 2020 Spencer Melnick

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "THAbilitySystemComponent.generated.h"



UCLASS()
class UTHAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	virtual void AbilityLocalInputPressed(int32 InputID) override;
};
