// Fill out your copyright notice in the Description page of Project Settings.


#include "Stricker_cpp.h"

#include "Ball.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "TeamManager.h"
#include "Kismet/KismetMathLibrary.h"

#include "DrawDebugHelpers.h"
#include "PlayerControllerCpp.h"


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

	GetCharacterMovement()->bOrientRotationToMovement = false;
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
	CanTackle = false;
	HasBufferAction = false;
	ActionType = 0;
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
	GetCharacterMovement()->bOrientRotationToMovement = false;
}

// Called every frame
void AStricker_cpp::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (HasBall() && Ball->BallMesh)
	{		
	    UStaticMeshComponent* BallMesh = Ball->BallMesh;
	    
		FVector PointDirection = (TargetPoint->GetComponentLocation() - BallMesh->GetComponentLocation()).GetSafeNormal();
		float BallDistance = (TargetPoint->GetComponentLocation() - BallMesh->GetComponentLocation()).Size();

		float Strength = FMath::Clamp(BallDistance * 30.0f, 0.0f, 3000.0f);

		if (IsSprinting)
		{
			Strength *= 1.5f;
		}

		FVector FinalVelocity = PointDirection * Strength;
		
		BallMesh->SetPhysicsLinearVelocity(FinalVelocity);
	}

	if (IsSprinting)
	{
		GetCharacterMovement()->bOrientRotationToMovement = true;
		FVector MoveDir = GetLastMovementInputVector();
		if (!MoveDir.IsNearlyZero())
		{
			TargetRotator = MoveDir.Rotation();
			FRotator NewRotator = FMath::RInterpTo(GetActorRotation(), TargetRotator, DeltaTime, InterpSpeed);
			SetActorRotation(NewRotator);
		}
	}

	else if (!IsSprinting)
	{
		Focus(DeltaTime);
		GetCharacterMovement()->bOrientRotationToMovement = false;
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
	GetCharacterMovement()->bOrientRotationToMovement = true;
	IsSprinting = true;
}

void AStricker_cpp::SprintOff()
{
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	GetCharacterMovement()->bOrientRotationToMovement = false;
	IsSprinting = false;
}

void AStricker_cpp::PassAction(FVector StickDirection)
{	
	if (HasBall())
	{
		if (!Team) return;
		AStricker_cpp* NextStricker = Team->GetPassTarget(this, StickDirection);

		if (NextStricker)
		{
			Pass(NextStricker);
			if (APlayerControllerCpp* PC = Cast<APlayerControllerCpp>(GetController()))
			{
				PC->OnActionExicuted(NextStricker, this);
			}
			GetMesh()->GetAnimInstance()->StopAllMontages(0.0f);
		}

		else if (!NextStricker)
		{
			PassForward(StickDirection);
		}
	}

	else if (!HasBall())
	{
		ActionType = 1;
		TargetDirection = StickDirection;
		HasBufferAction = true;

		GetWorldTimerManager().SetTimer(TimerHandle,this, &AStricker_cpp::ClearAction, 0.3f, false);
	}
}

void AStricker_cpp::LobPassAction(FVector StickDirection)
{
	if (HasBall())
	{
		if (!Team) return;
		AStricker_cpp* NextStricker = Team->GetPassTarget(this, StickDirection);

		if (NextStricker)
		{
			Pass(NextStricker);
			if (APlayerControllerCpp* PC = Cast<APlayerControllerCpp>(GetController()))
			{
				PC->OnActionExicuted(NextStricker, this);
			}
			GetMesh()->GetAnimInstance()->StopAllMontages(0.0f);
		}

		else if (!NextStricker)
		{
			PassForward(StickDirection);
		}
	}

	else if (!HasBall())
	{
		ActionType = 2;
		TargetDirection = StickDirection;
		HasBufferAction = true;

		GetWorldTimerManager().SetTimer(TimerHandle,this, &AStricker_cpp::ClearAction, 0.3f, false);
	}
}

void AStricker_cpp::ThroughAction(FVector StickDirection)
{		
	if (HasBall())
	{
		Through(StickDirection);
	}

	else if (!HasBall())
	{
		ActionType = 3;
		TargetDirection = StickDirection;
		HasBufferAction = true;

		GetWorldTimerManager().SetTimer(TimerHandle,this, &AStricker_cpp::ClearAction, 0.3f, false);
	}
}

void AStricker_cpp::ShootAction(FVector StickDirection)
{
	if (HasBall())
	{
		Shoot(StickDirection);
	}

	else if (!HasBall())
	{
		ActionType = 4;
		TargetDirection = StickDirection;
		HasBufferAction = true;

		GetWorldTimerManager().SetTimer(TimerHandle,this, &AStricker_cpp::ClearAction, 0.3f, false);
	}
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
		if (Ball->ControllingStricker == nullptr)
		{
			IsReciving = false;
			Ball->SetStricker(this);

			if (Team->PlayerStricker != this)
			{
				Team->AutoSwitch(this);
			}

			if (HasBufferAction)
			{
				ExicuteBufferAction();
			}
		}

		else if (Ball->ControllingStricker != nullptr)
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
		Ball->RemoveStricker();
		CanTackle = false;
	}
}

void AStricker_cpp::OnBallHeaderOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	
}

void AStricker_cpp::RemoveBallControlle()
{
	Ball->RemoveStricker();
}

