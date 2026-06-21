// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "FPSDamageableInterface.h"
#include "GameFramework/Character.h"
#include "FPS_Practice_DemoCharacter.generated.h"

class UInputComponent;
class USkeletalMeshComponent;
class UCameraComponent;
class UInputAction;
class UInputMappingContext;
class UTextRenderComponent;
class USoundBase;
class UCameraShakeBase;
class AFPS_Practice_DemoGameState;
class UDeathScreenWidget;
struct FInputActionValue;

/**
 *  A basic first person character
 */
UCLASS(abstract)
class AFPS_Practice_DemoCharacter : public ACharacter, public IFPSDamageableInterface
{
	GENERATED_BODY()

	/** Pawn mesh: first person view (arms; seen only by self) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* FirstPersonMesh;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FirstPersonCameraComponent;

	/** Lightweight overhead marker used to distinguish players in multiplayer */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	UTextRenderComponent* PlayerIdentityMarker;

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

	/** Scoreboard Input Action. If unassigned, a runtime Enhanced Input action is created and mapped to Tab. */
	UPROPERTY(EditAnywhere, Category ="Input")
	UInputAction* ScoreboardInputAction;

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

	/** Runtime fallback Scoreboard action used when no Scoreboard Input Action asset is assigned. */
	UPROPERTY(Transient)
	UInputAction* RuntimeScoreboardInputAction;

	/** Runtime mapping context that binds Scoreboard to Tab. */
	UPROPERTY(Transient)
	UInputMappingContext* RuntimeScoreboardMappingContext;

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

	/** Maximum health for the player */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Health", meta = (AllowPrivateAccess = "true"))
	float MaxHealth = 100.0f;

	/** Current player health */
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category="Health", meta = (AllowPrivateAccess = "true"))
	float CurrentHealth = 100.0f;

	/** True once the player reaches zero health */
	UPROPERTY(ReplicatedUsing=OnRep_PlayerDeadState, VisibleAnywhere, BlueprintReadOnly, Category="Health", meta = (AllowPrivateAccess = "true"))
	bool bIsDead = false;

	/** Widget class shown when the local player dies and can replay */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="UI", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UDeathScreenWidget> DeathScreenWidgetClass;

	/** Active local death screen widget instance */
	UPROPERTY(Transient)
	TObjectPtr<UDeathScreenWidget> DeathScreenWidgetInstance;

	/** Duration for drawing the local hit marker after a confirmed hit */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Feedback", meta = (AllowPrivateAccess = "true"))
	float HitMarkerDuration = 0.2f;

	/** Duration for drawing local damage feedback after taking damage */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Feedback", meta = (AllowPrivateAccess = "true"))
	float DamageFeedbackDuration = 0.2f;

	/** World time until which the hit marker should remain visible */
	UPROPERTY(Transient)
	float HitMarkerEndTime = 0.0f;

	/** World time until which the damage feedback should remain visible */
	UPROPERTY(Transient)
	float DamageFeedbackEndTime = 0.0f;

	/** True while the local player is holding the scoreboard input */
	UPROPERTY(Transient)
	bool bShowScoreboard = false;
	
public:
	AFPS_Practice_DemoCharacter();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

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

	UFUNCTION(Server, Reliable)
	void ServerFire(FVector_NetQuantize TraceStart, FVector_NetQuantizeNormal ShotDirection);

	/** Returns the configured Fire action, creating a runtime fallback if needed */
	UInputAction* GetOrCreateFireInputAction();

	/** Returns the configured Restart action, creating a runtime fallback if needed */
	UInputAction* GetOrCreateRestartInputAction();

	/** Returns the configured Scoreboard action, creating a runtime fallback if needed */
	UInputAction* GetOrCreateScoreboardInputAction();

	/** Adds a runtime Enhanced Input mapping for Fire on left mouse */
	void ConfigureRuntimeFireInputMapping();

	/** Adds a runtime Enhanced Input mapping for Restart on R */
	void ConfigureRuntimeRestartInputMapping();

	/** Adds a runtime Enhanced Input mapping for Scoreboard on Tab */
	void ConfigureRuntimeScoreboardInputMapping();

	/** Reloads the current level. */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void RestartLevel();

	UFUNCTION(Server, Reliable)
	void ServerRequestRestartLevel();

	UFUNCTION(Server, Reliable)
	void ServerRequestRespawn();

	UFUNCTION(BlueprintCallable, Category="Input")
	void ShowScoreboard();

	UFUNCTION(BlueprintCallable, Category="Input")
	void HideScoreboard();

	UFUNCTION(Client, Reliable)
	void ClientShowHitMarker();

	UFUNCTION(Client, Reliable)
	void ClientShowDamageFeedback();

	/** Handles generic FPS damage */
	virtual void ReceiveFPSDamage_Implementation(float DamageAmount, AActor* DamageCauser) override;

	/** Returns true if this character is dead */
	virtual bool IsDead_Implementation() const override;

	void PlayLocalFireEffects(const FVector& FireLocation);

	void ExecuteFireTrace(const FVector& TraceStart, const FVector& ShotDirection, bool bApplyDamage);

	void ApplyDeathState();
	void RestoreAliveState();
	void SetCharacterVisualState(bool bDead);
	void ShowDeathScreen();
	void HideDeathScreen();
	void SetDeathScreenInputMode(bool bEnableUIInput);
	bool ShouldShowDeathScreen() const;
	void HandleMatchStateUpdated();
	void BindMatchStateDelegate();
	void RespawnAtPlayerStart();
	void ApplyPlayerIdentity();

	UFUNCTION()
	void OnRep_PlayerDeadState();

	virtual void BeginPlay() override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;

protected:

	/** Set up input action bindings */
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	

public:

	/** Returns the first person mesh **/
	USkeletalMeshComponent* GetFirstPersonMesh() const { return FirstPersonMesh; }

	/** Returns first person camera component **/
	UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

	/** Returns the player's current health **/
	float GetCurrentHealth() const { return CurrentHealth; }

	/** Returns the player's maximum health **/
	float GetMaxHealth() const { return MaxHealth; }

	/** Returns true if the player is dead **/
	bool IsPlayerDead() const { return bIsDead; }

	/** Handles replay button presses from the local death screen **/
	void HandleReplayButtonClicked();

	/** Reapplies the replicated player identity to the visible marker **/
	void RefreshPlayerIdentity();

	/** Returns true while the local HUD should draw a hit marker **/
	bool ShouldDrawHitMarker() const;

	/** Returns true while the local HUD should draw damage feedback **/
	bool ShouldDrawDamageFeedback() const;

	/** Returns true while the local HUD should draw the scoreboard **/
	bool ShouldDrawScoreboard() const { return bShowScoreboard && !IsDeathScreenVisible(); }

	/** Returns true if the local death screen is currently visible **/
	bool IsDeathScreenVisible() const;

};

