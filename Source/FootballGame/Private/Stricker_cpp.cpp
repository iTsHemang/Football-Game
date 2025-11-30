// Fill out your copyright notice in the Description page of Project Settings.


#include "Stricker_cpp.h"

#include "Ball.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"

// Sets default values
AStricker_cpp::AStricker_cpp()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_PhysicsBody, ECR_Ignore);

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 640.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;

	TargetPoint = CreateDefaultSubobject<USceneComponent>("BallTarget");
	TargetPoint->SetupAttachment(RootComponent);
	TargetPoint->SetRelativeLocation(FVector(50.0f, 0.0f, 0.0f));

	CollisionComponent = CreateDefaultSubobject<USphereComponent>("Collision Component");
	CollisionComponent->SetupAttachment(RootComponent);
	CollisionComponent->SetSphereRadius(100.0f);

	InterpSpeed = 3.0f;
	IsReciving = false;
}

// Called to bind functionality to input
void AStricker_cpp::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
	}
}

// Called when the game starts or when spawned
void AStricker_cpp::BeginPlay()
{
	Super::BeginPlay();

	CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &AStricker_cpp::OnBallOverlap);
	CollisionComponent->OnComponentEndOverlap.AddDynamic(this, &AStricker_cpp::OnBallOverlapEnd);
	
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}

// Called every frame
void AStricker_cpp::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (IsBallInControle && Ball->BallMesh)
	{
	    UStaticMeshComponent* BallMesh = Ball->BallMesh;
	    
		FVector PointDirection = (TargetPoint->GetComponentLocation() - BallMesh->GetComponentLocation()).GetSafeNormal();
		float BallDistance = (TargetPoint->GetComponentLocation() - BallMesh->GetComponentLocation()).Size();

		float Strength = FMath::Clamp(BallDistance * 30.0f, 0.0f, 3000.0f);

		if (IsSprinting)
		{
			Strength *= 2.5f;
		}

		FVector FinalVelocity = PointDirection * Strength;
		
		BallMesh->SetPhysicsLinearVelocity(FinalVelocity);
	}

	if (IsSprinting)
	{
		FVector MoveDir = GetLastMovementInputVector();
		if (!MoveDir.IsNearlyZero())
		{
			TargetRotator = MoveDir.Rotation();
			FRotator NewRotator = FMath::RInterpTo(GetActorRotation(), TargetRotator, DeltaTime, InterpSpeed);
			SetActorRotation(NewRotator);
		}
	}
	
}

void AStricker_cpp::Move(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();
	DoMove(MovementVector.X, MovementVector.Y);
}

void AStricker_cpp::SprintOn()
{
	GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
	GetCharacterMovement()->bOrientRotationToMovement = false;
	IsSprinting = true;
}

void AStricker_cpp::SprintOff()
{
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	IsSprinting = false;
}


void AStricker_cpp::DoMove(float Right, float Forward)
{
	if (GetController() != nullptr)
	{
		const FVector ForwardDirection = FVector::ForwardVector;
		const FVector RightDirection = FVector::RightVector;

		FVector InputVector = FVector::ZeroVector;
		InputVector.X = Forward;
		InputVector.Y = Right;

		if (IsSprinting)
		{
			InputVector *= 0.7f;
		}
		
		if (!IsReciving)
		{
			AddMovementInput(ForwardDirection, InputVector.X);
			AddMovementInput(RightDirection, InputVector.Y);
		}
	}
}

void AStricker_cpp::OnBallOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor->IsA(ABall::StaticClass()))
	{
		Ball = Cast<ABall>(OtherActor);

		if (!Ball->IsControlled)
		{
			IsReciving = false;
			IsBallInControle = true;
			Ball-> IsControlled = true;
			Ball->ControllingStricker = this;
		}

		else if (Ball->IsControlled)
		{
			CanTackle = true;
		}
	}
}

void AStricker_cpp::OnBallOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor == Ball)
	{
		IsBallInControle = false;
		Ball = nullptr;
		CanTackle = false;
	}
}

