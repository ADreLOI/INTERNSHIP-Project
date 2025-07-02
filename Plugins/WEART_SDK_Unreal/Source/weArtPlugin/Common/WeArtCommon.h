#pragma once
#include <vector>
#include <string>
#include "json.hpp"

/*
 * Remember to keep naming for json structs same as field names in messages, if you're using nlohmann macroses
 */

UENUM(BlueprintType, meta = (Bitflags, UseEnumValuesAsMaskValuesInEditor = "true"))
enum EHandSide
{
	HSnone = 0			UMETA(Hidden),
	Left	= 1 << 0    UMETA(DisplayName = "Left")	,
	Right	= 1 << 1    UMETA(DisplayName = "Right"),
};
ENUM_RANGE_BY_VALUES(EHandSide, EHandSide::Left, EHandSide::Right);
NLOHMANN_JSON_SERIALIZE_ENUM(EHandSide, {
	{EHandSide::Left, "LEFT"},
	{EHandSide::Right, "RIGHT"},
	})

UENUM(BlueprintType, meta = (Bitflags, UseEnumValuesAsMaskValuesInEditor = "true"))
enum EActuationPoint
{
	APnone	= 0			UMETA(Hidden),
	Thumb	= 1 << 0	UMETA(DisplayName = "Thumb"),
	Index	= 1 << 1	UMETA(DisplayName = "Index"),
	Middle	= 1 << 2	UMETA(DisplayName = "Middle"),
	Palm	= 1 << 3	UMETA(DisplayName = "Palm"),
	Annular	= 1 << 4	UMETA(DisplayName = "Annular"),
	Pinky	= 1 << 5	UMETA(DisplayName = "Pinky"),
};
ENUM_RANGE_BY_VALUES(EActuationPoint, EActuationPoint::Thumb, EActuationPoint::Index, EActuationPoint::Middle, EActuationPoint::Palm, EActuationPoint::Annular, EActuationPoint::Pinky);

NLOHMANN_JSON_SERIALIZE_ENUM(EActuationPoint, {
	{EActuationPoint::Thumb, "THUMB"},
	{EActuationPoint::Index, "INDEX"},
	{EActuationPoint::Middle, "MIDDLE"},
	{EActuationPoint::Palm, "PALM"},
	{EActuationPoint::Annular, "ANNULAR"},
	{EActuationPoint::Pinky, "PINKY"}
	})	

enum class TrackingType{
	DEFAULT,	//!< Deprecated, contains only closure values 
	WEART_HAND	//!< Tracking with closures, and abduction value for thumb
};

enum HandClosingState
{
	Open = 0,
	Closing = 1,
	Closed = 2
};

UENUM(BlueprintType)
enum class GraspingState : uint8
{
	Grabbed = 0,
	Released = 1
};

UENUM(meta = (Bitflags, UseEnumValuesAsMaskValuesInEditor = "true"))
enum class MiddlewareStatus {
	DISCONNECTED,
	IDLE,
	STARTING,
	RUNNING,
	STOPPING,
	UPLOADING_TEXTURES,
	CONNECTING_DEVICE,
	CALIBRATION,
};

NLOHMANN_JSON_SERIALIZE_ENUM(MiddlewareStatus, {
	{MiddlewareStatus::DISCONNECTED, "DISCONNECTED"},
	{MiddlewareStatus::IDLE, "IDLE"},
	{MiddlewareStatus::STARTING, "STARTING"},
	{MiddlewareStatus::RUNNING, "RUNNING"},
	{MiddlewareStatus::STOPPING, "STOPPING"},
	{MiddlewareStatus::UPLOADING_TEXTURES, "UPLOADING_TEXTURES"},
	{MiddlewareStatus::CONNECTING_DEVICE, "CONNECTING_DEVICE"},
	{MiddlewareStatus::CALIBRATION, "CALIBRATION"},
	})

struct AccelerometerData {
	float x;
	float y;
	float z;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(AccelerometerData, x, y, z)

struct GyroscopeData {
	float x;
	float y;
	float z;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(GyroscopeData, x, y, z)

struct TofData {
	int distance = 0;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(TofData, distance)

struct SensorData {
	AccelerometerData accelerometer;
	GyroscopeData gyroscope;
	TofData timeOfFlight;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(SensorData, accelerometer, gyroscope, timeOfFlight)

struct MiddlewareConnectedDevice {
	std::string macAddress;
	EHandSide handSide;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(MiddlewareConnectedDevice, macAddress, handSide)

struct MiddlewareStatusData {
	std::uint64_t timestamp;
	std::string status;
	std::string version;
	int statusCode;
	std::string errorDesc;
	bool actuationsEnabled;
	std::vector<MiddlewareConnectedDevice> connectedDevices;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(MiddlewareStatusData, status, version, statusCode, errorDesc, actuationsEnabled, connectedDevices);

struct WeartAppStatusData : public MiddlewareStatusData {
	std::string connectionType;
	bool autoconnection;
	std::string deviceSelection;
	bool trackingPlayback;
	bool rawDataLog;
	bool sensorOnMask;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(WeartAppStatusData, status, version, statusCode, errorDesc, actuationsEnabled, connectionType, autoconnection, deviceSelection, trackingPlayback, rawDataLog, sensorOnMask, connectedDevices);


//! @brief Status of a single thimble connected to a device
struct ThimbleStatus {
	//! @brief Actuation Point to which the thimble is assigned
	EActuationPoint id;
	//! @brief Tells whether the thimble is connected to the device or not
	bool connected;
	//! @brief Current status code of the thimble (0 = OK)
	int statusCode;
	//! @brief Description of the thimble status code
	std::string errorDesc;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(ThimbleStatus, id, connected, statusCode, errorDesc);

//! @brief Status of a connected TouchDIVER device
struct ConnectedDeviceStatus {
	//! @brief Device BLE mac address
	std::string macAddress;

