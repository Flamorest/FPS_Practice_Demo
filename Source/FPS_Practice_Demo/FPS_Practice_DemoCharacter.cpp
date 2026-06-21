// Copyright Epic Games, Inc. All Rights Reserved.

#include "FPS_Practice_DemoCharacter.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Camera/CameraShakeBase.h"
#include "Components/CapsuleComponent.h"
#include "Components/MeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "DeathScreenWidget.h"
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
#include "Net/UnrealNetwork.h"
#include "Sound/SoundBase.h"
#include "FPSDamageableInterface.h"
#include "FPS_Practice_Demo.h"
#include "FPS_Practice_DemoGameMode.h"
#include "FPS_Practice_DemoGameState.h"
#include "FPSPracticePlayerState.h"
#include "FPSBasicEnemy.h"
#include "ShootingTarget.h"

AFPS_Practice_DemoCharacter::AFPS_Practice_DemoCharacter()
{
	bReplicates = true;
	SetReplicateMovement(true);
	DeathScreenWidgetClass = UDeathScreenWidget::StaticClass();

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

	// Make sure server-side hitscan using ECC_Visibility can hit player characters.
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

	CurrentHealth = MaxHealth;
}

void AFPS_Practice_DemoCharacter::BeginPlay()
{
	Super::BeginPlay();
	BindMatchStateDelegate();
}

void AFPS_Practice_DemoCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AFPS_Practice_DemoCharacter, CurrentHealth);
	DOREPLIFETIME(AFPS_Practice_DemoCharacter, bIsDead);
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
	if (bIsDead)
	{
		return;
	}

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
	if (bIsDead)
	{
		return;
	}

	if (GetController())
	{
		// pass the move inputs
		AddMovementInput(GetActorRightVector(), Right);
		AddMovementInput(GetActorForwardVector(), Forward);
	}
}

void AFPS_Practice_DemoCharacter::DoJumpStart()
{
	if (bIsDead)
	{
		return;
	}

	// pass Jump to the character
	Jump();
}

void AFPS_Practice_DemoCharacter::DoJumpEnd()
{
	if (bIsDead)
	{
		return;
	}

	// pass StopJumping to the character
	StopJumping();
}

