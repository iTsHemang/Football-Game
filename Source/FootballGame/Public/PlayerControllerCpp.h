// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
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

	FRotator PlayerForward;
	
    void Move(const FInputActionValue& Value);
    void SprintOn();
    void SprintOff();
	void Pass();
    void Through();
    void LobPass();
	void Tackle();

	void SwitchPlayer();

	virtual void SetupInputComponent() override;

public:

	UFUNCTION()
	void SetTeamManager(ATeamManager* TM);

	UPROPERTY()
	AActor* CamActor;

};
