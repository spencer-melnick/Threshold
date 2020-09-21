// Copyright (c) 2020 Spencer Melnick

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "UObject/WeakInterfacePtr.h"
#include "InteractionSubsystem.generated.h"


// Forward declarations

class IInteractiveObject;


/**
 * Subsystem to track interactive actors
 */
UCLASS()
class UInteractionSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	void RegisterObject(IInteractiveObject* Object);
	void UnregisterObject(IInteractiveObject* Object);
	
	const TArray<TWeakInterfacePtr<IInteractiveObject>>& GetObjects() const
	{
		return InteractiveObjects;
	};


private:
	TArray<TWeakInterfacePtr<IInteractiveObject>> InteractiveObjects;
};
