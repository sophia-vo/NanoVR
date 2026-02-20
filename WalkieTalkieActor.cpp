// Fill out your copyright notice in the Description page of Project Settings.


#include "WalkieTalkieActor.h"
#include "Components/InputComponent.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"


AWalkieTalkieActor::AWalkieTalkieActor()
{
    PrimaryActorTick.bCanEverTick = false;

    // Create and setup Static Mesh component
    WalkieTalkieMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WalkieTalkieMesh"));
    RootComponent = WalkieTalkieMesh;

    // Create and configure Sphere Component as detection boundary
    DetectionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("DetectionSphere"));
    DetectionSphere->SetupAttachment(WalkieTalkieMesh);
    DetectionSphere->SetSphereRadius(300.f);  // Adjust radius as needed
    DetectionSphere->SetCollisionProfileName(TEXT("Trigger"));
    DetectionSphere->SetGenerateOverlapEvents(true);

    // Create Widget Component for "Press E" prompt (3D Widget)
    PressEPromptWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("PressEPromptWidget"));
    PressEPromptWidget->SetupAttachment(WalkieTalkieMesh);
    PressEPromptWidget->SetWidgetSpace(EWidgetSpace::World);
    PressEPromptWidget->SetDrawSize(FVector2D(150, 50));
    PressEPromptWidget->SetRelativeLocation(FVector(0.f, 0.f, 150.f)); // above the walkie talkie mesh
    PressEPromptWidget->SetVisibility(false); // Hidden initially

    bPlayerInRange = false;
}

void AWalkieTalkieActor::BeginPlay()
{
    Super::BeginPlay();

    DetectionSphere->OnComponentBeginOverlap.AddDynamic(this, &AWalkieTalkieActor::OnDetectionBeginOverlap);
    DetectionSphere->OnComponentEndOverlap.AddDynamic(this, &AWalkieTalkieActor::OnDetectionEndOverlap);

}

void AWalkieTalkieActor::OnDetectionBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (OtherActor && OtherActor == UGameplayStatics::GetPlayerPawn(this, 0))
    {
        bPlayerInRange = true;

        // Show the 3D Press E widget
        PressEPromptWidget->SetVisibility(true);

        // Enable input for player controller and bind E key
        APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
        if (PC)
        {
            EnableInput(PC);
            if (InputComponent)
            {
                InputComponent->BindAction("Interact", IE_Pressed, this, &AWalkieTalkieActor::OnPressE);
            }
        }
    }
}

void AWalkieTalkieActor::OnDetectionEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    if (OtherActor && OtherActor == UGameplayStatics::GetPlayerPawn(this, 0))
    {
        bPlayerInRange = false;

        // Hide the 3D widget
        PressEPromptWidget->SetVisibility(false);

        APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
    
        if (PC)
        {
            DisableInput(PC);
        }
        
    }
}

void AWalkieTalkieActor::OnPressE()
{
    if (bPlayerInRange)
    {
        // Hide 3D widget prompt
        PressEPromptWidget->SetVisibility(false);

        ShowChatInput();
        
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(
                -1,                 // Key (-1 = new message every time)
                5.f,                // Duration (seconds)
                FColor::Green,      // Text color
                TEXT("E pressed: Show chat input UI")
            );
        }

        // TODO: Show 2D chat input UI (widget on left screen) here via your UUserWidget class
    
        UE_LOG(LogTemp, Log, TEXT("E pressed: Show chat input UI"));
    }
}


// Text Box Handling --------------------------

void AWalkieTalkieActor::ShowChatInput()
{
    if (!ChatInputWidgetClass) 
    {
        UE_LOG(LogTemp, Warning, TEXT("ChatInputWidgetClass not set!"));
        return;
    }

    if (!ChatInputWidget)
    {
        ChatInputWidget = CreateWidget<UUserWidget>(
            GetWorld(),
            ChatInputWidgetClass
        );

        if (!ChatInputWidget)
        {
            UE_LOG(LogTemp, Warning, TEXT("Failed to create ChatInputWidget"));
            return;
        }
    }

    ChatInputWidget->AddToViewport(0);

    if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
    {
        PC->bShowMouseCursor = true;

        FInputModeUIOnly InputMode;
        InputMode.SetWidgetToFocus(ChatInputWidget->TakeWidget());
        InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);

        PC->SetInputMode(InputMode);
    }
}

void AWalkieTalkieActor::HideChatInput()
{
    if (ChatInputWidget && ChatInputWidget->IsInViewport())
    {
        ChatInputWidget->RemoveFromParent();
    }

    if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
    {
        PC->bShowMouseCursor = false;
        PC->SetInputMode(FInputModeGameOnly());
    }
}

void AWalkieTalkieActor::OnChatTextSubmitted(const FString& Text)
{
    HideChatInput();

    UE_LOG(LogTemp, Log, TEXT("Chat text submitted: %s"), *Text);

    // API call or further processing here
}