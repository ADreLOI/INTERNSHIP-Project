// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "WeArtFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class WEARTPLUGIN_API UWeArtFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "WeArtSettings")
	static const UWeArtSettings* GetWeArtSettings();

	UFUNCTION(BlueprintCallable, Category = "Regex")
	static bool MatchRegex(const FString& InputString, const FString& Pattern);
};
