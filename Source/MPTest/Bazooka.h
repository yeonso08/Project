// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon.h"
#include "Bazooka.generated.h"

/**
 * 
 */
UCLASS()
class MPTEST_API ABazooka : public AWeapon
{
	GENERATED_BODY()

public:
	ABazooka();

protected:
	virtual void PressShoot_Implementation() override;
	
	UPROPERTY(EditAnywhere, Category = "Shell")
	TSubclassOf<class AShell> Shell;

};
