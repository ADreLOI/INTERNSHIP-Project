
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeArtCommon.h"
#include "WeArtMessages.h"
#include "WeArtStatusTracker.generated.h"

/**
 * @struct FThimbleStatus
 * @brief Represents the status of a thimble.
 */
USTRUCT(BlueprintType)
struct FThimbleStatus {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ReflectionPoint")
	TEnumAsByte <EActuationPoint> id;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ReflectionPoint")
	bool connected;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ReflectionPoint")
	int statusCode;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ReflectionPoint")
	FString errorDesc;
};

//! @brief Node that represents each of the actuation point
USTRUCT(BlueprintType)
struct FTdProNode {
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ReflectionPoint")
	FString id;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ReflectionPoint")
	bool connected;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ReflectionPoint")
	bool imu_fault;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ReflectionPoint")\
	bool tof_fault;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ReflectionPoint")
	bool biometric_fault;
};

/**
 * @struct FConnectedDeviceStatus
 * @brief Represents the status of a connected device.
 */
USTRUCT(BlueprintType)
struct FConnectedDeviceStatus {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ReflectionPoint")
	FString macAddress;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ReflectionPoint")
	TEnumAsByte <EHandSide> handSide;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ReflectionPoint")
	int32 batteryLevel;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ReflectionPoint")
	bool charging;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ReflectionPoint")
	TArray<FThimbleStatus> thimbles;
};

/**
 * @struct FConnectedDeviceStatus
 * @brief Represents the status of a connected device.
 */
USTRUCT(BlueprintType)
struct FConnectedTdProDeviceStatus {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ReflectionPoint")
	FString MacAddress;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ReflectionPoint")
	TEnumAsByte <EHandSide> HandSide;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ReflectionPoint")
	int BatteryLevel;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ReflectionPoint")
	bool Charging;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ReflectionPoint")
	bool ChargeCompleted;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ReflectionPoint")
	bool UsbConnected;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ReflectionPoint")
	bool WifiConnected;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ReflectionPoint")
	bool WifiOn;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ReflectionPoint")
	bool BleConnected;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ReflectionPoint")
	bool BleOn;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ReflectionPoint")
	bool ImuFault;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ReflectionPoint")
	bool AdcFault;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ReflectionPoint")
	bool StatusError;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ReflectionPoint")
	bool ButtonPushed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ReflectionPoint")
	TArray<FTdProNode> Nodes;
};

/**
 * @struct FMiddlewareConnectedDevice
 * @brief Represents a connected device in the middleware.
 */
USTRUCT(BlueprintType)
struct FMiddlewareConnectedDevice {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ReflectionPoint")
	FString macAddress = "";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ReflectionPoint")
	TEnumAsByte <EHandSide> handSide = EHandSide::Left;
};

/**
 * @struct FMiddlewareStatusData
 * @brief Represents the status data of the middleware.
 */
USTRUCT(BlueprintType)
struct FMiddlewareStatusData {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ReflectionPoint")
	FString timestamp = "";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ReflectionPoint")
	FString status = "DISCONNECTED";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ReflectionPoint")
	FString version = "";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ReflectionPoint")
	FString statusCode = "";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ReflectionPoint")
	FString errorDesc = "";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ReflectionPoint")
	bool actuationsEnabled = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ReflectionPoint")
	TArray<FMiddlewareConnectedDevice> connectedDevices;
};

/**
 * @struct FWeartAppStatusData
 * @brief Represents the status data of the weart app.
 */
USTRUCT(BlueprintType)
struct FWeartAppStatusData : public FMiddlewareStatusData {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Touch Diver Pro | ReflectionPoint")
	FString ConnectionType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Touch Diver Pro | ReflectionPoint")
	bool AutoConnection;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Touch Diver Pro | ReflectionPoint")
	FString DeviceSelection;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Touch Diver Pro | ReflectionPoint")
	bool TrackingPlayback;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Touch Diver Pro | ReflectionPoint")
	bool RawDataLog;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Touch Diver Pro | ReflectionPoint")
	bool SensorOnMask;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStatusCalibrationStartSignature, EHandSide, handSide);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStatusCalibrationFinishSignature, EHandSide, handSide);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStatusCalibrationResultSuccessSignature, EHandSide, handSide);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStatusCalibrationResultFailSignature, EHandSide, handSide);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStatusCalibrationStopSignature);

// G1 Delegates
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMiddlewareSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDevicesSignature);

// G2 Delegates (Touch Diver Pro)
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnWeartAppSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTdProDevicesSignature);

/**
 * @class AWeArtStatusTracker
 * @brief Tracks the status of connected devices and middleware.
 */
