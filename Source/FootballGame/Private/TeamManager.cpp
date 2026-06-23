// Fill out your copyright notice in the Description page of Project Settings.


#include "TeamManager.h"

#include <string>

#include "Ball.h"
#include "GoalPost.h"
#include "PlayerControllerCpp.h"
#include "StrickerAIController.h"
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
	GM = Cast<AGM_Main>(GetWorld()->GetAuthGameMode());
	GS = Cast<AGS_Football>(GetWorld()->GetGameState());
	NavSys = UNavigationSystemV1::GetCurrent(GetWorld());
}

// Called every frame
void ATeamManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (GS->IsGameOn)
	{
		if (TeamState != NewState)
		{
			NewState = TeamState;
			ManageAIStricker();
			CalculateTimer = 0.0f;
			return;
		}
			
		CalculateTimer += DeltaTime;
		if (CalculateTimer >= 0.25f)
		{
			CalculateTimer = 0.0f;
			ManageAIStricker();
		}

		if (Goali != nullptr && PlayerStricker == Goali)
		{
			float GolliDistance = FVector::Distance(GoalPost->GetActorLocation(), Goali->GetActorLocation());
			if (GolliDistance > 700.0f) Goali = nullptr;
		}
	}
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

	PlayerStricker = SwitchTarget;
	return SwitchTarget;
}

AStricker_cpp* ATeamManager::GetPassTarget(AStricker_cpp* CurStricker,FVector StickDirection)
{
	AStricker_cpp* NextStricker = nullptr;
	float BestaScore = -1.0f;

	FVector Forward = CurStricker->GetActorForwardVector();
	FVector CurLocation = CurStricker->GetActorLocation();

	for (AStricker_cpp* stricker : TeamStrickers)
	{
		if (stricker == CurStricker) continue;

		FVector ToMate = (stricker->GetActorLocation() - CurLocation).GetSafeNormal();
		float Dot = FVector::DotProduct(StickDirection, ToMate);
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

void ATeamManager::GetOppGoalPost()
{
	TArray<AActor*> AllGoalPosts;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AGoalPost::StaticClass(), AllGoalPosts);

	for (AActor* CurGoalPost : AllGoalPosts)
	{
		if (CurGoalPost != GoalPost)
        {
            OppGoalPost = CurGoalPost;
            break;
        }
	}
}

void ATeamManager::SetControler(APlayerControllerCpp* PlrC)
{
	PlayerController = PlrC;

	PosessForward();
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

	GetWorldTimerManager().SetTimerForNextTick(this, &ATeamManager::GetOppGoalPost);
}

void ATeamManager::SpawnStrickers(TArray<FTransform> StrickerSpawnData)
{
	AStricker_cpp* STR;
	
	for (int i = 0; i < 5; i++)
	{
		STR = GetWorld()->SpawnActor<AStricker_cpp>(StrickerSpawn, StrickerSpawnData[i].GetLocation(), StrickerSpawnData[i].Rotator());
		STR->Team = this;
		STR->Ball = Ball;
		TeamStrickers.Add(STR);

		STR = nullptr;
	}
}

void ATeamManager::PosessForward()
{
	AStricker_cpp* Stricker = TeamStrickers[0];
	
	if (Stricker && PlayerController)
	{
		PlayerStricker = Stricker;
		PlayerController->Possess(Stricker);
		PlayerController->SetViewTargetWithBlend(CamActor, 0.0f);
		PlayerController->CamActor = CamActor;
	}
}

void ATeamManager::ResetPosition(TArray<FTransform> StrickerSpawnData, FTransform SpawnLocation)
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
			PlayerController->SetViewTargetWithBlend(CamActor, 0.0f);
		}
	}
}

bool ATeamManager::HasPossassion()
{
	return Ball && Ball->ControllingTeam == this;
}

void ATeamManager::AutoSwitch(AStricker_cpp* Str)
{
	PlayerStricker->SpawnDefaultController();
	PlayerController->Possess(Str);
	PlayerController->SetViewTargetWithBlend(CamActor, 0.0f);
	PlayerStricker = Str;
}

