// Copyright (c) 2020 Spencer Melnick

#pragma once

#include "CoreMinimal.h"
#include "Components/Widget.h"
#include "ThresholdUI/Widgets/SLiveRender.h"
#include "LiveRender.generated.h"



// Delegates

DECLARE_DYNAMIC_DELEGATE_OneParam(FOnVisibilityChangedEvent, bool, bIsVisible);
DECLARE_DYNAMIC_DELEGATE(FOnSizeChangedEvent);



/**
 * UMG wrapper for a slate widget that holds and displays an unscaled render target
 */
UCLASS()
class THRESHOLDUI_API ULiveRender : public UWidget
{
	GENERATED_BODY()

public:

	// Editor properties
	
	/** Color and opacity */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Appearance, meta=( sRGB="true") )
	FLinearColor ColorAndOpacity = FLinearColor::White;

	/** A bindable delegate for the ColorAndOpacity. */
	UPROPERTY()
	FGetLinearColor ColorAndOpacityDelegate;

	/** Flips the image if the localization's flow direction is RightToLeft */
	UPROPERTY(EditAnywhere, Category=Localization)
	bool bFlipForRightToLeftFlowDirection;

	/** Default requested size of the render target */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Render)
	FIntPoint DefaultTargetSize = FIntPoint(128, 128);

	/** Name of material parameter to be assigned to render target value */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Render)
	FName TextureParameterName = NAME_None;

	/** Material used as base for rendering render target */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Render)
	UMaterialInterface* ParentMaterial;



	// Delegates
	
	UPROPERTY(EditAnywhere, Category=Events, meta=( IsBindableEvent="True" ))
	FOnVisibilityChangedEvent OnVisibilityChangedEvent;

	UPROPERTY(EditAnywhere, Category=Events, meta=( IsBindableEvent="True" ))
	FOnSizeChangedEvent OnSizeChangedEvent;



	// Accessors

	UFUNCTION(BlueprintCallable, Category="Appearance")
    void SetColorAndOpacity(FLinearColor InColorAndOpacity);

	UFUNCTION(BlueprintCallable, Category="Appearance")
    void SetOpacity(float InOpacity);

	UFUNCTION(BlueprintCallable, Category="Render")
	void SetDefaultTargetSize(FIntPoint InDefaultTargetSize);

	UFUNCTION(BlueprintCallable, Category="Render")
	void SetTextureParameterName(FName InTextureParameterName);

	UFUNCTION(BlueprintCallable, Category="Render")
	void SetParentMaterial(UMaterialInterface* InParentMaterial);

	UFUNCTION(BlueprintPure, Category="Render")
	UMaterialInstanceDynamic* GetMaterialInstance() const;

	UFUNCTION(BlueprintPure, Category="Render")
	UTextureRenderTarget2D* GetRenderTarget() const;



	// Widget overrides

	virtual void SynchronizeProperties() override;
	virtual void ReleaseSlateResources(bool bReleaseChildren) override;


protected:

	virtual TSharedRef<SWidget> RebuildWidget() override;



	// Delegates

	void OnVisibilityChanged(bool bIsVisible);
	void OnSizeChanged();
	
	
	
	// Widget storage
	
	TSharedPtr<SLiveRender> LiveRenderWidget;
};
