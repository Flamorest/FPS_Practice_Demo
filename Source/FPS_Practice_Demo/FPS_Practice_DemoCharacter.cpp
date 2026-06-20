// Copyright Epic Games, Inc. All Rights Reserved.

#include "FPS_Practice_DemoCharacter.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Camera/CameraShakeBase.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "DrawDebugHelpers.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "Engine/World.h"
#include "InputAction.h"
#include "InputActionValue.h"
#include "InputCoreTypes.h"
#include "InputMappingContext.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"
#include "FPSDamageableInterface.h"
#include "FPS_Practice_Demo.h"
#include "FPS_Practice_DemoGameMode.h"
#include "ShootingTarget.h"

AFPS_Practice_DemoCharacter::AFPS_Practice_DemoCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);
	
	// Create the first person mesh that will be viewed only by this character's owner
	FirstPersonMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("First Person Mesh"));

	FirstPersonMesh->SetupAttachment(GetMesh());
	FirstPersonMesh->SetOnlyOwnerSee(true);
	FirstPersonMesh->FirstPersonPrimitiveType = EFirstPersonPrimitiveType::FirstPerson;
	FirstPersonMesh->SetCollisionProfileName(FName("NoCollision"));

	// Create the Camera Component	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("First Person Camera"));
	FirstPersonCameraComponent->SetupAttachment(FirstPersonMesh, FName("head"));
	FirstPersonCameraComponent->SetRelativeLocationAndRotation(FVector(-2.8f, 5.89f, 0.0f), FRotator(0.0f, 90.0f, -90.0f));
	FirstPersonCameraComponent->bUsePawnControlRotation = true;
	FirstPersonCameraComponent->bEnableFirstPersonFieldOfView = true;
	FirstPersonCameraComponent->bEnableFirstPersonScale = true;
	FirstPersonCameraComponent->FirstPersonFieldOfView = 70.0f;
	FirstPersonCameraComponent->FirstPersonScale = 0.6f;

	// configure the character comps
	GetMesh()->SetOwnerNoSee(true);
	GetMesh()->FirstPersonPrimitiveType = EFirstPersonPrimitiveType::WorldSpaceRepresentation;

	GetCapsuleComponent()->SetCapsuleSize(34.0f, 96.0f);

	// Configure character movement
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;
	GetCharacterMovement()->AirControl = 0.5f;
}

void AFPS_Practice_DemoCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{	
	ConfigureRuntimeFireInputMapping();
	ConfigureRuntimeRestartInputMapping();

	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &AFPS_Practice_DemoCharacter::DoJumpStart);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &AFPS_Practice_DemoCharacter::DoJumpEnd);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AFPS_Practice_DemoCharacter::MoveInput);

		// Looking/Aiming
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AFPS_Practice_DemoCharacter::LookInput);
		EnhancedInputComponent->BindAction(MouseLookAction, ETriggerEvent::Triggered, this, &AFPS_Practice_DemoCharacter::LookInput);

		// Firing
		EnhancedInputComponent->BindAction(GetOrCreateFireInputAction(), ETriggerEvent::Started, this, &AFPS_Practice_DemoCharacter::Fire);

		// Restart
		EnhancedInputComponent->BindAction(GetOrCreateRestartInputAction(), ETriggerEvent::Started, this, &AFPS_Practice_DemoCharacter::RestartLevel);
	}
	else
	{
		UE_LOG(LogFPS_Practice_Demo, Error, TEXT("'%s' Failed to find an Enhanced Input Component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}


void AFPS_Practice_DemoCharacter::MoveInput(const FInputActionValue& Value)
{
	// get the Vector2D move axis
	FVector2D MovementVector = Value.Get<FVector2D>();

	// pass the axis values to the move input
	DoMove(MovementVector.X, MovementVector.Y);

}

void AFPS_Practice_DemoCharacter::LookInput(const FInputActionValue& Value)
{
	// get the Vector2D look axis
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	// pass the axis values to the aim input
	DoAim(LookAxisVector.X, LookAxisVector.Y);

}

void AFPS_Practice_DemoCharacter::DoAim(float Yaw, float Pitch)
{
	if (GetController())
	{
		// pass the rotation inputs
		AddControllerYawInput(Yaw);
		AddControllerPitchInput(Pitch);
	}
}

void AFPS_Practice_DemoCharacter::DoMove(float Right, float Forward)
{
	if (GetController())
	{
		// pass the move inputs
		AddMovementInput(GetActorRightVector(), Right);
		AddMovementInput(GetActorForwardVector(), Forward);
	}
}

void AFPS_Practice_DemoCharacter::DoJumpStart()
{
	// pass Jump to the character
	Jump();
}

void AFPS_Practice_DemoCharacter::DoJumpEnd()
{
	// pass StopJumping to the character
	StopJumping();
}

void AFPS_Practice_DemoCharacter::Fire()
{
	UWorld* World = GetWorld();
	if (!World || !FirstPersonCameraComponent)
	{
		return;
	}

	constexpr float TraceDistance = 5000.0f;
	const FVector TraceStart = FirstPersonCameraComponent->GetComponentLocation();
	const FVector TraceEnd = TraceStart + (FirstPersonCameraComponent->GetForwardVector() * TraceDistance);

	if (FireSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, TraceStart);
	}

	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (FireCameraShakeClass)
		{
			PlayerController->ClientStartCameraShake(FireCameraShakeClass);
		}
	}

	FHitResult Hit;
	FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(FPS_Practice_Demo_FireTrace), true, this);
	QueryParams.AddIgnoredActor(this);

	const bool bHit = World->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_Visibility, QueryParams);

	if (bHit)
	{
		AActor* HitActor = Hit.GetActor();
		UE_LOG(LogFPS_Practice_Demo, Log, TEXT("Fire hit actor: %s"), *GetNameSafe(HitActor));

		if (HitActor && HitActor->GetClass()->ImplementsInterface(UFPSDamageableInterface::StaticClass()))
		{
			if (HitSound && HitActor->IsA<AShootingTarget>())
			{
				UGameplayStatics::PlaySoundAtLocation(this, HitSound, Hit.ImpactPoint);
			}

			IFPSDamageableInterface::Execute_ReceiveFPSDamage(HitActor, 1.0f, this);
		}
	}

	if (bDrawDebugFireLine)
	{
		const FColor DebugColor = bHit ? FColor::Green : FColor::Red;
		const FVector DebugEnd = bHit ? Hit.ImpactPoint : TraceEnd;
		DrawDebugLine(World, TraceStart, DebugEnd, DebugColor, false, 1.0f, 0, 1.5f);
	}
}

