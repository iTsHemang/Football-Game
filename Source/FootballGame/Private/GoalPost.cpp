// Fill out your copyright notice in the Description page of Project Settings.


#include "GoalPost.h"

#include "Ball.h"
#include "Components/BoxComponent.h"
#include "TeamManager.h"
#include "DrawDebugHelpers.h"

// Sets default values
AGoalPost::AGoalPost()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GoalPostMesh = CreateDefaultSubobject<UStaticMeshComponent>("Goal Post Mesh");
	SetRootComponent(GoalPostMesh);

	GoalOverlap = CreateDefaultSubobject<UBoxComponent>("Goal Overlap");
	GoalOverlap->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AGoalPost::BeginPlay()
{
	Super::BeginPlay();

	GoalOverlap->OnComponentBeginOverlap.AddDynamic(this, &AGoalPost::OnGoalBallOverlap);
}

// Called every frame
void AGoalPost::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AGoalPost::SetTeamManager(ATeamManager* TM)
{
	TeamManager = TM;
}

void AGoalPost::OnGoalBallOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                  UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor->IsA(ABall::StaticClass()))
	{
		TeamManager->GoalConcede();
	}
}

