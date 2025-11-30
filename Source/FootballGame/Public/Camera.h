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
	FVector Offset = FVector(-800, 0, 600);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Camera Folow Data")
	float FolowSpeed = 3.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Camera Folow Data")
	float PanSpeed = 0.2f;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	FVector CurrentLoc;

};
