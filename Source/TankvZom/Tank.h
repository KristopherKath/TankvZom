// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Tank.generated.h"

class UArrowComponent;
class UPaperSpriteComponent;
class UCameraComponent;

// This struct covers all possible tank input schemes.
// What the inputs do can vary by tank, but the same inputs will always exist.
// Have same keys, but keys may do something different depending on tank
USTRUCT(BlueprintType)
struct FTankInput
{
	GENERATED_BODY()

public:
	// Sanitized movement input, ready to be used by game logic. 
	// (Handles issue of different keys with same purpose. eg: W and UP keys to move up make speed twice as fast)

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TankInput")
	FVector2D MovementInput;

	void Sanitize();
	void MoveX(float AxisValue);
	void MoveY(float AxisValue);

private:
	// Private because it's internal, raw data. Game code should never do this.
	FVector2D RawMovementInput;
};

UCLASS()
class TANKVZOM_API ATank : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ATank();


private:
	void MoveX(float AxisValue);
	void MoveY(float AxisValue);


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	// Helpful debug tool - which way the tank is facing
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Tank", meta = (AllowPrivateAccess = "true"))
	UArrowComponent* TankDirection;

	// Sprite for the tank body.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Tank", meta = (AllowPrivateAccess = "true"))
	UPaperSpriteComponent* TankSprite;

	// The actor used as the turret
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Tank", meta = (AllowPrivateAccess = "true"))
	UChildActorComponent* ChildTurret;

	// The in-game camera
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Tank", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* CameraComponent;


protected:

	// Our input structure.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TankInput", meta = (AllowPrivateAccess = "true"))
	FTankInput TankInput;
};
