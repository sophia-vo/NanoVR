// Fill out your copyright notice in the Description page of Project Settings.


#include "WalkieTalkieActor.h"
#include "Components/InputComponent.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"
#include "Serialization/JsonSerializer.h"


AWalkieTalkieActor::AWalkieTalkieActor()
{
    PrimaryActorTick.bCanEverTick = false;

    // Walkie Talkie Static Mesh Component
    WalkieTalkieMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WalkieTalkieMesh"));
    RootComponent = WalkieTalkieMesh;

    // Sphere Component as detection boundary
    DetectionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("DetectionSphere"));
    DetectionSphere->SetupAttachment(WalkieTalkieMesh);
    DetectionSphere->SetSphereRadius(300.f);  // Adjust radius as needed
    DetectionSphere->SetCollisionProfileName(TEXT("Trigger"));
    DetectionSphere->SetGenerateOverlapEvents(true);

    // Widget Component for "Press E" prompt (3D Widget)
    PressEPromptWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("PressEPromptWidget"));
    PressEPromptWidget->SetupAttachment(WalkieTalkieMesh);
    PressEPromptWidget->SetWidgetSpace(EWidgetSpace::World);
    PressEPromptWidget->SetDrawSize(FVector2D(150, 50));
    PressEPromptWidget->SetRelativeLocation(FVector(0.f, 0.f, 150.f)); 
    PressEPromptWidget->SetVisibility(false); 
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

        // Show the 3D Press E widget when User crosses detection boundary
        PressEPromptWidget->SetVisibility(true);

        // Enable input for player controller and bind E key -> consider changing to non depreciated method
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

        // Hide the 3D widget when User leaves detection boundary
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
        // Check if the chat widget is currently visible on the screen
        if (ChatInputWidget && ChatInputWidget->IsInViewport())
    {
            // The user wants to exit the chat
            HideChatInput();

            // Bring the 3D prompt back
            if (PressEPromptWidget)
            {
                PressEPromptWidget->SetVisibility(true);
            }

            // Reset the state machine in case they walked away mid-quiz
            CurrentChatState = EChatState::Normal; 
        }
        else
        {
            // The user wants to start chatting
            if (PressEPromptWidget)
            {
                PressEPromptWidget->SetVisibility(false);
            }
            
            ShowChatInput();
        }
    }
}


// Text Box Handling 

void AWalkieTalkieActor::ShowChatInput()
{
    if (!ChatInputWidgetClass) 
    {
        UE_LOG(LogTemp, Warning, TEXT("ChatInputWidgetClass not set!"));
        return;
    }

    if (!ChatInputWidget)
    {
        ChatInputWidget = CreateWidget<UUserWidget>(GetWorld(), ChatInputWidgetClass);

        if (!ChatInputWidget)
        {
            UE_LOG(LogTemp, Warning, TEXT("Failed to create ChatInputWidget"));
            return;
        }
    }

    if (!ChatInputWidget->IsInViewport())
    {
        ChatInputWidget->AddToViewport(0);
    }

    if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
    {
        PC->bShowMouseCursor = true;
        // Allow user to click out of textbox and Click E to close text box
        FInputModeGameAndUI InputMode;
        InputMode.SetWidgetToFocus(ChatInputWidget->TakeWidget());
        InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);

        InputMode.SetHideCursorDuringCapture(false);

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

void AWalkieTalkieActor::SubmitChatText(const FString& Text)
{
    HideChatInput();

    // Print what the user typed to the screen in Cyan (temporary check to ensure imputs are getting recieved)
    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Cyan, FString::Printf(TEXT("You: %s"), *Text));
    }

    FString MessageToSend = Text;

    // State Machine Logic -> Consider changing to prompt what mode the User wants
    if (Text.Equals(TEXT("Quiz Me."), ESearchCase::CaseSensitive))
    {
        CurrentChatState = EChatState::RequestedQuiz;
        MessageToSend = TEXT("Ask me a single short quiz question about RNA or COVID-19 vaccines.");
    }
    else if (CurrentChatState == EChatState::WaitingForAnswer)
    {
        // Package the question and the user's answer together so the AI has context
        MessageToSend = FString::Printf(TEXT("You previously asked me this quiz question: '%s'. My answer is: '%s'. Is my answer correct? Please explain the right answer."), *LastQuizQuestion, *Text);
        
        // Reset the state back to normal
        CurrentChatState = EChatState::Normal;
    }
    else
    {
        CurrentChatState = EChatState::Normal;
    }

    // Create the HTTP Request
    FHttpRequestRef Request = FHttpModule::Get().CreateRequest();
    Request->OnProcessRequestComplete().BindUObject(this, &AWalkieTalkieActor::OnChatAPIResponseReceived);
    
    Request->SetURL("http://127.0.0.1:8000/chat"); 
    Request->SetVerb("POST");
    Request->SetHeader("Content-Type", "application/json");

    // Build the JSON Payload using MessageToSend
    TSharedPtr<FJsonObject> JsonObject = MakeShared<FJsonObject>();
    JsonObject->SetStringField("message", MessageToSend); 
    JsonObject->SetNumberField("max_tokens", 256);
    JsonObject->SetNumberField("temperature", 0.7);

    // Serialize to string and send
    FString OutputString;
    TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
    FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);
    
    Request->SetContentAsString(OutputString);
    
    // ... Animation
    OnStartWaitingForBot();
    
    Request->ProcessRequest();
}

void AWalkieTalkieActor::OnChatAPIResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
    if (bWasSuccessful && Response.IsValid())
    {
        TSharedPtr<FJsonObject> JsonResponse;
        TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());

        if (FJsonSerializer::Deserialize(Reader, JsonResponse))
        {
            FString BotResponse = JsonResponse->GetStringField("response");
            
            // Check if this response is the quiz question requested
            if (CurrentChatState == EChatState::RequestedQuiz)
            {
                // Save the question to memory and change state
                LastQuizQuestion = BotResponse;
                CurrentChatState = EChatState::WaitingForAnswer;
            }

            // Print the AI's response in Green -> temporary checking if a response is recieved, will ouput to UI
            if (GEngine)
            {
                GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Green, FString::Printf(TEXT("Bot: %s"), *BotResponse));
            }
            // For ... animation
            OnBotResponseReceived(BotResponse);

            ShowChatInput();
        }
    }
    else
    {
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Error: Could not connect to API."));
            // ensure api server is running locally -> consider hosting with a cloud hosting provider?
        }
        CurrentChatState = EChatState::Normal; // Reset on failure
    }
}