void ATeamManager::ManageAIStricker()
{
	FVector BallPos = Ball->GetActorLocation() + Ball->GetVelocity() * 0.3f;
	
	if (TeamState == ETeamState::Defending || TeamState == ETeamState::Lose)
	{
		if (Goali == nullptr)
		{
			float MinGoalDistance = 999.0f;
			AStricker_cpp* TempGoli = nullptr;
			for (AStricker_cpp* Stricker : TeamStrickers)
			{
				if (Stricker == PlayerStricker) continue;

				float GoalDistance = FVector::Distance(GoalPost->GetActorLocation(), Stricker->GetActorLocation());

				if (GoalDistance < MinGoalDistance)
				{
					MinGoalDistance = GoalDistance;
					TempGoli = Stricker;	
				}
			}
			Goali = TempGoli;
		}

		GoaliSpot(BallPos);
	}

	else if (TeamState == ETeamState::Attacking) Goali = nullptr;
	
	TArray<FVector> Spots = GenerateSpots(BallPos);

	TArray<FVector> AssignedSpots;
	for (AStricker_cpp* Stricker : TeamStrickers)
	{
		if (Stricker == PlayerStricker) continue;
		if (Stricker == Goali) continue;

		FVector BestSpot = Spots[0];
		float BestScore = -999.0f;

		for (FVector Spot : Spots)
		{
			float Score = SpotScore(Spot, Stricker, AssignedSpots);
			if (Score > BestScore)
			{
				BestScore = Score;
				BestSpot = Spot;
			}
		}

		AssignedSpots.Add(BestSpot);

		FVector Jitter = FVector(FMath::RandRange(-60.0f, 60.0f), FMath::RandRange(-60.0f, 60.0f), 0.0f);

		//if (TeamState == ETeamState::Attacking)DrawDebugSphere(GetWorld(), BestSpot+Jitter, 20, 12, FColor::Red, false, 2.0f);
		//else if (TeamState == ETeamState::Defending)DrawDebugSphere(GetWorld(), BestSpot+Jitter, 20, 12, FColor::Green, false, 2.0f);
		//else if (TeamState == ETeamState::Lose)DrawDebugSphere(GetWorld(), BestSpot+Jitter, 20, 12, FColor::Yellow, false, 2.0f);
		MoveAi(Stricker, BestSpot+Jitter);
	}
}

TArray<FVector> ATeamManager::GenerateSpots(FVector BallPos)
{
	FVector Center;

	float Dot = FVector::DotProduct(GetActorForwardVector(), GoalPost->GetActorForwardVector());
	int AttackDirection = FMath::Cos(FMath::Acos(Dot));

	switch (TeamState)
	{
		case ETeamState::Attacking:Center = FVector(PlayerStricker->GetActorLocation().X, PlayerStricker->GetActorLocation().Y, 0.0f);break;
		case ETeamState::Defending:Center = FVector(GoalPost->GetActorLocation().X, GoalPost->GetActorLocation().Y - (AttackDirection * 400.0f), 0.0f) ;break;
		case ETeamState::Lose:Center = BallPos;break;
	}

	TArray<FVector> Spots;
	float Radius[3]={280, 560, 980};
	int SCount[3]={8,8,8};

	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < SCount[i]; j++)
		{
			float Angle = (360.0f / SCount[i] * j);
			float Rad = FMath::DegreesToRadians(Angle);
			FVector Spot = Center + FVector(FMath::Cos(Rad), FMath::Sin(Rad), 0.0f) * Radius[i];

			if (Spot.X < -800.0f || Spot.X > 800.0f || Spot.Y < -1200.0f || Spot.Y > 1200.0f) continue;

			FNavLocation NavLoc;
			if (NavSys->ProjectPointToNavigation(Spot, NavLoc, FVector(50.0f, 50.0f, 50.0f)))
			{
				Spots.Add(NavLoc.Location);
			}
		}
	}
	
	return Spots;
}

