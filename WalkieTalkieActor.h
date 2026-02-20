// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
 
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "Blueprint/UserWidget.h"
#include "WalkieTalkieActor.generated.h"



UCLASS()
class NANOTESTER3D_API AWalkieTalkieActor : public AActor
{
	GENERATED_BODY()
	
public:
    AWalkieTalkieActor();

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
    TSubclassOf<UUserWidget> ChatInputWidgetClass;
 
protected:
    virtual void BeginPlay() override;
 
    // Components
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
   UStaticMeshComponent* WalkieTalkieMesh;
 
   UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
   USphereComponent* DetectionSphere;
 
   UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
   UWidgetComponent* PressEPromptWidget;
 
    // Overlap Functions
    UFUNCTION()
    void OnDetectionBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                 UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
                                 bool bFromSweep, const FHitResult& SweepResult);
 
    UFUNCTION()
    void OnDetectionEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                               UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
 
    // E key input handler
    void OnPressE();
 
    // State
    bool bPlayerInRange;

    // User Widget Text Box --------------------------------
    // UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
    // TSubclassOf<UUserWidget> ChatInputWidgetClass;
    
    UPROPERTY()
    UUserWidget* ChatInputWidget;
    
    // Function to handle submitted chat text
    UFUNCTION()
    void OnChatTextSubmitted(const FString& Text);
    
    // Show/hide chat input UI
    void ShowChatInput();
    void HideChatInput();
};