void AFPS_Practice_DemoCharacter::Fire()
{
	if (bIsDead)
	{
		return;
	}

	UWorld* World = GetWorld();
	if (!World || !FirstPersonCameraComponent)
	{
		return;
	}

	const FVector TraceStart = FirstPersonCameraComponent->GetComponentLocation();
	const FVector ShotDirection = FirstPersonCameraComponent->GetForwardVector();

	PlayLocalFireEffects(TraceStart);

	if (HasAuthority())
	{
		ExecuteFireTrace(TraceStart, ShotDirection, true);
	}
	else
	{
		ExecuteFireTrace(TraceStart, ShotDirection, false);
		ServerFire(TraceStart, ShotDirection);
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
	UE_LOG(LogFPS_Practice_Demo, Log, TEXT("Restart requested"));

	if (!HasAuthority())
	{
		ServerRequestRestartLevel();
		return;
	}

	const FString CurrentLevelName = UGameplayStatics::GetCurrentLevelName(this, true);
	if (!CurrentLevelName.IsEmpty())
	{
		UE_LOG(LogFPS_Practice_Demo, Log, TEXT("Restarting level: %s"), *CurrentLevelName);

		if (GetNetMode() == NM_Standalone)
		{
			UGameplayStatics::OpenLevel(this, FName(*CurrentLevelName));
		}
		else if (UWorld* World = GetWorld())
		{
			World->ServerTravel(FString::Printf(TEXT("%s?listen"), *CurrentLevelName));
		}
	}
}

void AFPS_Practice_DemoCharacter::ReceiveFPSDamage_Implementation(float DamageAmount, AActor* DamageCauser)
{
	if (!HasAuthority() || bIsDead)
	{
		return;
	}

	CurrentHealth = FMath::Max(0.0f, CurrentHealth - DamageAmount);
	UE_LOG(LogFPS_Practice_Demo, Log, TEXT("Player health after damage: %.1f / %.1f"), CurrentHealth, MaxHealth);

	if (CurrentHealth <= 0.0f && !bIsDead)
	{
		bIsDead = true;
		ApplyDeathState();
		ShowDeathScreen();

		UE_LOG(LogFPS_Practice_Demo, Log, TEXT("Player killed"));
		UE_LOG(LogFPS_Practice_Demo, Log, TEXT("Player killed by DamageCauser: %s"), *GetNameSafe(DamageCauser));

		if (DamageCauser && DamageCauser != this && DamageCauser->IsA<AFPS_Practice_DemoCharacter>())
		{
			if (AFPS_Practice_DemoGameMode* GameMode = GetWorld() ? GetWorld()->GetAuthGameMode<AFPS_Practice_DemoGameMode>() : nullptr)
			{
				GameMode->AddPlayerKillScore(Cast<AFPS_Practice_DemoCharacter>(DamageCauser)->GetController(), GetController());
			}
		}
	}
}

bool AFPS_Practice_DemoCharacter::IsDead_Implementation() const
{
	return bIsDead;
}

void AFPS_Practice_DemoCharacter::ServerFire_Implementation(FVector_NetQuantize TraceStart, FVector_NetQuantizeNormal ShotDirection)
{
	if (bIsDead)
	{
		return;
	}

	UE_LOG(LogFPS_Practice_Demo, Log, TEXT("ServerFire called"));
	ExecuteFireTrace(TraceStart, ShotDirection, true);
}

void AFPS_Practice_DemoCharacter::ServerRequestRestartLevel_Implementation()
{
	RestartLevel();
}

void AFPS_Practice_DemoCharacter::ServerRequestRespawn_Implementation()
{
	UE_LOG(LogFPS_Practice_Demo, Log, TEXT("ServerRequestRespawn called"));

	AFPS_Practice_DemoGameState* PracticeGameState = GetWorld() ? GetWorld()->GetGameState<AFPS_Practice_DemoGameState>() : nullptr;
	if (!bIsDead || (PracticeGameState && PracticeGameState->HasWonGame()))
	{
		return;
	}

	RespawnAtPlayerStart();
}

void AFPS_Practice_DemoCharacter::PlayLocalFireEffects(const FVector& FireLocation)
{
	if (FireSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, FireLocation);
	}

	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (FireCameraShakeClass)
		{
			PlayerController->ClientStartCameraShake(FireCameraShakeClass);
		}
	}
}

void AFPS_Practice_DemoCharacter::ExecuteFireTrace(const FVector& TraceStart, const FVector& ShotDirection, bool bApplyDamage)
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	constexpr float TraceDistance = 5000.0f;
	const FVector TraceEnd = TraceStart + (ShotDirection * TraceDistance);

	FHitResult Hit;
	FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(FPS_Practice_Demo_FireTrace), true, this);
	QueryParams.AddIgnoredActor(this);

	const bool bHit = World->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_Visibility, QueryParams);

	if (bHit)
	{
		AActor* HitActor = Hit.GetActor();
		const TCHAR* TraceLogPrefix = bApplyDamage ? TEXT("ServerFire") : TEXT("LocalFire");
		UE_LOG(LogFPS_Practice_Demo, Log, TEXT("%s hit actor: %s / %s"), TraceLogPrefix, *GetNameSafe(HitActor), HitActor ? *GetNameSafe(HitActor->GetClass()) : TEXT("None"));
		UE_LOG(LogFPS_Practice_Demo, Log, TEXT("%s hit component: %s"), TraceLogPrefix, *GetNameSafe(Hit.GetComponent()));

		if (HitActor && HitActor != this && HitActor->GetClass()->ImplementsInterface(UFPSDamageableInterface::StaticClass()))
		{
			if (bApplyDamage && HitActor->IsA<AFPS_Practice_DemoCharacter>())
			{
				UE_LOG(LogFPS_Practice_Demo, Log, TEXT("ServerFire hit player: %s"), *GetNameSafe(HitActor));
			}

			if (bApplyDamage)
			{
				if (HitActor->IsA<AFPS_Practice_DemoCharacter>())
				{
					UE_LOG(LogFPS_Practice_Demo, Log, TEXT("Server applied player damage: %s"), *GetNameSafe(HitActor));
				}
				else
				{
					UE_LOG(LogFPS_Practice_Demo, Log, TEXT("Server applied damage to: %s"), *GetNameSafe(HitActor));
				}

				IFPSDamageableInterface::Execute_ReceiveFPSDamage(HitActor, 1.0f, this);
			}
			else if (HitSound && (HitActor->IsA<AShootingTarget>() || HitActor->IsA<AFPS_Practice_DemoCharacter>() || HitActor->IsA<AFPSBasicEnemy>()))
			{
				UGameplayStatics::PlaySoundAtLocation(this, HitSound, Hit.ImpactPoint);
			}
		}
		else if (HitActor == this)
		{
			UE_LOG(LogFPS_Practice_Demo, Log, TEXT("ServerFire ignored self hit"));
		}
	}

	if (bDrawDebugFireLine)
	{
		const FColor DebugColor = bHit ? FColor::Green : FColor::Red;
		const FVector DebugEnd = bHit ? Hit.ImpactPoint : TraceEnd;
		DrawDebugLine(World, TraceStart, DebugEnd, DebugColor, false, 1.0f, 0, 1.5f);
	}
}

