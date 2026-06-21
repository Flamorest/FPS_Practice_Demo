// Copyright Epic Games, Inc. All Rights Reserved.

#include "ShootingTarget.h"
#include "Components/StaticMeshComponent.h"
#include "FPS_Practice_DemoGameMode.h"
#include "FPS_Practice_Demo.h"
#include "Net/UnrealNetwork.h"

AShootingTarget::AShootingTarget()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	TargetMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TargetMesh"));
	SetRootComponent(TargetMesh);

	TargetMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	TargetMesh->SetCollisionObjectType(ECC_WorldDynamic);
	TargetMesh->SetCollisionResponseToAllChannels(ECR_Block);
}

void AShootingTarget::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AShootingTarget, bIsDead);
}

void AShootingTarget::HandleShotHit(AActor* InstigatorActor)
{
	if (!HasAuthority())
	{
		return;
	}

	if (bIsDead)
	{
		return;
	}

	bIsDead = true;

	FPS_PRACTICE_VERBOSE_LOG(TEXT("Target hit: %s by %s"), *GetNameSafe(this), *GetNameSafe(InstigatorActor));

	if (AFPS_Practice_DemoGameMode* GameMode = GetWorld()->GetAuthGameMode<AFPS_Practice_DemoGameMode>())
	{
		GameMode->AddScoreForActor(InstigatorActor, TargetScoreValue, this);
	}

	ApplyDisabledState();
}

void AShootingTarget::ReceiveFPSDamage_Implementation(float DamageAmount, AActor* DamageCauser)
{
	if (!HasAuthority())
	{
		return;
	}

	HandleShotHit(DamageCauser);
}

bool AShootingTarget::IsDead_Implementation() const
{
	return bIsDead;
}

void AShootingTarget::OnRep_TargetDisabled()
{
	if (bIsDead)
	{
		FPS_PRACTICE_VERBOSE_LOG(TEXT("Replicated target disabled: %s"), *GetNameSafe(this));
		ApplyDisabledState();
	}
}

void AShootingTarget::ApplyDisabledState()
{
	if (!bIsDead)
	{
		return;
	}

	if (bHideOnHit)
	{
		FPS_PRACTICE_VERBOSE_LOG(TEXT("Target feedback: hiding %s after hit"), *GetNameSafe(this));
		SetActorHiddenInGame(true);
		SetActorEnableCollision(false);
	}

	if (bDestroyOnHit && HasAuthority())
	{
		FPS_PRACTICE_VERBOSE_LOG(TEXT("Target feedback: destroying %s after hit"), *GetNameSafe(this));
		Destroy();
	}
}