	//! @brief Hand to which the device is assigned
	EHandSide handSide;

	//! @brief Battery charge level (from 0 to 100)
	int batteryLevel;

	//! @brief Tells whether the device is under charge (true) or not (false)
	bool charging;

	//! @brief Status of the device thimbles
	std::vector<ThimbleStatus> thimbles;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(ConnectedDeviceStatus, macAddress, handSide, batteryLevel, charging, thimbles);

//! @brief Connection status struct
struct Connection {
	bool usb_connected;
	bool wifi_connected;
	bool wifi_on;
	bool ble_connected;
	bool ble_on;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(Connection, usb_connected, wifi_connected, wifi_on, ble_connected, ble_on);

//! @brief Master device status struct
struct Master {
	int batteryLevel;
	bool charging;
	bool charge_completed;
	Connection connection;
	bool imu_fault;
	bool adc_fault;
	bool status_error;
	bool button_pushed;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(Master, batteryLevel, charging, charge_completed, connection, imu_fault, adc_fault, status_error, button_pushed);

//! @brief Node that represents each of the actuation point
struct Node {
	std::string id;
	bool connected;
	bool imu_fault;
	bool tof_fault;
	bool biometric_fault;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(Node, id, connected, imu_fault, tof_fault, biometric_fault);


//! @brief Status of a connected TouchDIVER device
struct TdProConnectedDeviceStatus {
	//! @brief Device BLE mac address
	std::string macAddress;
	//! @brief Hand to which the device is assigned
	EHandSide handSide;
	//! @brief Data related to whole touch diver pro
	Master master;
	//! @brief Data of each finger and palm
	std::vector<Node> nodes;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(TdProConnectedDeviceStatus, macAddress, handSide, master, nodes);

UENUM()
enum class TextureType : uint8
{
	Click = 0,
	SoftClick = 1,
	DoubleClick = 2,
	FineAluminiumSlow = 3,
	FineAluminiumFast = 4,
	PlasticSlow = 5,
	ProfiledAluminiumMedium = 6,
	ProfiledAluminiumFast = 7,
	RhombAluminiumMedium = 8,
	TextileMedium = 9,
	CrushedRock = 10,
	Granite = 11,
	Wood = 12,
	Laminate = 13,
	ProfiledRubber = 14,
	VelcroHooks = 15,
	VelcroLoops = 16,
	PlasticFoil = 17,
	Leather = 18,
	Cotton = 19,
	Aluminium = 20,
	DoubleSidedTape = 21
};



// Constants shared by the WeArt components
namespace WeArtConstants
{
	const std::string WEART_SDK_TYPE = "SdkUnreal";
	const std::string WEART_SDK_VERSION = "2.1.0";
	const std::string WEART_SDK_TRACK_TYPE = "TrackType1";

	constexpr float defaultTemperature = 0.5f;
	constexpr float minTemperature = 0.0f;
	constexpr float maxTemperature = 1.0f;

	constexpr float defaultForce = 0.0f;
	constexpr float minForce = 0.0f;
	constexpr float maxForce = 1.0f;

	constexpr float defaultClosure = 0.0f;
	constexpr float minClosure = 0.0f;
	constexpr float maxClosure = 1.0f;

	constexpr float staticTextureVelocity = 0.5f;
	constexpr float minInteractionForce = 0.2f;

	constexpr int defaultTextureIndex = 0;
	constexpr int minTextureIndex = 0;
	constexpr int maxTextureIndex = 254;
	constexpr int nullTextureIndex = 255;

	constexpr float defaultTextureVelocity[3] = {0.5f, 0.0f, 0.0f};
	constexpr float minTextureVelocity[3] = {0.0f, 0.0f, 0.0f};
	constexpr float maxTextureVelocity[3] = {1.0f, 1.0f, 1.0f};

	constexpr float defaultCollisionMultiplier = 20.0f;
	constexpr float minCollisionMultiplier = 0.0f;
	constexpr float maxCollisionMultiplier = 100.0f;

	constexpr float defaultVolumeTexture = 100.0f;
	constexpr float minVolumeTexture = 0.0f;
	constexpr float maxVolumeTexture = 100.0f;

	constexpr float thresholdThumbClosure = 0.5f;
	constexpr float thresholdIndexClosure = 0.5f;
	constexpr float thresholdMiddleClosure = 0.5f;

	constexpr float defaultGraspForce = 0.3f;
	constexpr float dynamicForceSensibility = 10.0f;

	constexpr float MaxDistanceForMinStiffness = 0.06f; //6cm
	constexpr float MaxDistanceForMaxStiffness = 0.02f; //2cm
	constexpr float MaxDistanceForMinStiffnessMiddle = 0.1f; //10cm
	constexpr float MaxDistanceForMaxStiffnessMiddle = 0.025f; //2.5cm
	constexpr float MaxDistanceForMinStiffnessThumb = 0.04f; // 4cm
	constexpr float MaxDistanceForMaxStiffnessThumb = 0.01f; // 1cm
};
