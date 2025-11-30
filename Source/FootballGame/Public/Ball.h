

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Ball.generated.h"

class AStricker_cpp;

UCLASS()
class FOOTBALLGAME_API ABall : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABall();

	UPROPERTY(EditAnywhere, Category="Ball")
	UStaticMeshComponent* BallMesh;

	UPROPERTY()
	AStricker_cpp* ControllingStricker;

	bool IsControlled = false;

	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
