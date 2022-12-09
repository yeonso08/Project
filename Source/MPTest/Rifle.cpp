// Fill out your copyright notice in the Description page of Project Settings.


#include "Rifle.h"
#include "Net/UnrealNetwork.h"

ARifle::ARifle()
{
	ItemType = EItemType::EIT_Rifle;
}

// 상속받은 변수 복제 x ??
// 
//void ARifle::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
//{
//	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
//
//	DOREPLIFETIME(ARifle, ShootMontage);
//	DOREPLIFETIME(ARifle, ShootEffect);
//}

void ARifle::PressShoot_Implementation()
{
	if (bPressShoot == false) return;

	Super::PressShoot_Implementation();

	PressShootTimer();

}

void ARifle::PressShootTimer()
{
	if (bPressShoot)
	{
		GetWorldTimerManager().SetTimer(ShootTimer, this,
			&ARifle::PressShoot_Implementation, 0.2f);
	}
	
}
