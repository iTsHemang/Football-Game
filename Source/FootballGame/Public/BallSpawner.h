// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Ball.h"
#include "GameFramework/Actor.h"
#include "BallSpawner.generated.h"

UCLASS()
class FOOTBALLGAME_API ABallSpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABallSpawner();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BallSpawn")
	TSubclassOf<ABall> BallToSpawn;

	UFUNCTION(BlueprintCallable, Category = "BallSpawn")
	void SpawnBall();
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
