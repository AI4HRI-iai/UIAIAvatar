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
	UWorld* World = GetWorld();

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
	
	for (TActorIterator<AIAIAvatarCharacter> It(World); It; ++It)
	{
		AIAIAvatarCharacter* AICharacter =  *It;
		//APawn* Pawn = *It;
		if (AICharacter)
		{
			AAIController* AIController = Cast<AAIController>(AICharacter->GetController());
			if (AIController)
			{
				TriggeringActor = Cast<APawn>(AIController->GetPawn()); //returns NULL , still overlap is generated with AI player
			}
			else
			{
				//UE_LOG(LogAvatarCharacter, Log, TEXT("Actor %s is Player controlled"), *GetName());
				TriggeringActor = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
			}
		}

		InitialRotation = GetOwner()->GetActorRotation();
		TargetRotation = InitialRotation;
		TargetRotation.Yaw += OpenAngle;
		bIsOpening = false;
		CurrentTime = 0.0f;
	}
}

	void UOpenDoor::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
								   UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
								   const FHitResult& SweepResult)
	{
		/*ACharacter* Character = Cast<ACharacter>(OtherActor);
		if (Character)
		{
			USkeletalMeshComponent* SkeletalMesh = Character->GetMesh();
			if (SkeletalMesh && SkeletalMesh->DoesSocketExist(HandBoneName))
			{
				FVector HandLocation = SkeletalMesh->GetSocketLocation(HandBoneName);
				if (TriggerVolume->IsOverlappingComponent(OtherComp) && OtherComp->GetComponentLocation().Equals(HandLocation, 10.0f))
				{
					OpenDoor();
				}
			}
		}*/

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

