// Copyright (c) 2020 Spencer Melnick


#include "ThresholdGame/Global/THGameInstance.h"
#include "ThresholdGame/Global/THConfig.h"


// Engine overrides

void UTHGameInstance::Init()
{
    Super::Init();

    UserConfig = NewObject<UTHConfig>();
}

void UTHGameInstance::Shutdown()
{
    UserConfig->SaveConfig();
}




// Accessors

UTHConfig* UTHGameInstance::GetTHConfig() const
{
    return UserConfig;
}

