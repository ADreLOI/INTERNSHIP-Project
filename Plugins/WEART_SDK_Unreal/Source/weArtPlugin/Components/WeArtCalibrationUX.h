// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeArtCommon.h"
#include "WeArtCalibrationUX.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnUXCalibrationStartSignature, EHandSide, handSide);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnUXCalibrationFinishSignature, EHandSide, handSide);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnUXCalibrationResultSuccessSignature, EHandSide, handSide);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnUXCalibrationResultFailSignature, EHandSide, handSide);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnUXCalibrationStopSignature);

UCLASS()
class WEARTPLUGIN_API AWeArtCalibrationUX : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWeArtCalibrationUX();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(BlueprintAssignable)
		FOnUXCalibrationStartSignature OnUXMiddlewareCalibrationStart;

	UFUNCTION(BlueprintImplementableEvent)
		void OnUXCalibrationStart(EHandSide handSide);

	UPROPERTY(BlueprintAssignable)
		FOnUXCalibrationFinishSignature OnUXMiddlewareCalibrationFinish;

	UFUNCTION(BlueprintImplementableEvent)
		void OnUXCalibrationFinish(EHandSide handSide);

	UPROPERTY(BlueprintAssignable)
		FOnUXCalibrationResultSuccessSignature OnUXMiddlewareCalibrationResultSuccess;

	UFUNCTION(BlueprintImplementableEvent)
		void OnUXCalibrationResultSuccess(EHandSide handSide);

	UPROPERTY(BlueprintAssignable)
		FOnUXCalibrationResultFailSignature OnUXMiddlewareCalibrationResultFail;

	UFUNCTION(BlueprintImplementableEvent)
		void OnUXCalibrationResultFail(EHandSide handSide);

	UPROPERTY(BlueprintAssignable)
		FOnUXCalibrationStopSignature OnUXMiddlewareCalibrationStop;

	UFUNCTION(BlueprintImplementableEvent)
		void OnUXCalibrationStop();
};