void AStricker_cpp::RemoveBallControlle()
{
	IsBallInControle = false;
	Ball-> ControllingStricker = nullptr;
	Ball-> IsControlled = false;
}

void AStricker_cpp::Through()
{
	
	if (IsBallInControle && Ball->BallMesh)
	{
		UStaticMeshComponent* BallMesh = Ball->BallMesh;
		FVector ThroughDirection = this->GetActorForwardVector();
		
		float Strength = 3500.0f;
		
		RemoveBallControlle();
		BallMesh->SetPhysicsLinearVelocity(ThroughDirection * Strength);
	}
}

void AStricker_cpp::Pass(AStricker_cpp* PassStricker)
{
	if (!Ball || !PassStricker) return;

	FVector CurLoc = Ball->GetActorLocation();
	FVector TargetLoc = PassStricker->GetActorLocation();

	FVector TargetLead = TargetLoc + PassStricker->GetVelocity() * 0.2f;

	float Distance = (TargetLead - CurLoc).Size();
	FVector Direction = (TargetLead - CurLoc).GetSafeNormal();
	
	float Speed = FMath::Clamp(Distance* 0.8, 1000.0f, 3500.0f);

	Ball->BallMesh->SetPhysicsLinearVelocity(FVector::ZeroVector);

	RemoveBallControlle();
	Ball->BallMesh->SetPhysicsLinearVelocity(Direction * Speed);
}

void AStricker_cpp::LobPass(AStricker_cpp* PassStricker)
{
	if (!Ball || !PassStricker) return;

	FVector CurLoc = Ball->GetActorLocation();
	FVector TargetLoc = PassStricker->GetActorLocation();

	FVector TargetLead = TargetLoc + PassStricker->GetVelocity() * 0.2f;

	float Distance = (TargetLead - CurLoc).Size();
	FVector Direction = (TargetLead - CurLoc).GetSafeNormal();
	float Speed = FMath::GetMappedRangeValueClamped(FVector2D(500.f, 6000.f),FVector2D(1200.f, 3000.f),Distance);

	Ball->BallMesh->SetPhysicsLinearVelocity(FVector::ZeroVector);

	float LaunchAngle = FMath::DegreesToRadians(FMath::GetMappedRangeValueClamped(FVector2D(500, 6000), FVector2D(45, 35), Distance));

	//GEngine->AddOnScreenDebugMessage(1, 5.0f, FColor::Green, FString::SanitizeFloat(LaunchAngle),true );
	FVector LaunchVelocity = Direction * Speed;
	
	if (Distance >= 500)
	{
		LaunchVelocity *= FMath::Cos(LaunchAngle);
		LaunchVelocity.Z = Speed * FMath::Sin(LaunchAngle);
	}
	
	RemoveBallControlle();
	Ball->BallMesh->SetPhysicsLinearVelocity(LaunchVelocity);
}

void AStricker_cpp::PassForward()
{
	if (!Ball) return;

	FVector Direction = GetActorForwardVector();

	Ball->BallMesh->SetPhysicsLinearVelocity(FVector::ZeroVector);
	
	RemoveBallControlle();
	Ball->BallMesh->SetPhysicsLinearVelocity(Direction * 1500.0f);
}

void AStricker_cpp::LobPassForward()
{
	if (!Ball) return;

	FVector Direction = GetActorForwardVector();

	Ball->BallMesh->SetPhysicsLinearVelocity(FVector::ZeroVector);

	FVector LobVelocity = Direction * 1500.0f * FMath::Cos(FMath::DegreesToRadians(45));
	LobVelocity.Z = 1500.0f * FMath::Sin(FMath::DegreesToRadians(45));
	
	RemoveBallControlle();
	Ball->BallMesh->SetPhysicsLinearVelocity(LobVelocity);
	
}

void AStricker_cpp::Tackle()
{
	if (CanTackle)
	{
		Ball->ControllingStricker->IsBallInControle=false;
		Ball->ControllingStricker=nullptr;
		Ball->ControllingStricker=this;
		IsBallInControle=true;
	}
}