UInputAction* AFPS_Practice_DemoCharacter::GetOrCreateFireInputAction()
{
	if (FireInputAction)
	{
		return FireInputAction;
	}

	if (!RuntimeFireInputAction)
	{
		RuntimeFireInputAction = NewObject<UInputAction>(this, TEXT("IA_Fire_Runtime"));
		RuntimeFireInputAction->ValueType = EInputActionValueType::Boolean;
	}

	return RuntimeFireInputAction;
}

UInputAction* AFPS_Practice_DemoCharacter::GetOrCreateRestartInputAction()
{
	if (RestartInputAction)
	{
		return RestartInputAction;
	}

	if (!RuntimeRestartInputAction)
	{
		RuntimeRestartInputAction = NewObject<UInputAction>(this, TEXT("IA_Restart_Runtime"));
		RuntimeRestartInputAction->ValueType = EInputActionValueType::Boolean;
	}

	return RuntimeRestartInputAction;
}

void AFPS_Practice_DemoCharacter::ConfigureRuntimeFireInputMapping()
{
	UInputAction* FireAction = GetOrCreateFireInputAction();
	if (!FireAction)
	{
		return;
	}

	if (!RuntimeFireMappingContext)
	{
		RuntimeFireMappingContext = NewObject<UInputMappingContext>(this, TEXT("IMC_Fire_Runtime"));
		RuntimeFireMappingContext->MapKey(FireAction, EKeys::LeftMouseButton);
	}

	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (!PlayerController || !PlayerController->IsLocalPlayerController())
	{
		return;
	}

	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
	{
		if (!Subsystem->HasMappingContext(RuntimeFireMappingContext))
		{
			Subsystem->AddMappingContext(RuntimeFireMappingContext, 1);
		}
	}
}

void AFPS_Practice_DemoCharacter::ConfigureRuntimeRestartInputMapping()
{
	UInputAction* RestartAction = GetOrCreateRestartInputAction();
	if (!RestartAction)
	{
		return;
	}

	if (!RuntimeRestartMappingContext)
	{
		RuntimeRestartMappingContext = NewObject<UInputMappingContext>(this, TEXT("IMC_Restart_Runtime"));
		RuntimeRestartMappingContext->MapKey(RestartAction, EKeys::R);
	}

	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (!PlayerController || !PlayerController->IsLocalPlayerController())
	{
		return;
	}

	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
	{
		if (!Subsystem->HasMappingContext(RuntimeRestartMappingContext))
		{
			Subsystem->AddMappingContext(RuntimeRestartMappingContext, 1);
		}
	}
}

void AFPS_Practice_DemoCharacter::RestartLevel()
{
	AFPS_Practice_DemoGameMode* GameMode = GetWorld() ? GetWorld()->GetAuthGameMode<AFPS_Practice_DemoGameMode>() : nullptr;
	if (GameMode && !GameMode->HasWonGame())
	{
		return;
	}

	const FString CurrentLevelName = UGameplayStatics::GetCurrentLevelName(this, true);
	if (!CurrentLevelName.IsEmpty())
	{
		UGameplayStatics::OpenLevel(this, FName(*CurrentLevelName));
	}
}
