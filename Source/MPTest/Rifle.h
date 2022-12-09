// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon.h"
#include "Rifle.generated.h"

/**
 * 
 */
UCLASS()
class MPTEST_API ARifle : public AWeapon
{
	GENERATED_BODY()
	
public:
	ARifle();

protected:
	virtual void PressShoot_Implementation() override;

	void PressShootTimer();

	FTimerHandle ShootTimer;

private:
	
public:
	
};
