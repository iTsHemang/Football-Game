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
	Camera->SetActive(true);
	Camera->FieldOfView = 70.0f;
}

// Called when the game starts or when spawned
void ACamera::BeginPlay()
{
	Super::BeginPlay();

	CurrentLoc = GetActorLocation();
	GetWorldTimerManager().SetTimerForNextTick(this, &ACamera::RotateCam);    
}

// Called every frame
void ACamera::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	MoveCamera(DeltaTime);
}

void ACamera::MoveCamera(float DeltaTime)
{
	if (!Ball) return;

	FVector TargetLoc = Ball->GetActorLocation() + Offset;
	CurrentLoc = FMath::VInterpTo(CurrentLoc, TargetLoc, DeltaTime, FolowSpeed);
	CurrentLoc.Y = FMath::Clamp(CurrentLoc.Y, -800.0f, 800.0f);
	CurrentLoc.X = Offset.X;
	CurrentLoc.Z = Offset.Z;
	SetActorLocation(CurrentLoc);
}

void ACamera::RotateCam()
{
	if (!Ball) return;
	
	FRotator Look = UKismetMathLibrary::FindLookAtRotation(CurrentLoc, Ball->GetActorLocation());
	SetActorRotation(Look);
}
