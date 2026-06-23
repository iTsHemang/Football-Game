// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GM_Main.h"
#include "GameFramework/Actor.h"
#include "NavigationSystem.h"
#include "TeamManager.generated.h"

class AStricker_cpp;
class APlayerControllerCpp;
class ABall;
class AGoalPost;

UENUM(BlueprintType)
enum class ETeamState : uint8
{
	Attacking UMETA(DisplayName = "Attacking"),
	Defending UMETA(DisplayName = "Defending"),
	Lose UMETA(DisplayName = "Lose"),
};

UCLASS()
class FOOTBALLGAME_API ATeamManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATeamManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	AGM_Main* GM;
	AGS_Football* GS;

	float CalculateTimer = 0.0f;
	ETeamState NewState = ETeamState::Lose;
	UNavigationSystemV1* NavSys;


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY()
	AGoalPost* GoalPost;
	
	UPROPERTY()
	AStricker_cpp* Goali;

	UPROPERTY()
	AStricker_cpp* PlayerStricker;

	UPROPERTY()
	AActor* CamActor;

	UPROPERTY()
	APlayerControllerCpp* PlayerController;

	UPROPERTY(EditAnywhere, Category="Spawn")
	TSubclassOf<AStricker_cpp> StrickerSpawn;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<AStricker_cpp*> TeamStrickers;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ABall* Ball;

	UPROPERTY(EditAnywhere, Category="Spawn")
	TSubclassOf<AGoalPost> GoalPostSpawn;

	UPROPERTY()
	AActor* OppGoalPost;

	UPROPERTY()
	ATeamManager* OppTM;

	UPROPERTY()
	ETeamState TeamState;
	
	AStricker_cpp* GetSwitchTarget(AStricker_cpp* Target);
	
	AStricker_cpp* GetPassTarget(AStricker_cpp* CurStricker, FVector StickDirection);

	void GetOppGoalPost();

	bool IsLossing = false;

	FVector FutureBallPos;
	
	void SetControler(APlayerControllerCpp* PlrC);
	void SpawnGoalPost(FTransform SpawnLocation);
	void SpawnStrickers(TArray<FTransform> StrickerSpawnData);
	void PosessForward();
	void GoalConcede();
	void ResetPosition(TArray<FTransform> StrickerSpawnData, FTransform SpawnLocation);
	bool HasPossassion();
	void AutoSwitch(AStricker_cpp* Str);
	
	void ManageAIStricker();
	TArray<FVector>  GenerateSpots(FVector BallPos);
	float SpotScore(FVector S, AStricker_cpp* Stricker, TArray<FVector> AsignedSpots);
	void GoaliSpot(FVector BallPos);
	void MoveAi(AStricker_cpp* Stricker, FVector TargetPos);
};
