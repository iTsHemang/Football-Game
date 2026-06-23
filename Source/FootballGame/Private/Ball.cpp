// Fill out your copyright notice in the Description page of Project Settings.


#include "Ball.h"

#include "Stricker_cpp.h"
#include "TeamManager.h"

// Sets default values
ABall::ABall()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BallMesh = CreateDefaultSubobject<UStaticMeshComponent>("BallMesh");
	SetRootComponent(BallMesh);

	BallMesh->SetSimulatePhysics(true);
	BallMesh->SetEnableGravity(true);
	BallMesh->SetCollisionProfileName("PhysicsActor");
	
	ControllingStricker = nullptr;
	ControllingTeam = nullptr;

}

// Called when the game starts or when spawned
void ABall::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ABall::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);;
}

void ABall::SetStricker(AStricker_cpp* Stricker)
{
	ControllingStricker = Stricker;

	ATeamManager* Team = Stricker->Team;

	if (ControllingTeam != Team) ControllingTeam = Team;
}

void ABall::RemoveStricker()
{
	ControllingStricker = nullptr;
}

void ABall::LoseBall()
{
	if (ControllingStricker == nullptr && ControllingTeam != nullptr) ControllingTeam = nullptr; 
}

