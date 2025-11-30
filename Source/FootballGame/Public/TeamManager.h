// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GM_FirsttoXN.h"
#include "GameFramework/Actor.h"
#include "TeamManager.generated.h"

class AStricker_cpp;
class APlayerControllerCpp;
class ABall;
class AGoalPost;

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

	AGM_FirsttoXN* GM;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY()
	AGoalPost* GoalPost;

	UPROPERTY()
	AStricker_cpp* STR;

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

	AStricker_cpp* GetSwitchTarget(AStricker_cpp* Target);
	
	AStricker_cpp* GetPassTarget(AStricker_cpp* CurStricker);

	bool IsLossing = false;

	void SetControler(APlayerControllerCpp* PlrC);

	void GoalConcede();
	void SpawnGoalPost(FTransform SpawnLocation);
	void SpawnStrickers(TArray<FTransform> StrickerSpawnData, AActor* Cam);
	
	void ResetPosition(TArray<FTransform> StrickerSpawnData, FTransform SpawnLocation, AActor* Cam);

};
