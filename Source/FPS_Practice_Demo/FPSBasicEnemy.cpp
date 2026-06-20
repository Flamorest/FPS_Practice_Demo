// Copyright Epic Games, Inc. All Rights Reserved.

#include "FPSBasicEnemy.h"
#include "AIController.h"
#include "FPSBasicEnemyAIController.h"
#include "FPSDamageableInterface.h"
#include "FPS_Practice_Demo.h"
#include "FPS_Practice_DemoGameMode.h"
#include "Engine/World.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "TimerManager.h"

AFPSBasicEnemy::AFPSBasicEnemy()
{
	PrimaryActorTick.bCanEverTick = false;
	AIControllerClass = AFPSBasicEnemyAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	CurrentHealth = MaxHealth;
}

void AFPSBasicEnemy::BeginPlay()
{
	Super::BeginPlay();

	if (GetWorld())
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
	}
}

void AFPSBasicEnemy::ReceiveFPSDamage_Implementation(float DamageAmount, AActor* DamageCauser)
{
	if (bIsDead)
	{
		return;
	}

	CurrentHealth = FMath::Max(0.0f, CurrentHealth - DamageAmount);

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
	if (bIsDead)
	{
		return;
	}

	bIsDead = true;

	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(MoveUpdateTimer);
	}

	if (AAIController* AIController = Cast<AAIController>(GetController()))
	{
		AIController->StopMovement();
	}

	SetActorEnableCollision(false);
	SetActorHiddenInGame(true);
	SetLifeSpan(2.0f);
	GetCharacterMovement()->StopMovementImmediately();
	GetCharacterMovement()->DisableMovement();

	if (AFPS_Practice_DemoGameMode* GameMode = GetWorld()->GetAuthGameMode<AFPS_Practice_DemoGameMode>())
	{
		GameMode->AddScore(ScoreValue, this);
	}

	UE_LOG(LogFPS_Practice_Demo, Log, TEXT("Enemy Dead: %s (Causer: %s)"), *GetNameSafe(this), *GetNameSafe(DamageCauser));
}

void AFPSBasicEnemy::UpdateMovementTarget()
{
	if (bIsDead)
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
	if (bIsDead || !TargetPawn)
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
	UE_LOG(LogFPS_Practice_Demo, Log, TEXT("Enemy attacked player: %s"), *GetNameSafe(TargetPawn));
}
