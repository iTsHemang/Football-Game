// Fill out your copyright notice in the Description page of Project Settings.


#include "TeamManager.h"

#include "Ball.h"
#include "GoalPost.h"
#include "PlayerControllerCpp.h"
#include "Stricker_cpp.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
ATeamManager::ATeamManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ATeamManager::BeginPlay()
{
	Super::BeginPlay();
	GM = Cast<AGM_FirsttoXN>(GetWorld()->GetAuthGameMode());
}

// Called every frame
void ATeamManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

AStricker_cpp* ATeamManager::GetSwitchTarget(AStricker_cpp* Target)
{
	if (!Ball) return nullptr;

	FVector BallLocation = Ball->GetActorLocation();
	AStricker_cpp* SwitchTarget = nullptr;
	float MinDistance = FLT_MAX;

	for (AStricker_cpp* stricker : TeamStrickers)
	{
		if (stricker == Target) continue;

		float BallDist = FVector::DistSquared(stricker->GetActorLocation(), BallLocation);
		if (BallDist < MinDistance)
		{
			MinDistance = BallDist;
			SwitchTarget = stricker;
		}
	}

	return SwitchTarget;
}

AStricker_cpp* ATeamManager::GetPassTarget(AStricker_cpp* CurStricker)
{
	AStricker_cpp* NextStricker = nullptr;
	float BestaScore = -1.0f;

	FVector Forward = CurStricker->GetActorForwardVector();
	FVector CurLocation = CurStricker->GetActorLocation();

	for (AStricker_cpp* stricker : TeamStrickers)
	{
		if (stricker == CurStricker) continue;

		FVector ToMate = (stricker->GetActorLocation() - CurLocation).GetSafeNormal();
		float Dot = FVector::DotProduct(Forward, ToMate);
		float Angle = FMath::RadiansToDegrees(FMath::Acos(Dot));

		if (Angle <= 45)
		{
			float Dist = FVector::Dist(CurLocation, stricker->GetActorLocation());
			float score = (Dot * 1000.0f) - Dist;

			if (score > BestaScore)
			{
				BestaScore = score;
				NextStricker = stricker;
			}
		}
	}
	
	return NextStricker;
}

void ATeamManager::SetControler(APlayerControllerCpp* PlrC)
{
	PlayerController = PlrC;
}

void ATeamManager::GoalConcede()
{
	if (GM)
	{
		GM->GoalScored(this);
	}
}

void ATeamManager::SpawnGoalPost(FTransform SpawnLocation)
{
	GoalPost = GetWorld()->SpawnActor<AGoalPost>(GoalPostSpawn, SpawnLocation.GetLocation(), SpawnLocation.Rotator());
	GoalPost->SetTeamManager(this);
}

void ATeamManager::SpawnStrickers(TArray<FTransform> StrickerSpawnData, AActor* Cam)
{
	for (int i = 0; i < 4; i++)
	{
		STR = GetWorld()->SpawnActor<AStricker_cpp>(StrickerSpawn, StrickerSpawnData[i].GetLocation(), StrickerSpawnData[i].Rotator());
		TeamStrickers.Add(STR);

		if (i == 0 && STR && PlayerController)
		{
			PlayerController->Possess(STR);
			PlayerController->SetViewTargetWithBlend(Cam, 0.0f);
			PlayerController->CamActor = Cam;
		}

		STR = nullptr;
	}
}

void ATeamManager::ResetPosition(TArray<FTransform> StrickerSpawnData, FTransform SpawnLocation, AActor* Cam)
{
	for (int i = 0; i < TeamStrickers.Num(); i++)
	{
		if (i == 0 && IsLossing)
		{
			TeamStrickers[i]->SetActorTransform(SpawnLocation);
		}

		else
		{
			TeamStrickers[i]->SetActorTransform(StrickerSpawnData[i]);
		}

		TeamStrickers[i]->GetCharacterMovement()->StopMovementImmediately();
		TeamStrickers[i]->GetMesh()->GetAnimInstance()->StopAllMontages(0.0f);

		if (i == 0 && PlayerController)
		{
			PlayerController->Possess(TeamStrickers[0]);
			PlayerController->SetViewTargetWithBlend(Cam, 0.0f);
		}
	}
}