float ATeamManager::SpotScore(FVector S, AStricker_cpp* Stricker, TArray<FVector> AsignedSpots)
{
	if (Ball == nullptr || OppGoalPost == nullptr) return 0;
	
	FVector BallPos = Ball->GetActorLocation();
	FVector AttackDir = (OppGoalPost->GetActorLocation() - GoalPost->GetActorLocation()).GetSafeNormal();
	
	FVector Center = (GoalPost->GetActorLocation() + OppGoalPost->GetActorLocation()) * 0.5f;

	float LateralDist = FMath::Abs(S.X - Center.X);
	float PitchHalf = 400.0f;
	float T1 = FMath::Clamp(LateralDist / PitchHalf, 0.0f, 1.0f);

	float Score = T1 * 1.0f;

	float MinSpace = 120.0f;
	for (FVector Taken : AsignedSpots)
	{
		if (FVector::Dist(S, Taken) < MinSpace) return -99.0f;
	}

	float MaxDistance = 500.0f;
	float SpotToStr = FVector::Dist(S, Stricker->GetActorLocation());
	if (SpotToStr > MaxDistance) return -99.0f;

	/*switch (TeamState)
	{
		case ETeamState::Attacking:
			{
				FVector PlayerToSpot = (S - PlayerStricker->GetActorLocation()).GetSafeNormal();
				float T3 = FMath::Clamp(FVector::DotProduct(PlayerToSpot, AttackDir), 0.0f, 1.0f);

				float GoalDist = FVector::Dist(S, OppGoalPost->GetActorLocation());
				float T4 = FMath::Clamp(GoalDist / 1400, 0.0f, 1.0f);

				bool IsClear = !GetWorld()->LineTraceTestByChannel(PlayerStricker->GetActorLocation(), S, ECC_Pawn);
				float T5 = IsClear? 1.0f : 0.0f;

				Score += T3 * 0.8f + T4 * 0.7f + T5 * 0.6f;
				break;
			}
		case ETeamState::Defending:
			{
				FVector BallToGoal = (GoalPost->GetActorLocation() - BallPos).GetSafeNormal();
				FVector GoalToSpot = (S - GoalPost->GetActorLocation()).GetSafeNormal();
				float T3 = FMath::Clamp(FVector::DotProduct(GoalToSpot, BallToGoal), 0.0f, 1.0f);

				float MinDistance = 5000.0f;
				for (AStricker_cpp* OppStr : OppTM->TeamStrickers)
				{
					FVector Closet = FMath::ClosestPointOnSegment(S, BallPos, OppGoalPost->GetActorLocation());
					MinDistance = FMath::Min(MinDistance, FVector::Dist(S,Closet));
				}
				float T4 = 1.0f - FMath::Clamp(MinDistance/400.0f, 0.0f, 1.0f);

				float BallDist = FVector::Dist(S, BallPos);
				float T5 =1.0f - FMath::Abs(BallDist - 300.0f)/300.0f;
				T5 = FMath::Clamp(T5, 0.0f, 1.0f);

				Score += T3 * 1.0f + T4 * 0.9f + T5 * 0.5f;
				break;
			}
		case ETeamState::Lose:
			{
				bool IsClear = !GetWorld()->LineTraceTestByChannel(BallPos, S, ECC_Pawn);
				float T3 = IsClear? 1.0f : 0.0f;

				float BallDist = FVector::Dist(S, BallPos);
				if (BallDist < 300.0f) return -99.0f;
				float T4 = FMath::Clamp(BallDist/2800.0f, 0.0f, 1.0f);

				FVector Midline = FVector(0.0f, S.Y, S.Z);
				float DistMid = FVector::Dist(S, Midline);
				float T5 = 1.0f -  FMath::Clamp(DistMid / 1400.0f, 0.0f, 1.0f);

				Score += T3 * 0.5f + T4 * 0.9f + T5 * 0.7f;
				break;
			}
	}*/

	return Score;
}

void ATeamManager::GoaliSpot(FVector BallPos)
{
	if (Goali == PlayerStricker || Goali == nullptr) return;
	
	FVector GoaliPos;

	float Dot = FVector::DotProduct(GetActorForwardVector(), GoalPost->GetActorForwardVector());
	int AttackDirection = FMath::Cos(FMath::Acos(Dot));
	
	FVector GoalLocation = GoalPost->GetActorLocation();
	float GoalPostWidth = 250.0f;
	float PitchLength = 2800.0f;
	float BalltoGoalDist = FVector::Distance(BallPos, GoalLocation);

	float A = FMath::Clamp(1.0f - (BalltoGoalDist / PitchLength), 0.0f, 1.0f);
	float Radius = FMath::Lerp(20.0f, 200.0f, A);

	FVector GoaltoBall = (BallPos - GoalLocation).GetSafeNormal();

	GoaliPos = GoalLocation + GoaltoBall * Radius;
	GoaliPos.X = FMath::Clamp(BallPos.X, GoalLocation.X - (GoalPostWidth*0.5f + 60.0f), GoalLocation.X + (GoalPostWidth*0.5f + 60.0f));
	
	if (AttackDirection < 0) GoaliPos.Y = FMath::Clamp(GoaliPos.Y, GoalLocation.Y - (AttackDirection * 60.0f), GoalLocation.Y - (AttackDirection * 400.0f));
	else GoaliPos.Y = FMath::Clamp(GoaliPos.Y, GoalLocation.Y - (AttackDirection * 400.0f), GoalLocation.Y - (AttackDirection * 60.0f));
	
	GoaliPos += FVector(FMath::FRandRange(-10.0f, 10.0f),FMath::FRandRange(-10.0f, 10.0f),0.0f);

	MoveAi(Goali,GoaliPos);
}

void ATeamManager::MoveAi(AStricker_cpp* Stricker, FVector TargetPos)
{
	AStrickerAIController* AIController = Cast<AStrickerAIController>(Stricker->GetController());
	if (AIController)
	{
		AIController->MoveAI(TargetPos);
	}
}

