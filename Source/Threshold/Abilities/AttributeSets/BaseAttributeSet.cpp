// Copyright (c) 2020 Spencer Melnick

#include "BaseAttributeSet.h"
#include "GameplayEffect.h"
#include "GameplayEffectExtension.h"
#include "Net/UnrealNetwork.h"


// UBaseAttributeSet

// Attribute set overrides

void UBaseAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	if (Attribute == GetMaxHealthAttribute())
	{
		// Limit health by the new max health
		LimitAttributeOnMaxChange(Health, GetHealthAttribute(), NewValue);
	}
}

void UBaseAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	if (Data.EvaluatedData.Attribute == GetDamageAttribute())
	{
		// Applying damage

		// Copy the damage meta attribute and clear it
		float LocalDamage = GetDamage();
		SetDamage(0.f);

		// Apply defense to damage
		LocalDamage = FMath::Max(LocalDamage - GetDefense(), 0.f);
		
		// More damage logic?

		// Apply the damage as a health change
		const float NewHealth = FMath::Clamp(GetHealth() - LocalDamage, 0.f, GetMaxHealth());
		SetHealth(NewHealth);
	}
	else if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		// Applying healing
		// Clamp health values
		SetHealth(FMath::Clamp(GetHealth(), 0.f, GetMaxHealth()));
	}
}



// Network replication

void UBaseAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME_CONDITION_NOTIFY(UBaseAttributeSet, Health, COND_None, REPNOTIFY_Always)
	DOREPLIFETIME_CONDITION_NOTIFY(UBaseAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always)
	DOREPLIFETIME_CONDITION_NOTIFY(UBaseAttributeSet, Defense, COND_None, REPNOTIFY_Always)
}

void UBaseAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UBaseAttributeSet, Health, OldHealth)
}

void UBaseAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UBaseAttributeSet, MaxHealth, OldMaxHealth)
}

void UBaseAttributeSet::OnRep_Defense(const FGameplayAttributeData& OldDefense)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UBaseAttributeSet, Defense, OldDefense)
}




// Helper functions

void UBaseAttributeSet::LimitAttributeOnMaxChange(FGameplayAttributeData& AttributeData, FGameplayAttribute AttributeProperty, float NewMax) const
{
	UAbilitySystemComponent* AbilitySystemComponent = GetOwningAbilitySystemComponent();

	if (!AbilitySystemComponent)
	{
		return;
	}
	
	const float CurrentValue = AttributeData.GetCurrentValue();
	
	if (CurrentValue > NewMax)
	{
		// Subtract from our current value so it's limited by the new max
		const float DeltaValue = CurrentValue - NewMax;
		AbilitySystemComponent->ApplyModToAttributeUnsafe(AttributeProperty, EGameplayModOp::Additive, DeltaValue);
	}
}


