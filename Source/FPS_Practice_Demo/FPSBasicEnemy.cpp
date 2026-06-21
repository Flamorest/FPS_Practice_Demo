// Copyright Epic Games, Inc. All Rights Reserved.

#include "FPSBasicEnemy.h"
#include "AIController.h"
#include "Animation/AnimInstance.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "FPSBasicEnemyAIController.h"
#include "FPSDamageableInterface.h"
#include "FPS_Practice_Demo.h"
#include "FPS_Practice_DemoGameMode.h"
#include "Engine/World.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "Net/UnrealNetwork.h"
#include "TimerManager.h"

AFPSBasicEnemy::AFPSBasicEnemy()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	SetReplicateMovement(true);
	AIControllerClass = AFPSBasicEnemyAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	// Ensure the enemy can be hit by the player's ECC_Visibility line trace.
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	GetMesh()->SetCollisionResponseToAllChannels(ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

	CurrentHealth = MaxHealth;
}

void AFPSBasicEnemy::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AFPSBasicEnemy, CurrentHealth);
	DOREPLIFETIME(AFPSBasicEnemy, bIsDead);
	DOREPLIFETIME(AFPSBasicEnemy, bIsAttacking);
}

void AFPSBasicEnemy::BeginPlay()
{
	Super::BeginPlay();

	CurrentHealth = MaxHealth;

	if (HasAuthority() && GetWorld())
	{
		GetWorld()->GetTimerManager().SetTimer(MoveUpdateTimer, this, &AFPSBasicEnemy::UpdateMovementTarget, MoveUpdateInterval, true);
	}
}

void AFPSBasicEnemy::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(MoveUpdateTimer);
		GetWorld()->GetTimerManager().ClearTimer(AttackStateTimer);
	}
}

void AFPSBasicEnemy::ReceiveFPSDamage_Implementation(float DamageAmount, AActor* DamageCauser)
{
	if (!HasAuthority() || bIsDead)
	{
		return;
	}

	CurrentHealth = FMath::Max(0.0f, CurrentHealth - DamageAmount);
	UE_LOG(LogFPS_Practice_Demo, Log, TEXT("Enemy took damage: %.1f / Health: %.1f"), DamageAmount, CurrentHealth);

	if (CurrentHealth <= 0.0f)
	{
		Die(DamageCauser);
	}
}

bool AFPSBasicEnemy::IsDead_Implementation() const
{
	return bIsDead;
}

void AFPSBasicEnemy::Die(AActor* DamageCauser)
{
	if (!HasAuthority() || bIsDead)
	{
		return;
	}

	bIsDead = true;
	bIsAttacking = false;
	ApplyDeadState();

	if (AFPS_Practice_DemoGameMode* GameMode = GetWorld()->GetAuthGameMode<AFPS_Practice_DemoGameMode>())
	{
		GameMode->AddScoreForActor(DamageCauser, ScoreValue, this);
	}

	SetLifeSpan(2.0f);
	UE_LOG(LogFPS_Practice_Demo, Log, TEXT("Enemy died on server: %s (Causer: %s)"), *GetNameSafe(this), *GetNameSafe(DamageCauser));
}

void AFPSBasicEnemy::UpdateMovementTarget()
{
	if (!HasAuthority() || bIsDead)
	{
		return;
	}

	AAIController* AIController = Cast<AAIController>(GetController());
	if (!AIController)
	{
		return;
	}

	APawn* TargetPawn = FindNearestPlayerPawn();
	if (TargetPawn)
	{
		const float DistanceToTarget = FVector::Dist(GetActorLocation(), TargetPawn->GetActorLocation());
		if (DistanceToTarget <= AttackRange)
		{
			AIController->StopMovement();
			TryAttackPlayer(TargetPawn);
		}
		else
		{
			AIController->MoveToActor(TargetPawn, AcceptanceRadius, true, true, true, nullptr, true);
		}
	}
	else
	{
		AIController->StopMovement();
	}
}

