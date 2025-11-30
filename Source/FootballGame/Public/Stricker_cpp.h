

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Stricker_cpp.generated.h"

class UInputAction;
class USphereComponent;
class ABall;
struct FInputActionValue;

UCLASS()
class FOOTBALLGAME_API AStricker_cpp : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AStricker_cpp();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Speed")
	float WalkSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Speed")
	float SprintSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BallAtachment")
	USceneComponent* TargetPoint;

	UPROPERTY(VisibleAnywhere)
	USphereComponent* CollisionComponent;

	UPROPERTY()
	ABall* Ball;
	
	bool IsSprinting = false;

	FRotator TargetRotator;
	float InterpSpeed;

	void RemoveBallControlle();
	
	

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoMove(float Right, float Forward);

	UFUNCTION()
	void OnBallOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnBallOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
    bool IsBallInControle = false;
	bool IsReciving;
	bool CanTackle = false;

	void Move(const FInputActionValue& Value);
	void SprintOn();
	void SprintOff();
	void Through();
	void Pass(AStricker_cpp* PassStricker);
	void LobPass(AStricker_cpp* PassStricker);
	void PassForward();
	void LobPassForward();
	void Tackle();
};
