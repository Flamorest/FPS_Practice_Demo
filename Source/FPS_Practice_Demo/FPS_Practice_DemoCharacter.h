// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "FPS_Practice_DemoCharacter.generated.h"

class UInputComponent;
class USkeletalMeshComponent;
class UCameraComponent;
class UInputAction;
class UInputMappingContext;
class USoundBase;
class UCameraShakeBase;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

/**
 *  A basic first person character
 */
UCLASS(abstract)
class AFPS_Practice_DemoCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Pawn mesh: first person view (arms; seen only by self) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* FirstPersonMesh;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FirstPersonCameraComponent;

protected:

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, Category ="Input")
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, Category ="Input")
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, Category ="Input")
	class UInputAction* LookAction;

	/** Mouse Look Input Action */
	UPROPERTY(EditAnywhere, Category ="Input")
	class UInputAction* MouseLookAction;

	/** Fire Input Action. If unassigned, a runtime Enhanced Input action is created and mapped to left mouse. */
	UPROPERTY(EditAnywhere, Category ="Input")
	UInputAction* FireInputAction;

	/** Restart Input Action. If unassigned, a runtime Enhanced Input action is created and mapped to R. */
	UPROPERTY(EditAnywhere, Category ="Input")
	UInputAction* RestartInputAction;

	/** Runtime fallback Fire action used when no Fire Input Action asset is assigned. */
	UPROPERTY(Transient)
	UInputAction* RuntimeFireInputAction;

	/** Runtime mapping context that binds Fire to left mouse. */
	UPROPERTY(Transient)
	UInputMappingContext* RuntimeFireMappingContext;

	/** Runtime fallback Restart action used when no Restart Input Action asset is assigned. */
	UPROPERTY(Transient)
	UInputAction* RuntimeRestartInputAction;

	/** Runtime mapping context that binds Restart to R. */
	UPROPERTY(Transient)
	UInputMappingContext* RuntimeRestartMappingContext;

	/** Sound played when the player fires */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Fire", meta = (AllowPrivateAccess = "true"))
	USoundBase* FireSound;

	/** Sound played when the player hits a shooting target */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Fire", meta = (AllowPrivateAccess = "true"))
	USoundBase* HitSound;

	/** Optional camera shake to play when firing */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Fire", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UCameraShakeBase> FireCameraShakeClass;

	/** If true, draw the fire debug line */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Fire", meta = (AllowPrivateAccess = "true"))
	bool bDrawDebugFireLine = true;
	
public:
	AFPS_Practice_DemoCharacter();

protected:

	/** Called from Input Actions for movement input */
	void MoveInput(const FInputActionValue& Value);

	/** Called from Input Actions for looking input */
	void LookInput(const FInputActionValue& Value);

	/** Handles aim inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoAim(float Yaw, float Pitch);

	/** Handles move inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoMove(float Right, float Forward);

	/** Handles jump start inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoJumpStart();

	/** Handles jump end inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoJumpEnd();

	/** Handles fire input */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void Fire();

	/** Returns the configured Fire action, creating a runtime fallback if needed */
	UInputAction* GetOrCreateFireInputAction();

	/** Returns the configured Restart action, creating a runtime fallback if needed */
	UInputAction* GetOrCreateRestartInputAction();

	/** Adds a runtime Enhanced Input mapping for Fire on left mouse */
	void ConfigureRuntimeFireInputMapping();

	/** Adds a runtime Enhanced Input mapping for Restart on R */
	void ConfigureRuntimeRestartInputMapping();

	/** Reloads the current level. By default this is only allowed after victory. */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void RestartLevel();

protected:

	/** Set up input action bindings */
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	

public:

	/** Returns the first person mesh **/
	USkeletalMeshComponent* GetFirstPersonMesh() const { return FirstPersonMesh; }

	/** Returns first person camera component **/
	UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

};

