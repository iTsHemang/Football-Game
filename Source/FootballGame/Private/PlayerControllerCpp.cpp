	// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerControllerCpp.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "InputMappingContext.h"
#include "Stricker_cpp.h"
#include "TeamManager.h"
#include "InputActionValue.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"


APlayerControllerCpp::APlayerControllerCpp()
{
	bAutoManageActiveCameraTarget = false;
}

void APlayerControllerCpp::BeginPlay()
{
	Super::BeginPlay();
	
	FTimerHandle Timer;
	GetWorldTimerManager().SetTimer(Timer, [this]()
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
		{
			for (UInputMappingContext* CurrentContext : InputMappingContexts)
			{
				Subsystem->AddMappingContext(CurrentContext, 0);
			}
		}
	}, 0.2f, false);

	GS = Cast<AGS_Football>(GetWorld()->GetGameState());
}

void APlayerControllerCpp::SetupInputComponent()
{
	Super::SetupInputComponent();
	
	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(InputComponent))
	{
		EnhancedInput->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APlayerControllerCpp::Move);

		EnhancedInput->BindAction(SprintAction, ETriggerEvent::Started, this, &APlayerControllerCpp::SprintOn);
		EnhancedInput->BindAction(SprintAction, ETriggerEvent::Completed, this, &APlayerControllerCpp::SprintOff);

		EnhancedInput->BindAction(ThroughAction, ETriggerEvent::Triggered, this , &APlayerControllerCpp::Through);
		EnhancedInput->BindAction(PassAction, ETriggerEvent::Triggered, this, &APlayerControllerCpp::Pass);
		EnhancedInput->BindAction(LobPassAction, ETriggerEvent::Triggered, this, &APlayerControllerCpp::LobPass);
		EnhancedInput->BindAction(TackleAction, ETriggerEvent::Triggered, this, &APlayerControllerCpp::Tackle_Shoot);

		EnhancedInput->BindAction(SwitchAction, ETriggerEvent::Triggered, this, &APlayerControllerCpp::SwitchPlayer);

	}
		
}


void APlayerControllerCpp::SetTeamManager(ATeamManager* TM)
{
	TeamManager = TM;
	TeamManager->SetControler(this);	
}

void APlayerControllerCpp::OnActionExicuted(AStricker_cpp* Target, AStricker_cpp* PrevStricker)
{
	if (!Target) return;

	Possess(Target);
	TeamManager->PlayerStricker = Target;
	PrevStricker->SpawnDefaultController();
	SetViewTargetWithBlend(CamActor, 0.0f);
	Target->IsReciving=true;
	
	FTimerHandle ReceivingTimer;
	GetWorldTimerManager().ClearTimer(ReceivingTimer);
	GetWorldTimerManager().SetTimer(ReceivingTimer,
		FTimerDelegate::CreateLambda([Target]()
	{
		if (Target)
		{
			Target->LoseBall();
		}
			
	}), 0.5f, false);
}

void APlayerControllerCpp::Move(const FInputActionValue& Value)
{
	if (!GS->IsGameOn) return;
	
    stickdirection.X = Value.Get<FVector2D>().Y;
	stickdirection.Y = Value.Get<FVector2D>().X;
	if (AStricker_cpp* Stricker = Cast<AStricker_cpp>(GetPawn())) Stricker->Move(Value);
	FVector2D MovementVector = Value.Get<FVector2D>();
}

void APlayerControllerCpp::SprintOn()
{
	if (!GS->IsGameOn) return;
	
	if (AStricker_cpp* Stricker = Cast<AStricker_cpp>(GetPawn())) Stricker->SprintOn();
}

void APlayerControllerCpp::SprintOff()
{
	if (!GS->IsGameOn) return;
	
	if (AStricker_cpp* Stricker = Cast<AStricker_cpp>(GetPawn())) Stricker->SprintOff();
}

void APlayerControllerCpp::Pass()
{
	if (!GS->IsGameOn) return;
		
	AStricker_cpp* CurStricker = Cast<AStricker_cpp>(GetPawn());
	if (!CurStricker) return;

	CurStricker->PassAction(stickdirection);
}

void APlayerControllerCpp::Through()
{
	if (!GS->IsGameOn) return;
	
	AStricker_cpp* CurStricker = Cast<AStricker_cpp>(GetPawn());
	if (!CurStricker) return;

	CurStricker->ThroughAction(stickdirection);
	
}

void APlayerControllerCpp::LobPass()
{
	if (!GS->IsGameOn) return;
	
	AStricker_cpp* CurStricker = Cast<AStricker_cpp>(GetPawn());
	if (!CurStricker) return;

	CurStricker->LobPassAction(stickdirection);
}

void APlayerControllerCpp::Tackle_Shoot()
{
	if (!GS->IsGameOn) return;
	
	if (TeamManager->HasPossassion())
	{
		AStricker_cpp* CurStricker = Cast<AStricker_cpp>(GetPawn());
		if (!CurStricker) return;

		CurStricker->ShootAction(stickdirection);
	}

	else
	{
		AStricker_cpp* CurStricker = Cast<AStricker_cpp>(GetPawn());
		CurStricker->Tackle();
	}
}

void APlayerControllerCpp::SwitchPlayer()
{
	if (!TeamManager || TeamManager->HasPossassion() || !GS->IsGameOn) return;

	AStricker_cpp* CurStricker = Cast<AStricker_cpp>(GetPawn());
	
	if (CurStricker && !CurStricker->HasBall())
	{
		if (AStricker_cpp* NewStricker = TeamManager->GetSwitchTarget(CurStricker))
		{
			CurStricker->GetMesh()->GetAnimInstance()->StopAllMontages(0.0f);
			Possess(NewStricker);
			CurStricker->SpawnDefaultController();
			SetViewTargetWithBlend(CamActor, 0.0f);
		}
	}
}