void AStricker_cpp::Focus(float DeltaTime)
{
	float FocusRSpeed = 10.0f;
	FVector FocusTarget;
	
	if (HasBall())
	{
		FocusTarget = Team->OppGoalPost->GetActorLocation();
		FocusRSpeed = 5.0f;
	}

	else if (!HasBall())
	{
		FocusTarget = Ball->GetActorLocation();
		FocusRSpeed = 10.0f;
	}

	FRotator LookAt = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), FocusTarget);
	LookAt.Pitch = 0;
	LookAt.Roll = 0;

	FRotator TargetLook = FMath::RInterpTo(GetActorRotation(), LookAt, DeltaTime, FocusRSpeed);
	SetActorRotation(TargetLook);
}

void AStricker_cpp::ExicuteBufferAction()
{
	if (!HasBufferAction) return;

	switch (ActionType)
	{
		case 1:PassAction(TargetDirection); break;
		case 2:LobPassAction(TargetDirection); break;
		case 3:ThroughAction(TargetDirection); break;
		case 4:ShootAction(TargetDirection); break;
		default: break;
	}

	ClearAction();
}

void AStricker_cpp::ClearAction()
{
	ActionType = 0;
	TargetDirection = FVector::ZeroVector;
	HasBufferAction = false;
}

void AStricker_cpp::Through(FVector Direction)
{
	if (!Ball || !HasBall()) return;
	
	UStaticMeshComponent* BallMesh = Ball->BallMesh;
	
	float Strength = 3500.0f;
		
	RemoveBallControlle();
	BallMesh->SetPhysicsLinearVelocity(Direction * Strength);
}

void AStricker_cpp::Pass(AStricker_cpp* PassStricker)
{
	if (!Ball || !PassStricker || !HasBall()) return;
	
	FVector CurLoc = Ball->GetActorLocation();
	FVector TargetLoc = PassStricker->GetActorLocation();

	FVector TargetLead = TargetLoc + PassStricker->GetVelocity() * 0.2f;

	float Distance = (TargetLead - CurLoc).Size();
	FVector Direction = (TargetLead - CurLoc).GetSafeNormal();
	
	float Speed = FMath::Clamp(Distance* 0.8, 1500.0f, 4000.0f);

	Ball->BallMesh->SetPhysicsLinearVelocity(FVector::ZeroVector);

	RemoveBallControlle();
	Ball->BallMesh->SetPhysicsLinearVelocity(Direction * Speed);
}

void AStricker_cpp::LobPass(AStricker_cpp* PassStricker)
{
	if (!Ball || !PassStricker || !HasBall()) return;

	FVector CurLoc = Ball->GetActorLocation();
	FVector TargetLoc = PassStricker->GetActorLocation();

	FVector TargetLead = TargetLoc + PassStricker->GetVelocity() * 0.2f;

	float Distance = (TargetLead - CurLoc).Size();
	FVector Direction = (TargetLead - CurLoc).GetSafeNormal();
	float Speed = FMath::GetMappedRangeValueClamped(FVector2D(500.f, 6000.f),FVector2D(1200.f, 3000.f),Distance);

	Ball->BallMesh->SetPhysicsLinearVelocity(FVector::ZeroVector);

	float LaunchAngle = FMath::DegreesToRadians(FMath::GetMappedRangeValueClamped(FVector2D(500, 6000), FVector2D(45, 35), Distance));

	
	FVector LaunchVelocity = Direction * Speed;
	
	if (Distance >= 500)
	{
		LaunchVelocity *= FMath::Cos(LaunchAngle);
		LaunchVelocity.Z = Speed * FMath::Sin(LaunchAngle);
	}
	
	RemoveBallControlle();
	Ball->BallMesh->SetPhysicsLinearVelocity(LaunchVelocity);
}

void AStricker_cpp::PassForward(FVector StickDirection)
{
	if (!Ball || !HasBall()) return;

	Ball->BallMesh->SetPhysicsLinearVelocity(FVector::ZeroVector);
	
	RemoveBallControlle();
	Ball->BallMesh->SetPhysicsLinearVelocity(StickDirection * 1500.0f);
}

void AStricker_cpp::LobPassForward(FVector Direction)
{
	if (!Ball || !HasBall()) return;

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
		Ball->SetStricker(this);
	}
}

void AStricker_cpp::Shoot(FVector Direction)
{
	if (!Ball || !HasBall()) return;
	
	float GoalDistance = FVector::Dist(Ball->GetActorLocation(), Team->OppGoalPost->GetActorLocation());

	double TargetAngle = FMath::GetMappedRangeValueClamped(FVector2D(0, 2800), FVector2D(0, 25), GoalDistance);
	
	float Speed = FMath::Clamp(GoalDistance* 0.8, 3000.0f, 5000.0f);
		
	FVector ClearDirection = Direction * Speed * FMath::Cos(FMath::DegreesToRadians(TargetAngle));
	ClearDirection.Z = Speed * FMath::Sin(FMath::DegreesToRadians(TargetAngle));
		
	Ball->BallMesh->SetPhysicsLinearVelocity(FVector::ZeroVector);

	RemoveBallControlle();
	Ball->BallMesh->SetPhysicsLinearVelocity(ClearDirection);
}

bool AStricker_cpp::HasBall()
{
	return Ball && Ball->ControllingStricker == this;
}

void AStricker_cpp::LoseBall()
{
	IsReciving = false;
	Ball->LoseBall();
}
