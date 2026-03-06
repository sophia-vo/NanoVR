// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
 
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "Blueprint/UserWidget.h"

#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"

#include "WalkieTalkieActor.generated.h"

UENUM()
enum class EChatState : uint8
{
    Normal,
    RequestedQuiz,
    WaitingForAnswer
};

UCLASS()
class NANOTESTER3D_API AWalkieTalkieActor : public AActor
{
	GENERATED_BODY()
	
public:
    AWalkieTalkieActor();

    UPROPERTY(EditAnywhere, Category = "UI")
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
    
    UPROPERTY()
    UUserWidget* ChatInputWidget;
    
    void ShowChatInput();

    UFUNCTION(BlueprintCallable, Category = "Walkie Talkie")
    void HideChatInput();

    UFUNCTION(BlueprintCallable, Category = "Walkie Talkie")
    void SubmitChatText(const FString& Text);

    void OnChatAPIResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

    EChatState CurrentChatState = EChatState::Normal;
    FString LastQuizQuestion;
};
