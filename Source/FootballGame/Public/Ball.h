

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Ball.generated.h"

class AStricker_cpp;
class ATeamManager;

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
	
	UPROPERTY()
	ATeamManager* ControllingTeam;

	bool IsControlled = false;

	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	void SetStricker(AStricker_cpp* Stricker);
	void RemoveStricker();
	void LoseBall();
};
