// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GM_Main.h"
#include "Stricker_cpp.h"
#include "GameFramework/PlayerController.h"
#include "PlayerControllerCpp.generated.h"

class ATeamManager;
class UInputMappingContext;
struct FInputActionValue;
class UInputAction;
class ACamera;

/**
 * 
 */
UCLASS()
class FOOTBALLGAME_API APlayerControllerCpp : public APlayerController
{
	GENERATED_BODY()

protected:

	virtual void BeginPlay() override;
	
	virtual void SetupInputComponent() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	UInputMappingContext* DefaultMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Input, meta = (AllowPrivateAccess = "true"))
	TArray<UInputMappingContext*> InputMappingContexts;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Input")
    UInputAction* MoveAction;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Input")
    UInputAction* SprintAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Input")
	UInputAction* PassAction;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Input")
    UInputAction* ThroughAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Input")
	UInputAction* SwitchAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Input")
    UInputAction* ShootAction;
    	
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Input")
    UInputAction* LobPassAction;	
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Input")
    UInputAction* TackleAction;

	UPROPERTY(BlueprintReadOnly)
	ATeamManager* TeamManager;

	AGS_Football* GS;

	FRotator PlayerForward;
	FVector stickdirection;
	
    void Move(const FInputActionValue& Value);
    void SprintOn();
    void SprintOff();
	void Pass();
    void Through();
    void LobPass();
	void Tackle_Shoot();

	void SwitchPlayer();


public:

	APlayerControllerCpp();
	
	UFUNCTION()
	void SetTeamManager(ATeamManager* TM);

	UFUNCTION()
	void OnActionExicuted(AStricker_cpp* Target, AStricker_cpp* PrevStricker);

	UPROPERTY()
	AActor* CamActor;

};
