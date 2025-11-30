	// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerControllerCpp.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "InputMappingContext.h"
#include "Stricker_cpp.h"
#include "TeamManager.h"
#include "InputActionValue.h"


void APlayerControllerCpp::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		for (UInputMappingContext* CurrentContext : InputMappingContexts)
		{
			Subsystem->AddMappingContext(CurrentContext, 0);
		}
	}

	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(InputComponent))
	{
		EnhancedInput->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APlayerControllerCpp::Move);

		EnhancedInput->BindAction(SprintAction, ETriggerEvent::Started, this, &APlayerControllerCpp::SprintOn);
		EnhancedInput->BindAction(SprintAction, ETriggerEvent::Completed, this, &APlayerControllerCpp::SprintOff);

		EnhancedInput->BindAction(ThroughAction, ETriggerEvent::Triggered, this , &APlayerControllerCpp::Through);
		EnhancedInput->BindAction(PassAction, ETriggerEvent::Triggered, this, &APlayerControllerCpp::Pass);
		EnhancedInput->BindAction(LobPassAction, ETriggerEvent::Triggered, this, &APlayerControllerCpp::LobPass);

		EnhancedInput->BindAction(SwitchAction, ETriggerEvent::Triggered, this, &APlayerControllerCpp::SwitchPlayer);

		EnhancedInput->BindAction(TackleAction, ETriggerEvent::Triggered, this, &APlayerControllerCpp::Tackle);
	}
		
}

void APlayerControllerCpp::SetTeamManager(ATeamManager* TM)
{
	TeamManager = TM;
	TeamManager->SetControler(this);
}

void APlayerControllerCpp::Move(const FInputActionValue& Value)
{
	if (AStricker_cpp* Stricker = Cast<AStricker_cpp>(GetPawn())) Stricker->Move(Value);
	FVector2D MovementVector = Value.Get<FVector2D>();
}

void APlayerControllerCpp::SprintOn()
{
	if (AStricker_cpp* Stricker = Cast<AStricker_cpp>(GetPawn())) Stricker->SprintOn();
}

void APlayerControllerCpp::SprintOff()
{
	if (AStricker_cpp* Stricker = Cast<AStricker_cpp>(GetPawn())) Stricker->SprintOff();
}

void APlayerControllerCpp::Pass()
{
	AStricker_cpp* CurStricker = Cast<AStricker_cpp>(GetPawn());
	if (!CurStricker || !TeamManager) return;
	
	ABall* Ball = TeamManager->Ball;

	AStricker_cpp* NextStricker = TeamManager->GetPassTarget(CurStricker);

	if (NextStricker)
	{
		CurStricker->Pass(NextStricker);
		NextStricker->IsReciving = true;
		Possess(NextStricker);
		SetViewTargetWithBlend(CamActor, 0.0f);
	}

	else
	{
		CurStricker->PassForward();
	}
}

void APlayerControllerCpp::Through()
{
	if (AStricker_cpp* Stricker = Cast<AStricker_cpp>(GetPawn())) Stricker->Through();
}

void APlayerControllerCpp::LobPass()
{
	AStricker_cpp* CurStricker = Cast<AStricker_cpp>(GetPawn());
	if (!CurStricker || !TeamManager) return;
	
	ABall* Ball = TeamManager->Ball;

	AStricker_cpp* NextStricker = TeamManager->GetPassTarget(CurStricker);

	if (NextStricker)
	{
		CurStricker->LobPass(NextStricker);
		NextStricker->IsReciving = true;
		Possess(NextStricker);
		SetViewTargetWithBlend(CamActor, 0.0f);
	}

	else
	{
		CurStricker->LobPassForward();
	}
}

void APlayerControllerCpp::Tackle()
{
	AStricker_cpp* CurStricker = Cast<AStricker_cpp>(GetPawn());
	CurStricker->Tackle();
}

	void APlayerControllerCpp::SwitchPlayer()
{
	if (!TeamManager) return;

	AStricker_cpp* CurStricker = Cast<AStricker_cpp>(GetPawn());
	
	if (!CurStricker->IsBallInControle)
	{
		if (AStricker_cpp* NewStricker = TeamManager->GetSwitchTarget(CurStricker))
		{
			Possess(NewStricker);
			SetViewTargetWithBlend(CamActor, 0.0f);
		}
	}
}




