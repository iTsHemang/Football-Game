// Fill out your copyright notice in the Description page of Project Settings.


#include "StrickerAIController.h"

void AStrickerAIController::MoveAI(FVector TargetPos)
{
	auto Result = MoveToLocation(TargetPos, 5.0f, true, true, true, true);

	//DrawDebugSphere(GetWorld(), TargetPos, 20, 12, FColor::Red, false, 2.0f);
}
