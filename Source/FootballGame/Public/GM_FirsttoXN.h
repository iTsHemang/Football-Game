// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GM_FirsttoXN.generated.h"

class ABall;
class ATeamManager;
class ACamera;

/**
 * 
 */
UCLASS()
class FOOTBALLGAME_API AGM_FirsttoXN : public AGameModeBase
{
	GENERATED_BODY()

public:

	ABall* Ball;
	ACamera* Camera;
	ATeamManager* TeamManagerA;
	ATeamManager* TeamManagerB;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn")
	TSubclassOf<ABall> BallSpawn;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn")
	TSubclassOf<ATeamManager> TeamManagerSpawn;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn")
	TSubclassOf<ACamera> CameraSpawn;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Location")
    FVector BallSpawnLoc;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Location")
	FVector CamSpawnLoc;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Location")
    FTransform StrickerLeftLoc;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Location")
    FTransform StrickerRightLoc;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Location")
    FTransform GoalPostLeftLoc;
        
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Location")
    FTransform GoalPostRightLoc;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Location")
	FTransform GKLeftLoc;
    
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Location")
	FTransform GKRightLoc;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Location")
	TArray<FTransform> PlayersLeftLoc;
    
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Location")
	TArray<FTransform> PlayersRightLoc;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerManage")
	bool IsPlayerTeamA;

	UPROPERTY(BlueprintReadOnly, Category="Score")
	int ScoreA;

	UPROPERTY(BlueprintReadOnly, Category="Score")
	int ScoreB;

	void GoalScored(ATeamManager* ConcedingTeam);


protected:
	virtual void BeginPlay() override;
	
	void Restart();
	
};
