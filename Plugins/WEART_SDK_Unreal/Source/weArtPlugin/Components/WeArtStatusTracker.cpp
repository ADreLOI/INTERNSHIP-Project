// Fill out your copyright notice in the Description page of Project Settings.


#include "WeArtStatusTracker.h"

#include "WeArtSettings.h"

// Sets default values
AWeArtStatusTracker::AWeArtStatusTracker(): MiddlewareStatusDataCPP{}, WeartAppStatusDataCPP(),
                                            UnrealMiddlewareStatusData{},
                                            UnrealWeartAppStatusData{},
                                            ConnectedTdProDeviceStatusRight{},
                                            ConnectedTdProDeviceStatusLeft{}
{
	PrimaryActorTick.bCanEverTick = true;

	ConnectedDeviceStatusRight.handSide = EHandSide::Right;
	ConnectedDeviceStatusLeft.handSide = EHandSide::Left;
	ConnectedTdProDeviceStatusRight.HandSide = EHandSide::Right;
	ConnectedTdProDeviceStatusLeft.HandSide = EHandSide::Left;
}

void AWeArtStatusTracker::BeginPlay()
{
	Super::BeginPlay();
	
}

void AWeArtStatusTracker::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (isInitiated == false)
	{
		isInitiated = true;
		OnDevicesSignature.Broadcast();
	}

}

void AWeArtStatusTracker::SetMiddlewareStatus(const MiddlewareStatusData& statusData)
{
	MiddlewareStatusDataCPP = statusData;

	UnrealMiddlewareStatusData.timestamp = FString::Printf(TEXT("%lld"), statusData.timestamp);
	UnrealMiddlewareStatusData.status = statusData.status.c_str();
	UnrealMiddlewareStatusData.version = statusData.version.c_str();

	UnrealMiddlewareStatusData.statusCode = statusData.statusCode == 0 ? "OK" : FString::FromInt(statusData.statusCode);
	UnrealMiddlewareStatusData.errorDesc = statusData.statusCode == 0 ? "" : statusData.errorDesc.c_str();
	UnrealMiddlewareStatusData.actuationsEnabled = statusData.actuationsEnabled;
	UnrealMiddlewareStatusData.connectedDevices.Empty();
	
	for (size_t i = 0; i < statusData.connectedDevices.size(); i++)
	{
		FMiddlewareConnectedDevice device;
		device.macAddress = statusData.connectedDevices[i].macAddress.c_str();
		device.handSide = statusData.connectedDevices[i].handSide;
		UnrealMiddlewareStatusData.connectedDevices.Add(device);
	}
}

void AWeArtStatusTracker::SetWeartAppData(const WeartAppStatusData& statusData)
{
	WeartAppStatusDataCPP = statusData;
	UnrealWeartAppStatusData.timestamp = FString::Printf(TEXT("%lld"), statusData.timestamp);
	UnrealWeartAppStatusData.status = statusData.status.c_str();
	UnrealWeartAppStatusData.version = statusData.version.c_str();

	UnrealWeartAppStatusData.statusCode = statusData.statusCode == 0 ? "OK" :FString::FromInt(statusData.statusCode);
	UnrealWeartAppStatusData.errorDesc = statusData.statusCode == 0 ? "" : statusData.errorDesc.c_str();
	UnrealWeartAppStatusData.actuationsEnabled = statusData.actuationsEnabled;
	
	UnrealWeartAppStatusData.ConnectionType = statusData.connectionType.c_str();
	UnrealWeartAppStatusData.AutoConnection = statusData.autoconnection;
	UnrealWeartAppStatusData.DeviceSelection =	statusData.deviceSelection.c_str();
	UnrealWeartAppStatusData.TrackingPlayback = statusData.trackingPlayback;
	UnrealWeartAppStatusData.RawDataLog = statusData.rawDataLog;
	UnrealWeartAppStatusData.SensorOnMask = statusData.sensorOnMask;
}

void AWeArtStatusTracker::SetConnectedDevicesStatus(const std::vector<ConnectedDeviceStatus>& devices)
{
	for (size_t i = 0; i < devices.size(); i++)
	{
		devices[i].handSide == EHandSide::Left
			? InitG1Device(ConnectedDeviceStatusLeft, devices, i)
			: InitG1Device(ConnectedDeviceStatusRight, devices, i);
	}
}

