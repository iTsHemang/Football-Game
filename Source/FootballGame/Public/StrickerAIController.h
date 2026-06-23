// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "StrickerAIController.generated.h"

/**
 * 
 */
UCLASS()
class FOOTBALLGAME_API AStrickerAIController : public AAIController
{
	GENERATED_BODY()

	public:

	void MoveAI(FVector TargetPos);
	
};
