// Copyright © 2020 Spencer Melnick

#include "MaterialDissolveComponent.h"

#include "Components/MeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"


UMaterialDissolveComponent::UMaterialDissolveComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
}



// Engine overrides

void UMaterialDissolveComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    if (!bIsDissolving)
    {
        return;
    }

    DissolveAlpha += DeltaTime / DissolveTime;

    if (DissolveAlpha >= 1.f)
    {
        DissolveAlpha = 1.f;
        bIsDissolving = false;
        SetComponentTickEnabled(false);
    }

    for (UMaterialInstanceDynamic* MaterialInstanceDynamic : DissolveMaterialsDynamic)
    {
        MaterialInstanceDynamic->SetScalarParameterValue(MaterialParameter, DissolveAlpha);
    }
}




// Custom functions

void UMaterialDissolveComponent::StartDissolve()
{
    if (bIsDissolving)
    {
        UE_LOG(LogTemp, Warning, TEXT("Dissolve Component %s is already dissolving, cannot trigger another dissolve"),
            *GetNameSafe(this));
        return;
    }

    // Keep track of state to prevent firing twice
    bIsDissolving = true;
    SetComponentTickEnabled(true);

    // Do some sanity checks first
    UMeshComponent* MeshComponent = Cast<UMeshComponent>(TargetComponent.GetComponent(GetOwner()));

    if (MeshComponent == nullptr)
    {
        UE_LOG(LogTemp, Error, TEXT("Dissolve Component %s does not have a valid target MeshComponent"),
            *GetNameSafe(this));
    }

    // Create our dynamic materials
    DissolveMaterialsDynamic.Empty();
    DissolveMaterialsDynamic.Reserve(DissolveMaterials.Num());

    for (int32 i = 0; i < DissolveMaterials.Num(); i++)
    {
        UMaterialInterface* MaterialInterface = DissolveMaterials[i];
        UMaterialInstanceDynamic* MaterialInstanceDynamic = UMaterialInstanceDynamic::Create(MaterialInterface, this);

        // Quick check to ensure that the material is actually created successfully
        if (MaterialInstanceDynamic == nullptr)
        {
            // We should hopefully never reach this, but it can't hurt to be safe
            UE_LOG(LogTemp, Error, TEXT("Failed to create dynamic instance of material %s"),
                *GetNameSafe(MaterialInterface));
            continue;
        }

        // Actually apply and track the material
        MeshComponent->SetMaterial(i, MaterialInstanceDynamic);
        DissolveMaterialsDynamic.Add(MaterialInstanceDynamic);
    }
}


