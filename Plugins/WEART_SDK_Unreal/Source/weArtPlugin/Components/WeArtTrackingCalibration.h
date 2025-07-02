/**
 * @file WeArtTrackingCalibration.h
 * @brief Contains the declaration of the AWeArtTrackingCalibration class.
 */
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeArtCommon.h"
#include "WeArtTrackingCalibration.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCalibrationStartSignature,EHandSide,handSide);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCalibrationFinishSignature, EHandSide, handSide);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCalibrationResultSuccessSignature, EHandSide, handSide);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCalibrationResultFailSignature, EHandSide, handSide);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCalibrationStopSignature);

/// <summary>
/// Calibration status/result observer
/// </summary>
UCLASS()
class WEARTPLUGIN_API AWeArtTrackingCalibration : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWeArtTrackingCalibration();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/** @brief Delegate for calibration start event. */
	UPROPERTY(BlueprintAssignable)
		FOnCalibrationStartSignature OnMiddlewareCalibrationStart;

	/**
	 * @brief Blueprint event for calibration start.
	 * @param handSide The side of the hand being calibrated.
	 */
	UFUNCTION(BlueprintImplementableEvent)
		void OnCalibrationStart(EHandSide handSide);

	/** @brief Delegate for calibration finish event. */
	UPROPERTY(BlueprintAssignable)
		FOnCalibrationFinishSignature OnMiddlewareCalibrationFinish;

	/**
	 * @brief Blueprint event for calibration finish.
	 * @param handSide The side of the hand being calibrated.
	 */
	UFUNCTION(BlueprintImplementableEvent)
		void OnCalibrationFinish(EHandSide handSide);

	/** @brief Delegate for calibration result success event. */
	UPROPERTY(BlueprintAssignable)
		FOnCalibrationResultSuccessSignature OnMiddlewareCalibrationResultSuccess;

	/**
	 * @brief Blueprint event for calibration result success.
	 * @param handSide The side of the hand being calibrated.
	 */
	UFUNCTION(BlueprintImplementableEvent)
		void OnCalibrationResultSuccess(EHandSide handSide);

	/** @brief Delegate for calibration result fail event. */
	UPROPERTY(BlueprintAssignable)
		FOnCalibrationResultFailSignature OnMiddlewareCalibrationResultFail;

	/**
	 * @brief Blueprint event for calibration result fail.
	 * @param handSide The side of the hand being calibrated.
	 */
	UFUNCTION(BlueprintImplementableEvent)
		void OnCalibrationResultFail(EHandSide handSide);

	/** @brief Delegate for calibration stop event. */
	UPROPERTY(BlueprintAssignable)
		FOnCalibrationStopSignature OnMiddlewareCalibrationStop;

	/** @brief Blueprint event for calibration stop. */
	UFUNCTION(BlueprintImplementableEvent)
		void OnCalibrationStop();

};
