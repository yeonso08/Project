// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "ShooterHUD.generated.h"

UENUM(BlueprintType)
enum class ECrossHair : uint8
{
	ECH_Pistol UMETA(DisplayName = "CH_Pistol"),
	ECH_PistolMove UMETA(DisplayName = "CH_PistolMove"),

	ECH_Rifle UMETA(DisplayName = "CH_Rifle"),
	ECH_RifleMove UMETA(DisplayName = "CH_RifleMove"),

	ECH_Sniper UMETA(DisplayName = "CH_Sniper"),
	ECH_Bazooka UMETA(DisplayName = "CH_Bazooka"),
};

/**
 * 
 */
UCLASS()
class MPTEST_API AShooterHUD : public AHUD
{
	GENERATED_BODY()

public:
	virtual void DrawHUD() override;

protected:
	virtual void BeginPlay() override;
	
	void BindHealthDele();

	void EITnECH();

	void DrawCrossHair(UTexture2D* Texture, float XAxis, float YAxis);

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class UUserWidget> HUDWidgetClass;

	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	UUserWidget* HUDWidget;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class AShooterCharacter* Character;

	FTimerHandle BindTimer;

	// Crosshair
	UPROPERTY(VisibleAnywhere, Category = "Crosshair")
	ECrossHair CrossHairState;

	UPROPERTY(EditAnywhere, Category = "Crosshair", meta = (AllowPrivateAccess = "true"))
	class UTexture2D* CrossHairTop;
	
	UPROPERTY(EditAnywhere, Category = "Crosshair", meta = (AllowPrivateAccess = "true"))
	UTexture2D* CrossHairBottom;

	UPROPERTY(EditAnywhere, Category = "Crosshair", meta = (AllowPrivateAccess = "true"))
	UTexture2D* CrossHairLeft;

	UPROPERTY(EditAnywhere, Category = "Crosshair", meta = (AllowPrivateAccess = "true"))
	UTexture2D* CrossHairRight;

	UPROPERTY(VisibleAnywhere, Category = "Crosshair", meta = (AllowPrivateAccess = "true"))
	float DrawCrossHairXAxis;

	UPROPERTY(VisibleAnywhere, Category = "Crosshair", meta = (AllowPrivateAccess = "true"))
	float DrawCrossHairYAxis;

	UPROPERTY(VisibleAnywhere, Category = "Crosshair", meta = (AllowPrivateAccess = "true"))
	float CrosshairInAir;

	UPROPERTY(VisibleAnywhere, Category = "Crosshair", meta = (AllowPrivateAccess = "true"))
	float CrosshairIsShoot;

	UPROPERTY(VisibleAnywhere, Category = "Crosshair", meta = (AllowPrivateAccess = "true"))
	float LookUpWhenShoot;

	UPROPERTY(VisibleAnywhere, Category = "Crosshair", meta = (AllowPrivateAccess = "true"))
	float CrosshairCrouch;

public:
	UFUNCTION(BlueprintNativeEvent)
	void SetHealth(float HealthRate);

	void SetCrossHairState(ECrossHair NewCrossHair);
};