APawn* AFPSBasicEnemy::FindNearestPlayerPawn() const
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return nullptr;
	}

	APawn* NearestPawn = nullptr;
	float BestDistanceSquared = FMath::Square(DetectionRadius);

	for (FConstPlayerControllerIterator Iterator = World->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		APlayerController* PlayerController = Iterator->Get();
		APawn* PlayerPawn = PlayerController ? PlayerController->GetPawn() : nullptr;
		if (!PlayerPawn)
		{
			continue;
		}

		if (PlayerPawn->GetClass()->ImplementsInterface(UFPSDamageableInterface::StaticClass()) &&
			IFPSDamageableInterface::Execute_IsDead(PlayerPawn))
		{
			continue;
		}

		const float DistanceSquared = FVector::DistSquared(GetActorLocation(), PlayerPawn->GetActorLocation());
		if (DistanceSquared <= BestDistanceSquared)
		{
			BestDistanceSquared = DistanceSquared;
			NearestPawn = PlayerPawn;
		}
	}

	return NearestPawn;
}

void AFPSBasicEnemy::TryAttackPlayer(APawn* TargetPawn)
{
	if (!HasAuthority() || bIsDead || !TargetPawn)
	{
		return;
	}

	if (!TargetPawn->GetClass()->ImplementsInterface(UFPSDamageableInterface::StaticClass()))
	{
		return;
	}

	if (IFPSDamageableInterface::Execute_IsDead(TargetPawn))
	{
		return;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	const float CurrentTime = World->GetTimeSeconds();
	if ((CurrentTime - LastAttackTime) < AttackCooldown)
	{
		return;
	}

	LastAttackTime = CurrentTime;
	IFPSDamageableInterface::Execute_ReceiveFPSDamage(TargetPawn, AttackDamage, this);
	TriggerAttackAnimation();
	UE_LOG(LogFPS_Practice_Demo, Log, TEXT("Enemy attacked player: %s"), *GetNameSafe(TargetPawn));
}

void AFPSBasicEnemy::OnRep_DeadState()
{
	if (bIsDead)
	{
		ApplyDeadState();
	}
}

void AFPSBasicEnemy::ApplyDeadState()
{
	if (!bIsDead)
	{
		return;
	}

	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(MoveUpdateTimer);
		GetWorld()->GetTimerManager().ClearTimer(AttackStateTimer);
	}

	if (AAIController* AIController = Cast<AAIController>(GetController()))
	{
		AIController->StopMovement();
	}

	SetActorEnableCollision(false);
	SetActorHiddenInGame(true);
	GetCharacterMovement()->StopMovementImmediately();
	GetCharacterMovement()->DisableMovement();
	bIsAttacking = false;

	if (USkeletalMeshComponent* MeshComponent = GetMesh())
	{
		if (UAnimInstance* AnimInstance = MeshComponent->GetAnimInstance())
		{
			AnimInstance->Montage_Stop(0.1f);
		}
	}

	UE_LOG(LogFPS_Practice_Demo, Log, TEXT("Enemy dead, stop animation state"));
}

void AFPSBasicEnemy::TriggerAttackAnimation()
{
	if (!HasAuthority() || bIsDead)
	{
		return;
	}

	bIsAttacking = true;
	UE_LOG(LogFPS_Practice_Demo, Log, TEXT("Enemy attack animation triggered: %s"), *GetNameSafe(this));
	MulticastPlayAttackAnimation();

	const float AttackStateDuration = (bUseAttackMontage && AttackMontage) ? AttackMontage->GetPlayLength() : AttackFeedbackDuration;
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(AttackStateTimer);
		GetWorld()->GetTimerManager().SetTimer(AttackStateTimer, this, &AFPSBasicEnemy::ClearAttackState, AttackStateDuration, false);
	}
}

void AFPSBasicEnemy::ClearAttackState()
{
	bIsAttacking = false;
}

void AFPSBasicEnemy::MulticastPlayAttackAnimation_Implementation()
{
	if (bIsDead)
	{
		return;
	}

	USkeletalMeshComponent* MeshComponent = GetMesh();
	UAnimInstance* AnimInstance = MeshComponent ? MeshComponent->GetAnimInstance() : nullptr;
	if (bUseAttackMontage && AttackMontage && AnimInstance)
	{
		AnimInstance->Montage_Play(AttackMontage);
		UE_LOG(LogFPS_Practice_Demo, Log, TEXT("Enemy attack montage played: %s"), *GetNameSafe(this));
	}
	else
	{
		UE_LOG(LogFPS_Practice_Demo, Log, TEXT("Enemy attack montage missing, using fallback: %s"), *GetNameSafe(this));
	}
}