void AFPS_Practice_DemoCharacter::ApplyDeathState()
{
	UE_LOG(LogFPS_Practice_Demo, Log, TEXT("Player died, hiding character and disabling collision"));
	UE_LOG(LogFPS_Practice_Demo, Log, TEXT("Applying dead visual state"));

	StopJumping();
	SetCharacterVisualState(true);

	if (UCharacterMovementComponent* CharacterMovementComponent = GetCharacterMovement())
	{
		CharacterMovementComponent->StopMovementImmediately();
		CharacterMovementComponent->Velocity = FVector::ZeroVector;
		CharacterMovementComponent->DisableMovement();
	}
}

void AFPS_Practice_DemoCharacter::RestoreAliveState()
{
	UE_LOG(LogFPS_Practice_Demo, Log, TEXT("Applying alive visual state"));
	SetCharacterVisualState(false);

	if (UCharacterMovementComponent* CharacterMovementComponent = GetCharacterMovement())
	{
		CharacterMovementComponent->StopMovementImmediately();
		CharacterMovementComponent->Velocity = FVector::ZeroVector;
		CharacterMovementComponent->SetMovementMode(MOVE_Walking);
	}
}

void AFPS_Practice_DemoCharacter::SetCharacterVisualState(bool bDead)
{
	TArray<UMeshComponent*> MeshComponents;
	GetComponents<UMeshComponent>(MeshComponents);

	for (UMeshComponent* MeshComponent : MeshComponents)
	{
		if (!MeshComponent)
		{
			continue;
		}

		MeshComponent->SetHiddenInGame(bDead, true);

		if (bDead)
		{
			MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}
		else if (MeshComponent == GetMesh())
		{
			MeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
			MeshComponent->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
		}
		else if (MeshComponent == FirstPersonMesh)
		{
			MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}
	}

	if (UCapsuleComponent* CapsuleCollisionComponent = GetCapsuleComponent())
	{
		if (bDead)
		{
			CapsuleCollisionComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			SetActorEnableCollision(false);
		}
		else
		{
			SetActorEnableCollision(true);
			CapsuleCollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			CapsuleCollisionComponent->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
		}
	}
}

void AFPS_Practice_DemoCharacter::ShowDeathScreen()
{
	if (!ShouldShowDeathScreen())
	{
		return;
	}

	if (!DeathScreenWidgetInstance)
	{
		APlayerController* PlayerController = Cast<APlayerController>(GetController());
		if (!PlayerController || !DeathScreenWidgetClass)
		{
			return;
		}

		DeathScreenWidgetInstance = CreateWidget<UDeathScreenWidget>(PlayerController, DeathScreenWidgetClass);
		if (!DeathScreenWidgetInstance)
		{
			return;
		}

		DeathScreenWidgetInstance->InitializeForCharacter(this);
	}

	if (!DeathScreenWidgetInstance->IsInViewport())
	{
		DeathScreenWidgetInstance->AddToViewport(100);
	}

	DeathScreenWidgetInstance->StartFadeIn();
	SetDeathScreenInputMode(true);
	UE_LOG(LogFPS_Practice_Demo, Log, TEXT("Player died, showing death screen"));
}

void AFPS_Practice_DemoCharacter::HideDeathScreen()
{
	if (DeathScreenWidgetInstance && DeathScreenWidgetInstance->IsInViewport())
	{
		DeathScreenWidgetInstance->RemoveFromParent();
	}

	SetDeathScreenInputMode(false);
}

