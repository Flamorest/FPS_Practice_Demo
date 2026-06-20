// Copyright Epic Games, Inc. All Rights Reserved.

#include "ShootingTarget.h"
#include "Components/StaticMeshComponent.h"
#include "FPS_Practice_DemoGameMode.h"
#include "FPS_Practice_Demo.h"

AShootingTarget::AShootingTarget()
{
	PrimaryActorTick.bCanEverTick = false;

	TargetMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TargetMesh"));
	SetRootComponent(TargetMesh);

	TargetMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	TargetMesh->SetCollisionObjectType(ECC_WorldDynamic);
	TargetMesh->SetCollisionResponseToAllChannels(ECR_Block);
}

void AShootingTarget::HandleShotHit(AActor* InstigatorActor)
{
	if (bIsDead)
	{
		return;
	}

	bIsDead = true;

	UE_LOG(LogFPS_Practice_Demo, Log, TEXT("Target hit: %s by %s"), *GetNameSafe(this), *GetNameSafe(InstigatorActor));

	if (AFPS_Practice_DemoGameMode* GameMode = GetWorld()->GetAuthGameMode<AFPS_Practice_DemoGameMode>())
	{
		GameMode->AddScore(TargetScoreValue, this);
	}

	if (bHideOnHit)
	{
		UE_LOG(LogFPS_Practice_Demo, Log, TEXT("Target feedback: hiding %s after hit"), *GetNameSafe(this));
		SetActorHiddenInGame(true);
		SetActorEnableCollision(false);
	}

	if (bDestroyOnHit)
	{
		UE_LOG(LogFPS_Practice_Demo, Log, TEXT("Target feedback: destroying %s after hit"), *GetNameSafe(this));
		Destroy();
	}
}

void AShootingTarget::ReceiveFPSDamage_Implementation(float DamageAmount, AActor* DamageCauser)
{
	HandleShotHit(DamageCauser);
}

bool AShootingTarget::IsDead_Implementation() const
{
	return bIsDead;
}
