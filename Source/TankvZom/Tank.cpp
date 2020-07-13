// Fill out your copyright notice in the Description page of Project Settings.


#include "Tank.h"
#include "PaperSpriteComponent.h"
#include "Components/ArrowComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "TankStatics.h"


void FTankInput::Sanitize()
{
	MovementInput = RawMovementInput.ClampAxes(-1.0f, 1.0f); // Gets total input and clamps it to a square
	MovementInput = MovementInput.GetSafeNormal(); // Takes square input and shapes it down into a unit circle since x=1 and y=1 is outsiderange of unit circle
	RawMovementInput.Set(0.0f, 0.0f); // Resets RawMovementInput so movement is not always in that direction
}

void FTankInput::MoveX(float AxisValue)
{
	RawMovementInput.X += AxisValue;
}

void FTankInput::MoveY(float AxisValue)
{
	RawMovementInput.Y += AxisValue;
}

// Sets default values
ATank::ATank()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Root Object
	if (!RootComponent)
	{
		RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("TankBase"));
	}

	// Direction of tank body
	TankDirection = CreateDefaultSubobject<UArrowComponent>(TEXT("TankDirection"));
	TankDirection->AttachToComponent(RootComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale, NAME_None);

	// Sprite for tank body
	TankSprite = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("TankSprite"));
	TankSprite->AttachToComponent(TankDirection, FAttachmentTransformRules::SnapToTargetNotIncludingScale, NAME_None);

	// Spring Arm Component
	USpringArmComponent* SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->TargetArmLength = 500.0f;
	SpringArm->bEnableCameraLag = true;
	SpringArm->bEnableCameraRotationLag = false;
	SpringArm->bUsePawnControlRotation = false;
	SpringArm->CameraLagSpeed = 2.0f;
	SpringArm->bDoCollisionTest = false;
	SpringArm->AttachToComponent(RootComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale, NAME_None);
	SpringArm->SetWorldRotation(FRotator(-90.0f, 0.0f, 0.0f)); //Straight down rotation

	//Camera Component
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComponent->bUsePawnControlRotation = false;
	CameraComponent->ProjectionMode = ECameraProjectionMode::Orthographic; // Makes perspective flat view of objects
	CameraComponent->OrthoWidth = 1024.0f; // The width in unreal units of the camera view
	CameraComponent->AspectRatio = 3.0f / 4.0f; //width by height
	CameraComponent->AttachToComponent(SpringArm, FAttachmentTransformRules::SnapToTargetNotIncludingScale, USpringArmComponent::SocketName);
	CameraComponent->SetWorldRotation(FRotator(-90.0f, 0.0f, 0.0f)); // Camera points downward

	// Turret Attachement
	ChildTurret = CreateDefaultSubobject<UChildActorComponent>(TEXT("Turret"));
	ChildTurret->AttachToComponent(TankDirection, FAttachmentTransformRules::SnapToTargetNotIncludingScale, NAME_None);
}

// Called when the game starts or when spawned
void ATank::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATank::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	TankInput.Sanitize();
	UE_LOG(LogTemp, Warning, TEXT("Movement: (%f %f)"), TankInput.MovementInput.X, TankInput.MovementInput.Y);

	// Move the tank
	{
		FVector DesiredMovementDirection = FVector(TankInput.MovementInput.X, TankInput.MovementInput.Y, 0.0f);

		if (!DesiredMovementDirection.IsNearlyZero())
		{
			// Rotate the tank. Note that we rotate the TankDirection component,
			// not the RootComponent.
			FRotator MovementAngle = DesiredMovementDirection.Rotation(); // already normalized
			float DeltaYaw = UTankStatics::FindDeltaAngleDegrees(TankDirection->GetComponentRotation().Yaw, MovementAngle.Yaw); // gets difference of what tank is facing to where it wants to face
			bool bReverse = false;
			if (DeltaYaw != 0.0f)
			{
				float AdjustedDeltaYaw = DeltaYaw;

				if (AdjustedDeltaYaw < -90.0f)
				{
					AdjustedDeltaYaw += 180.0f;
					bReverse = true;
				}
				else if (AdjustedDeltaYaw > 90.0f)
				{
					AdjustedDeltaYaw -= 180.0f;
					bReverse = true;
				}

				// Turn toward the desired angle. Stop if we can get there in one frame.
				float MaxYawThisFrame = YawSpeed * DeltaTime;
				if (MaxYawThisFrame >= FMath::Abs(AdjustedDeltaYaw))
				{
					if (bReverse)
					{
						// Move backward
						FRotator FacingAngle = MovementAngle;
						FacingAngle.Yaw = MovementAngle.Yaw + 180.0f;
						TankDirection->SetWorldRotation(FacingAngle);
					}
					else
					{
						TankDirection->SetWorldRotation(MovementAngle);
					}
				}
				else
				{
					// Can't reach our desired angle this frame, rotate part way.
					TankDirection->AddLocalRotation(FRotator(0.0f, FMath::Sign(AdjustedDeltaYaw) * MaxYawThisFrame, 0.0f));
				}
			}

			// Move the tank
			{
				FVector MovementDirection = TankDirection->GetForwardVector() * (bReverse ? -1.0f : 1.0f);
				FVector Pos = GetActorLocation();
				Pos.X += MovementDirection.X * MoveSpeed * DeltaTime;
				Pos.Y += MovementDirection.Y * MoveSpeed * DeltaTime;
				SetActorLocation(Pos);
			}
		}
	}
}

// Called to bind functionality to input
void ATank::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Name of axis assigned in editor input settings, on this tank, and reference to method to call
	InputComponent->BindAxis("MoveX", this, &ATank::MoveX);
	InputComponent->BindAxis("MoveY", this, &ATank::MoveY);

}

void ATank::MoveX(float AxisValue)
{
	TankInput.MoveX(AxisValue);
}

void ATank::MoveY(float AxisValue)
{
	TankInput.MoveY(AxisValue);
}