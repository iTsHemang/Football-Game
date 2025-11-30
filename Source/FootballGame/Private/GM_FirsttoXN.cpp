// Fill out your copyright notice in the Description page of Project Settings.


#include "GM_FirsttoXN.h"

#include <string>

#include "Ball.h"
#include "Camera.h"
#include "PlayerControllerCpp.h"
#include "TeamManager.h"
#include "GameFramework/PlayerState.h"


void AGM_FirsttoXN::BeginPlay()
{
	Super::BeginPlay();

	Ball = GetWorld()->SpawnActor<ABall>(BallSpawn, BallSpawnLoc, FRotator::ZeroRotator);

	Camera = GetWorld()->SpawnActor<ACamera>(CameraSpawn, CamSpawnLoc, FRotator::ZeroRotator);
	Camera->Ball = Ball;
	
	TeamManagerA = GetWorld()->SpawnActor<ATeamManager>(TeamManagerSpawn);
	TeamManagerB = GetWorld()->SpawnActor<ATeamManager>(TeamManagerSpawn);

	TeamManagerA->Ball = Ball;
	TeamManagerB->Ball = Ball;


	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		APlayerControllerCpp* PC = Cast<APlayerControllerCpp>(Iterator->Get());
		if (PC)
		{
			
			if (IsPlayerTeamA)
			{
				if (PC->PlayerState && PC->PlayerState->GetPlayerId() == 0)
				{
					PC->SetTeamManager(TeamManagerA);
				}
				else
				{
					PC->SetTeamManager(TeamManagerB);
				}
			}

			else
			{
				if (PC->PlayerState && PC->PlayerState->GetPlayerId() == 0)
				{
					PC->SetTeamManager(TeamManagerB);
				}
				{
					PC->SetTeamManager(TeamManagerA);
				}
			}
		}
	}
	
	TeamManagerA->SpawnGoalPost(GoalPostLeftLoc);
	TeamManagerB->SpawnGoalPost(GoalPostRightLoc);

	TeamManagerA->SpawnStrickers(PlayersLeftLoc, Camera);
	TeamManagerB->SpawnStrickers(PlayersRightLoc, Camera);
}

void AGM_FirsttoXN::Restart()
{
	Ball->SetActorLocation(BallSpawnLoc);
	Ball->BallMesh->SetPhysicsLinearVelocity(FVector::ZeroVector);

	TeamManagerA->ResetPosition(PlayersLeftLoc, StrickerLeftLoc, Camera);
	TeamManagerB->ResetPosition(PlayersRightLoc, StrickerRightLoc, Camera);
}

void AGM_FirsttoXN::GoalScored(ATeamManager* ConcedingTeam)
{
	if (TeamManagerA == ConcedingTeam)
	{
		ScoreB ++;
	}

	else if (TeamManagerB == ConcedingTeam)
	{
		ScoreA ++;
	}

	if (ScoreA > ScoreB)
	{
		TeamManagerB->IsLossing = true;
		TeamManagerA->IsLossing = false;
	}
	else if (ScoreA < ScoreB)
	{
		TeamManagerA->IsLossing = true;
		TeamManagerB->IsLossing = false;
	}

	else
	{
		TeamManagerA->IsLossing = false;
		TeamManagerB->IsLossing = false;
	}

	FString score = FString::FromInt(ScoreA) + ":" + FString::FromInt(ScoreB);
	GEngine->AddOnScreenDebugMessage(1, 20.0f, FColor::Green, score);

	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &AGM_FirsttoXN::Restart, 2.0f, false);
}
