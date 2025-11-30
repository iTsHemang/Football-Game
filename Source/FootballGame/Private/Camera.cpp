// Fill out your copyright notice in the Description page of Project Settings.


#include "Camera.h"

#include "Ball.h"
#include "Camera/CameraComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ACamera::ACamera()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//RootComponent =  CreateDefaultSubobject<USceneComponent>("Root");
	UCameraComponent* Camera = CreateDefaultSubobject<UCameraComponent>("Camera");
	SetRootComponent(Camera);
	//Camera->SetupAttachment(RootComponent);
	Camera->SetActive(true);
}

// Called when the game starts or when spawned
void ACamera::BeginPlay()
{
	Super::BeginPlay();

	CurrentLoc = GetActorLocation();
}

// Called every frame
void ACamera::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


	if (!Ball) return;

	FVector TargetLoc = Ball->GetActorLocation() + Offset;
	CurrentLoc = FMath::VInterpTo(CurrentLoc, TargetLoc, DeltaTime, FolowSpeed);
	CurrentLoc.Y = FMath::Clamp(CurrentLoc.Y, -600.0f, 600.0f);
	CurrentLoc.X = FMath::Clamp(CurrentLoc.X, -900.0f, -750.0f);
	SetActorLocation(CurrentLoc);

	FVector BallOffset = Ball->GetActorLocation() - CurrentLoc;
	float Yaw = BallOffset.Y * PanSpeed * 0.01f;
	float Pitch = BallOffset.Z * PanSpeed * 0.01f;

	FRotator Look = UKismetMathLibrary::FindLookAtRotation(CurrentLoc, Ball->GetActorLocation());
	Look.Yaw += Yaw;
	Look.Pitch += Pitch;

	SetActorRotation(FMath::RInterpTo(GetActorRotation(), Look, DeltaTime, 2.0f));
}

