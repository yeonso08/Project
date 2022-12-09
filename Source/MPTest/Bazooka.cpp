// Fill out your copyright notice in the Description page of Project Settings.


#include "Bazooka.h"
#include "Shell.h"
#include "ShooterCharacter.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Kismet/GameplayStatics.h"

ABazooka::ABazooka()
{
	ItemType = EItemType::EIT_Bazooka;

}

void ABazooka::PressShoot_Implementation()
{
	if (Character)
	{
		if (ShootMontage)
		{
			Character->PlayAnimMontage(ShootMontage);
		}

		const USkeletalMeshSocket* SkeletalSocketName = Mesh->GetSocketByName(FName("MuzzleFlash"));
		if (SkeletalSocketName)
		{
			const FTransform SocketTransform = SkeletalSocketName->GetSocketTransform(Mesh);
			if (ShootEffect)
			{
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ShootEffect, SocketTransform);
			}
			if (Shell)
			{
				// 생성 위치 수정 필요
				GetWorld()->SpawnActor<AActor>(Shell, SocketTransform.GetLocation(), Character->GetViewRotation());
			}
		}
	}
}
