// Copyright (c) 2020 Spencer Melnick

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AbilityInputTypes.h"
#include "THAbilitySystemComponent.generated.h"



UCLASS()
class THRESHOLDGAME_API UTHAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	UTHAbilitySystemComponent();


	
	// Engine overrides

	virtual bool GetShouldTick() const override;
	virtual void AbilityLocalInputPressed(int32 InputID) override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
		FActorComponentTickFunction* ThisTickFunction) override;



	// Input buffering functions

	template <typename T>
	TWeakPtr<T> GetPendingAbilityInput() const
	{
		if (!MostRecentInputData.IsValid() || MostRecentInputData->GetScriptStruct() != T::StaticStruct())
		{
			// Validate that our data is the correct type
			return nullptr;
		}

		return StaticCastSharedPtr<T>(MostRecentInputData);
	};
	


	// Local gameplay cue functions
	
	UFUNCTION(BlueprintCallable, Category="GameplayCue",
		Meta=(AutoCreateRefTerm="GameplayCueParameters", GameplayTagFilter="GameplayCue"))
	void ExecuteGameplayCueLocal(const FGameplayTag GameplayCueTag,
		const FGameplayCueParameters& GameplayCueParameters);

	UFUNCTION(BlueprintCallable, Category="GameplayCue",
		Meta=(AutoCreateRefTerm="GameplayCueParameters", GameplayTagFilter="GameplayCue"))
    void AddGameplayCueLocal(const FGameplayTag GameplayCueTag,
    	const FGameplayCueParameters& GameplayCueParameters);

	UFUNCTION(BlueprintCallable, Category="GameplayCue",
		Meta=(AutoCreateRefTerm="GameplayCueParameters", GameplayTagFilter="GameplayCue"))
    void RemoveGameplayCueLocal(const FGameplayTag GameplayCueTag,
    	const FGameplayCueParameters& GameplayCueParameters);



	// Constants

	static const int32 MaxInputBufferSize;


	
	// Public properties

	// If this is true, abilities with input buffering enabled will be added to the input queue if they cannot be
	// activated yet.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Input")
	bool bEnableInputBuffering = false;

	// How long in seconds that buffered inputs will be stored
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Input", meta=(EditCondition="bEnableInputBuffering"))
	float InputBufferingTime = 0.5f;


	
private:
	// Internal struct
	struct FBufferedInput
	{
		int32 InputID;
		float InputTime;
		TMap<FGameplayAbilitySpecHandle, TSharedPtr<FBufferedAbilityInputData>> Data;
	};


	// Input buffer helpers

	void BufferInput(FBufferedInput&& Input);
	void RemoveFrontInput();
	void DispatchInputEvents(FGameplayAbilitySpec& Spec);
	
	

	TSharedPtr<FBufferedAbilityInputData> MostRecentInputData;
	TQueue<FBufferedInput, EQueueMode::Spsc> InputBuffer;
	int32 CurrentInputBufferSize = 0;
};
