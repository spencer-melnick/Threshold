// Copyright (c) 2020 Spencer Melnick

#include "CoreMinimal.h"

// Console variable to allow tag extension

static TAutoConsoleVariable<float> CVarServerTagExtensionTime(
    TEXT("th.TagExtensionTime"),
    0.5f,
    TEXT("Sets how much time (in seconds) GameplayTags with the ExtendTagOnServer setting\n")
    TEXT("will be extended for, used to support longer combo windows on the server given client latency"),
    ECVF_SetByProjectSetting | ECVF_SetByConsoleVariablesIni);

static TAutoConsoleVariable<float> CVarServerTagLookForwardTime(
    TEXT("th.TagLookForwardTime"),
    0.5f,
    TEXT("Sets how much time (in seconds) to look forward for gameplay tags in an animation\n")
    TEXT("This essentially increase the window on the server for combos driven by animation"),
    ECVF_SetByProjectSetting | ECVF_SetByConsoleVariablesIni);
