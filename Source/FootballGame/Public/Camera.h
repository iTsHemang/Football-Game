// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Camera.generated.h"

class ABall;

UCLASS()
class FOOTBALLGAME_API ACamera : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACamera();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ABall* Ball;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Camera Folow Data")
	FVector Offset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Camera Folow Data")
	float FolowSpeed;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void MoveCamera(float DeltaTime);
	void RotateCam();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	FVector CurrentLoc;

};
