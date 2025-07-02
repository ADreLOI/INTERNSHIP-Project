// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "WeArtGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class WEARTPLUGIN_API UWeArtGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	virtual void Init() override;

	virtual void Shutdown() override;
};