void AFPS_Practice_DemoCharacter::SetDeathScreenInputMode(bool bEnableUIInput)
{
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (!PlayerController || !PlayerController->IsLocalController())
	{
		return;
	}

	PlayerController->bShowMouseCursor = bEnableUIInput;

	if (bEnableUIInput)
	{
		FInputModeUIOnly InputMode;
		if (DeathScreenWidgetInstance)
		{
			InputMode.SetWidgetToFocus(DeathScreenWidgetInstance->TakeWidget());
		}
		PlayerController->SetInputMode(InputMode);
	}
	else
	{
		FInputModeGameOnly InputMode;
		PlayerController->SetInputMode(InputMode);
	}
}

bool AFPS_Practice_DemoCharacter::ShouldShowDeathScreen() const
{
	if (!IsLocallyControlled() || !bIsDead)
	{
		return false;
	}

	const AFPS_Practice_DemoGameState* PracticeGameState = GetWorld() ? GetWorld()->GetGameState<AFPS_Practice_DemoGameState>() : nullptr;
	return !(PracticeGameState && PracticeGameState->HasWonGame());
}

void AFPS_Practice_DemoCharacter::HandleMatchStateUpdated()
{
	const AFPS_Practice_DemoGameState* PracticeGameState = GetWorld() ? GetWorld()->GetGameState<AFPS_Practice_DemoGameState>() : nullptr;
	if (PracticeGameState && PracticeGameState->HasWonGame())
	{
		HideDeathScreen();
	}
}

void AFPS_Practice_DemoCharacter::BindMatchStateDelegate()
{
	if (!IsLocallyControlled())
	{
		return;
	}

	if (AFPS_Practice_DemoGameState* PracticeGameState = GetWorld() ? GetWorld()->GetGameState<AFPS_Practice_DemoGameState>() : nullptr)
	{
		PracticeGameState->OnMatchStateUpdated.RemoveAll(this);
		PracticeGameState->OnMatchStateUpdated.AddUObject(this, &AFPS_Practice_DemoCharacter::HandleMatchStateUpdated);
	}
}

void AFPS_Practice_DemoCharacter::RespawnAtPlayerStart()
{
	AFPS_Practice_DemoGameState* PracticeGameState = GetWorld() ? GetWorld()->GetGameState<AFPS_Practice_DemoGameState>() : nullptr;
	if (PracticeGameState && PracticeGameState->HasWonGame())
	{
		return;
	}

	AController* CharacterController = GetController();
	AFPS_Practice_DemoGameMode* GameMode = GetWorld() ? GetWorld()->GetAuthGameMode<AFPS_Practice_DemoGameMode>() : nullptr;
	AActor* PlayerStart = (GameMode && CharacterController) ? GameMode->FindPlayerStart(CharacterController) : nullptr;
	if (PlayerStart)
	{
		UE_LOG(LogFPS_Practice_Demo, Log, TEXT("Respawning player at PlayerStart: %s"), *GetNameSafe(PlayerStart));
		SetActorLocationAndRotation(PlayerStart->GetActorLocation(), PlayerStart->GetActorRotation(), false, nullptr, ETeleportType::TeleportPhysics);

		if (CharacterController)
		{
			CharacterController->SetControlRotation(PlayerStart->GetActorRotation());
		}
	}

	CurrentHealth = MaxHealth;
	bIsDead = false;
	RestoreAliveState();
	HideDeathScreen();
	UE_LOG(LogFPS_Practice_Demo, Log, TEXT("Player respawn complete"));
}

void AFPS_Practice_DemoCharacter::OnRep_PlayerDeadState()
{
	if (bIsDead)
	{
		ApplyDeathState();
		ShowDeathScreen();
	}
	else
	{
		RestoreAliveState();
		HideDeathScreen();
	}
}

void AFPS_Practice_DemoCharacter::HandleReplayButtonClicked()
{
	if (AFPS_Practice_DemoGameState* PracticeGameState = GetWorld() ? GetWorld()->GetGameState<AFPS_Practice_DemoGameState>() : nullptr)
	{
		if (PracticeGameState->HasWonGame())
		{
			HideDeathScreen();
			return;
		}
	}

	if (HasAuthority())
	{
		ServerRequestRespawn_Implementation();
	}
	else
	{
		ServerRequestRespawn();
	}
}