UCLASS()
class WEARTPLUGIN_API AWeArtStatusTracker : public AActor
{
	GENERATED_BODY()
public:	
	// Sets default values for this actor's properties
	AWeArtStatusTracker();

	MiddlewareStatusData MiddlewareStatusDataCPP;
	WeartAppStatusData WeartAppStatusDataCPP;
	
	UPROPERTY(BlueprintReadWrite, Category = "ReflectionPoint")
	FMiddlewareStatusData UnrealMiddlewareStatusData;

	UPROPERTY(BlueprintReadWrite, Category = "ReflectionPoint")
	FWeartAppStatusData UnrealWeartAppStatusData;
	
	UPROPERTY(BlueprintReadWrite, Category = "ReflectionPoint")
	FConnectedDeviceStatus ConnectedDeviceStatusRight;
	UPROPERTY(BlueprintReadWrite, Category = "ReflectionPoint")
	FConnectedDeviceStatus ConnectedDeviceStatusLeft;
	
	UPROPERTY(BlueprintReadWrite, Category = "ReflectionPoint")
	FConnectedTdProDeviceStatus ConnectedTdProDeviceStatusRight;
	UPROPERTY(BlueprintReadWrite, Category = "ReflectionPoint")
	FConnectedTdProDeviceStatus ConnectedTdProDeviceStatusLeft;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	bool isInitiated = false;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Delegates for calibration status
	UPROPERTY(BlueprintAssignable)
	FOnStatusCalibrationStartSignature OnStatusMiddlewareCalibrationStart;

	UFUNCTION(BlueprintImplementableEvent)
	void OnStatusCalibrationStart(EHandSide handSide);

	UPROPERTY(BlueprintAssignable)
	FOnStatusCalibrationFinishSignature OnStatusMiddlewareCalibrationFinish;

	UFUNCTION(BlueprintImplementableEvent)
	void OnStatusCalibrationFinish(EHandSide handSide);

	UPROPERTY(BlueprintAssignable)
	FOnStatusCalibrationResultSuccessSignature OnStatusMiddlewareCalibrationResultSuccess;

	UFUNCTION(BlueprintImplementableEvent)
	void OnStatusCalibrationResultSuccess(EHandSide handSide);

	UPROPERTY(BlueprintAssignable)
	FOnStatusCalibrationResultFailSignature OnStatusMiddlewareCalibrationResultFail;

	UFUNCTION(BlueprintImplementableEvent)
	void OnStatusCalibrationResultFail(EHandSide handSide);

	UPROPERTY(BlueprintAssignable)
	FOnStatusCalibrationStopSignature OnStatusMiddlewareCalibrationStop;

	UFUNCTION(BlueprintImplementableEvent)
	void OnStatusCalibrationStop();

	// Delegate for middleware status
	UPROPERTY(BlueprintAssignable)
	FOnMiddlewareSignature OnMiddlewareSignature;
	UFUNCTION(BlueprintImplementableEvent)
	void OnMiddlewareStatus();

	// Delegate for weart app status
	UPROPERTY(BlueprintAssignable)
	FOnWeartAppSignature OnWeartAppSignature;
	UFUNCTION(BlueprintImplementableEvent)
	void OnWeartAppStatus();
	
	// Delegate for device status
	UPROPERTY(BlueprintAssignable)
	FOnDevicesSignature OnDevicesSignature;
	UFUNCTION(BlueprintImplementableEvent)
	void OnDevicesStatus();

	// Delegate for td pro device status
	UPROPERTY(BlueprintAssignable)
	FOnTdProDevicesSignature OnTdProDevicesSignature;
	UFUNCTION(BlueprintImplementableEvent)
	void OnTdProDevicesStatus();
	
	/** Sets the middleware status data. */
	void SetMiddlewareStatus(const MiddlewareStatusData& statusData);
	/** Sets the weart app (touch diver pro) status data. */
	void SetWeartAppData(const WeartAppStatusData& statusData);
	/** Sets the connected devices status. */
	void SetConnectedDevicesStatus(const std::vector<ConnectedDeviceStatus>& devices);
	/** Init g1 device */
	void InitG1Device(FConnectedDeviceStatus& DeviceStatus, const std::vector<ConnectedDeviceStatus>& Devices,
	                         int32 DeviceIndex);
	/** Init td pro device */
	void InitTdProDevice(FConnectedTdProDeviceStatus& DeviceStatus, const std::vector<TdProConnectedDeviceStatus>& Devices, int32 DeviceIndex);
	/** Sets touch diver pro connected devices status. */
	void SetConnectedTdProDevicesStatus(const std::vector<TdProConnectedDeviceStatus>& devices);
};