void AWeArtStatusTracker::InitG1Device(FConnectedDeviceStatus& DeviceStatus,
	const std::vector<ConnectedDeviceStatus>& Devices, int32 DeviceIndex)
{
	DeviceStatus.batteryLevel = Devices[DeviceIndex].batteryLevel;
	DeviceStatus.charging = Devices[DeviceIndex].charging;
	DeviceStatus.handSide = Devices[DeviceIndex].handSide;
	DeviceStatus.macAddress = Devices[DeviceIndex].macAddress.c_str();
	DeviceStatus.thimbles.Empty();

	for (int32 j = 0; j < Devices[DeviceIndex].thimbles.size() ; j++)
	{
		FThimbleStatus thimble;
		thimble.connected = Devices[DeviceIndex].thimbles[j].connected;
		thimble.errorDesc = Devices[DeviceIndex].thimbles[j].errorDesc.c_str();
		thimble.id = Devices[DeviceIndex].thimbles[j].id;
		thimble.statusCode = Devices[DeviceIndex].thimbles[j].statusCode;
		DeviceStatus.thimbles.Add(thimble);
	}
}

void AWeArtStatusTracker::InitTdProDevice(FConnectedTdProDeviceStatus& DeviceStatus, const std::vector<TdProConnectedDeviceStatus>& Devices, int32 DeviceIndex)
{
	DeviceStatus.Nodes.Empty();

	UnrealWeartAppStatusData.connectedDevices.Empty();
	
	FMiddlewareConnectedDevice device;
	device.macAddress = Devices[DeviceIndex].macAddress.c_str();
	device.handSide = Devices[DeviceIndex].handSide;
	UnrealWeartAppStatusData.connectedDevices.Add(device);
	
	DeviceStatus.MacAddress = Devices[DeviceIndex].macAddress.c_str();
	DeviceStatus.HandSide = Devices[DeviceIndex].handSide;
	
	DeviceStatus.BatteryLevel = Devices[DeviceIndex].master.batteryLevel;
	DeviceStatus.Charging = Devices[DeviceIndex].master.charging;
	DeviceStatus.ChargeCompleted = Devices[DeviceIndex].master.charge_completed;
	DeviceStatus.ImuFault = Devices[DeviceIndex].master.imu_fault;
	DeviceStatus.AdcFault = Devices[DeviceIndex].master.adc_fault;
	DeviceStatus.StatusError = Devices[DeviceIndex].master.status_error;
	DeviceStatus.ButtonPushed = Devices[DeviceIndex].master.button_pushed;

	DeviceStatus.UsbConnected = Devices[DeviceIndex].master.connection.usb_connected; 
	DeviceStatus.WifiConnected = Devices[DeviceIndex].master.connection.wifi_connected;
	DeviceStatus.WifiOn = Devices[DeviceIndex].master.connection.wifi_on;
	DeviceStatus.BleConnected = Devices[DeviceIndex].master.connection.ble_connected;
	DeviceStatus.BleOn = Devices[DeviceIndex].master.connection.ble_on;
			
	for (int32 j = 0; j < Devices[DeviceIndex].nodes.size() ; j++)
	{
		FTdProNode node;
		node.id = Devices[DeviceIndex].nodes[j].id.c_str();
		node.connected = Devices[DeviceIndex].nodes[j].connected;
		node.imu_fault = Devices[DeviceIndex].nodes[j].imu_fault;
		node.tof_fault = Devices[DeviceIndex].nodes[j].tof_fault;
		DeviceStatus.Nodes.Add(node);
	}
}

void AWeArtStatusTracker::SetConnectedTdProDevicesStatus(const std::vector<TdProConnectedDeviceStatus>& devices)
{
	for (size_t i = 0; i < devices.size(); i++)
	{
		devices[i].handSide == EHandSide::Left
			?	InitTdProDevice(ConnectedTdProDeviceStatusLeft, devices, i)
			:	InitTdProDevice(ConnectedTdProDeviceStatusRight, devices, i);
	}
}

