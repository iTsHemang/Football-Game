// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "GS_Football.generated.h"

/**
 * 
 */
UCLASS()
class FOOTBALLGAME_API AGS_Football : public AGameStateBase
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadOnly, Category="Game")
	bool IsGameOn = false;

	UPROPERTY(BlueprintReadOnly, Category="Game")
	int ScoreA;

	UPROPERTY(BlueprintReadOnly, Category="Game")
	int ScoreB;
	
};
