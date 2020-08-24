// Copyright (c) 2020 Spencer Melnick

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MaterialDissolveComponent.generated.h"




UCLASS(Category="Effects", meta=(BlueprintSpawnableComponent))
class THRESHOLD_API UMaterialDissolveComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UMaterialDissolveComponent();



    // Engine overrides

    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
    


    // Custom functions
    
    UFUNCTION(BlueprintCallable)
    void StartDissolve();

    

    // Public properties
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Effect")
    TArray<TSoftObjectPtr<UMaterialInterface>> DissolveMaterials;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Effect")
    FName MaterialParameter;

    UPROPERTY(EditAnywhere, Category="Effect")
    FComponentReference TargetComponent;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Effect")
    float DissolveTime = 1.f;

private:
    // Materials to replace in the target mesh component. They need to have the same indices as the materials
    // in the target component that you are replacing
    UPROPERTY()
    TArray<class UMaterialInstanceDynamic*> DissolveMaterialsDynamic;

    float DissolveAlpha = 0.f;

    bool bIsDissolving = false;
};

