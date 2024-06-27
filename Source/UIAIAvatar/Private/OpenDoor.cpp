// Fill out your copyright notice in the Description page of Project Settings.


#include "OpenDoor.h"
#include "GameFramework/Actor.h"
#include "Engine/World.h"
#include "Components/PrimitiveComponent.h"
#include "Components/BoxComponent.h"
#include "Engine/Engine.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "IAIAvatarCharacter.h" //check for player controller
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Controller.h"
#include "AIController.h"
#include "GameFramework/Pawn.h"

// Sets default values for this component's properties
UOpenDoor::UOpenDoor()
{
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UOpenDoor::BeginPlay()
{
	Super::BeginPlay();

	 USceneComponent* RootComponent = GetOwner()->GetRootComponent();
	
	 if (RootComponent)
	 {
	 	// Attach the static mesh component to the owner actor's root component
	 	TriggerVolume->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepWorldTransform);  
	 }

	if (TriggerVolume)
	{
		TriggerVolume->OnComponentBeginOverlap.AddDynamic(this, &UOpenDoor::OnOverlapBegin);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("%s has no trigger volume"), *GetOwner()->GetName());
	}

	// Initialize TriggeringActor to NULL
	TriggeringActor = nullptr;

	// Iterate through all AI characters in the world and find the first one
	for (TActorIterator<AIAIAvatarCharacter> It(GetWorld()); It; ++It)
	{
		//AAIController* AIController = Cast<AAIController>(AICharacter->GetController());
		//if (AIController){
		//TriggeringActor = Cast<APawn>(AIController->GetPawn());}
		
		AIAIAvatarCharacter* AICharacter = *It;
		if (AICharacter)
		{
			
			// If an AI character is found, set TriggeringActor and break out of the loop
			TriggeringActor = AICharacter;
			break;
		}
	}

	// If TriggeringActor is still NULL, set it to the player pawn
	if (!TriggeringActor)
	{
		TriggeringActor = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	}

	// Initialize other variables
	InitialRotation = GetOwner()->GetActorRotation();
	TargetRotation = InitialRotation;
	TargetRotation.Yaw += OpenAngle;
	bIsOpening = false;
	CurrentTime = 0.0f;
}

void UOpenDoor::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
							   UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
							   const FHitResult& SweepResult)
{
	if (OtherActor == TriggeringActor)
	{
		OpenDoor();
	}
}

void UOpenDoor::OpenDoor()
{
	bIsOpening = true;
	CurrentTime = 0.0f;
}

void UOpenDoor::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bIsOpening)
	{
		CurrentTime += DeltaTime;
		float Progress = FMath::Clamp(CurrentTime / OpenDuration, 0.0f, 1.0f);
		FRotator NewRotation = FMath::Lerp(InitialRotation, TargetRotation, Progress);
		GetOwner()->SetActorRotation(NewRotation);

		if (Progress >= 1.0f)
		{
			bIsOpening = false;
		}
	}
}