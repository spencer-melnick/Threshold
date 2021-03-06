// Copyright (c) 2020 Spencer Melnick

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "THGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class THRESHOLDGAME_API UTHGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
    // Engine overrides

    virtual void Init() override;

    virtual void Shutdown() override;

    
    
    // Accessors

    class UTHConfig* GetTHConfig() const;

private:
    UPROPERTY()
    class UTHConfig* UserConfig;
};
