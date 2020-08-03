// Copyright � 2020 Spencer Melnick

#pragma once



#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Teams.generated.h"

UCLASS(Blueprintable)
class THRESHOLD_API UTeam : public UObject
{
    GENERATED_BODY()

public:
    UTeam();
    
    virtual bool GetCanTargetTeam(TSubclassOf<UTeam> OtherTeam) const;
    virtual bool GetCanDamageTeam(TSubclassOf<UTeam> OtherTeam) const;

protected:
    UPROPERTY(EditDefaultsOnly)
    TArray<TSubclassOf<UTeam>> TargetableTeams;

    UPROPERTY(EditDefaultsOnly)
    TArray<TSubclassOf<UTeam>> DamageableTeams;
};


UINTERFACE()
class UTeamMember : public UInterface
{
    GENERATED_BODY()
};

class ITeamMember
{
    GENERATED_BODY()

public:
    virtual TSubclassOf<UTeam> GetTeam() const = 0;
    virtual bool GetCanBeTargetedBy(TSubclassOf<UTeam> OtherTeam) const;
    virtual bool GetCanBeDamagedBy(TSubclassOf<UTeam> OtherTeam) const;
};
