// Copyright (c) 2020 Spencer Melnick

#pragma once



#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Teams.generated.h"

UCLASS(Abstract)
class THRESHOLDGAME_API UTeam : public UObject
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
class THRESHOLDGAME_API UCombatant : public UInterface
{
    GENERATED_BODY()
};

class ICombatant
{
    GENERATED_BODY()

public:
    virtual TSubclassOf<UTeam> GetTeam() const = 0;

    // Returns whether or not this team member can be targeted at all - 
    // Functions as a master override
    virtual bool GetCanBeTargeted() const;

    // Returns whether or not this team member can be damaged at all - 
    // Functions as a master override
    virtual bool GetCanBeDamaged() const;
    
    virtual bool GetCanBeTargetedBy(TSubclassOf<UTeam> OtherTeam) const;
    virtual bool GetCanBeDamagedBy(TSubclassOf<UTeam> OtherTeam) const;

    // Returns the location to be targeted. This should be the same
    // as a potential target indicator attachment point and can
    // be used to track this team member
    virtual FVector GetTargetLocation() const = 0;

    // Attaches a target indicator actor to this team member
    virtual void AttachTargetIndicator(AActor* TargetIndicator) = 0;
};
