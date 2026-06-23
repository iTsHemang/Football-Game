// Fill out your copyright notice in the Description page of Project Settings.


#include "GM_Main.h"

#include <string>

#include "Ball.h"
#include "Camera.h"
#include "PlayerControllerCpp.h"
#include "TeamManager.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/GameplayStatics.h"

AGM_Main::AGM_Main()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AGM_Main::BeginPlay()
{
	Super::BeginPlay();

	GS = Cast<AGS_Football>(GetWorld()->GetGameState());
	GS->IsGameOn = false;
	GS->ScoreA = 0;
	GS->ScoreB = 0;

	Ball = GetWorld()->SpawnActor<ABall>(BallSpawn, BallSpawnLoc, FRotator::ZeroRotator);

	Camera = GetWorld()->SpawnActor<ACamera>(CameraSpawn, CamSpawnLoc, FRotator::ZeroRotator);
	Camera->Ball = Ball;
	
	TeamManagerA = GetWorld()->SpawnActor<ATeamManager>(TeamManagerSpawn);
	TeamManagerB = GetWorld()->SpawnActor<ATeamManager>(TeamManagerSpawn);
	
	TeamManagerA->Ball = Ball;
	TeamManagerB->Ball = Ball;

	TeamManagerA->SpawnGoalPost(GoalPostLeftLoc);
	TeamManagerB->SpawnGoalPost(GoalPostRightLoc);
	
	TeamManagerA->CamActor = Camera;
	TeamManagerB->CamActor = Camera;

	TeamManagerA->OppTM = TeamManagerB;
	TeamManagerB->OppTM = TeamManagerA;

	
	TeamManagerA->SpawnStrickers(PlayersLeftLoc);
	TeamManagerB->SpawnStrickers(PlayersRightLoc);
	
    UGameplayStatics::CreatePlayer(GetWorld(), 1, true);

	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		APlayerControllerCpp* PC = Cast<APlayerControllerCpp>(Iterator->Get());
		
		if (PC)
		{
			if (IsPlayerTeamA)
			{
				if (PC->PlayerState && PC->GetLocalPlayer()->GetControllerId() == 0)
				{
					UE_LOG(LogTemp, Error, TEXT("%s is connected to %s"), *PC->GetName(), *TeamManagerA->GetName());
					PC->SetTeamManager(TeamManagerA);
				}
				else
				{
					UE_LOG(LogTemp, Error, TEXT("%s is connected to %s"), *PC->GetName(), *TeamManagerB->GetName());
					PC->SetTeamManager(TeamManagerB);
				}
			}

			else
			{
				if (PC->PlayerState && PC->GetLocalPlayer()->GetControllerId() == 0)
				{
					PC->SetTeamManager(TeamManagerB);
				}
				
				else
				{
					PC->SetTeamManager(TeamManagerA);
				}
			}
		}
	}

	FTimerHandle Timer;
	GetWorldTimerManager().SetTimer(Timer, [this]()
	{
		for (int i = 0; i < 2; i++)
		{
			if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, i))
			{
				PC->SetViewTargetWithBlend(Camera, 0.0f);
			}
		}
	}, 0.05f, false);

	FTimerHandle T;
	GetWorldTimerManager().SetTimer(T, this, &AGM_Main::StartTimer, 1.0f, true, 0.0f);
}

void AGM_Main::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	SetTeamState();
}


void AGM_Main::Restart()	
{
	Ball->SetActorLocation(BallSpawnLoc);
	Ball->BallMesh->SetPhysicsLinearVelocity(FVector::ZeroVector);

	TeamManagerA->ResetPosition(PlayersLeftLoc, StrickerLeftLoc);
	TeamManagerB->ResetPosition(PlayersRightLoc, StrickerRightLoc);

	StartingT = 3;
	GS->IsGameOn = false;
	
	FTimerHandle T;
    GetWorldTimerManager().SetTimer(T, this, &AGM_Main::StartTimer, 1.0f, true, 0.0f);
}

void AGM_Main::StartTimer()
{
	GEngine->AddOnScreenDebugMessage(2, 20.0f, FColor::Red, FString::FromInt(StartingT));
	
	if (StartingT != 0) StartingT --;
	else GS->IsGameOn = true;
}

void AGM_Main::GoalScored(ATeamManager* ConcedingTeam)
{
	if (TeamManagerA == ConcedingTeam)
	{
		GS->ScoreB ++;
	}

	else if (TeamManagerB == ConcedingTeam)
	{
		GS->ScoreA ++;
	}

	if (GS->ScoreA > GS->ScoreB)
	{
		TeamManagerB->IsLossing = true;
		TeamManagerA->IsLossing = false;
	}
	else if (GS->ScoreA < GS->ScoreB)
	{
		TeamManagerA->IsLossing = true;
		TeamManagerB->IsLossing = false;
	}

	else
	{
		TeamManagerA->IsLossing = false;
		TeamManagerB->IsLossing = false;
	}

	FString score = FString::FromInt(GS->ScoreA) + ":" + FString::FromInt(GS->ScoreB);
	GEngine->AddOnScreenDebugMessage(1, 20.0f, FColor::Green, score);

	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &AGM_Main::Restart, 2.0f, false);
}

void AGM_Main::SetTeamState()
{
	if (TeamManagerA->HasPossassion() && !TeamManagerB->HasPossassion())
	{
		TeamManagerA->TeamState = ETeamState::Attacking;
		TeamManagerB->TeamState = ETeamState::Defending;
	}

	else if (!TeamManagerA->HasPossassion() && TeamManagerB->HasPossassion())
	{
		TeamManagerA->TeamState = ETeamState::Defending;
		TeamManagerB->TeamState = ETeamState::Attacking;
	}

	else if (!TeamManagerA->HasPossassion() && !TeamManagerB->HasPossassion())
	{
		TeamManagerA->TeamState = ETeamState::Lose;
		TeamManagerB->TeamState = ETeamState::Lose;
	}
}
