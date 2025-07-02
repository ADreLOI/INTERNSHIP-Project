#pragma once

#include "WeArtCommon.h"
#include <string>
#include <vector>
#include <cassert>
#include "json.hpp"

/**
 * @brief Converts a string representation of hand side to EHandSide enum.
 * @param str The string representation of hand side.
 * @return The corresponding EHandSide enum value.
 */
static EHandSide StringToHandside(std::string &str) {
	if (str == "LEFT") {
		return EHandSide::Left;
	}
	if (str == "RIGHT") {
		return EHandSide::Right;
	}
	assert(false);
	return EHandSide::Left;
};

/**
 * @brief Converts EHandSide enum to its string representation.
 * @param hs The EHandSide enum value.
 * @return The string representation of the hand side.
 */
static std::string HandsideToString(EHandSide hs) {
	if (hs == EHandSide::Left) {
		return "LEFT";
	}
	if (hs == EHandSide::Right) {
		return "RIGHT";
	}
	assert(false);
	return "";
};

/**
 * @brief Converts a string representation of actuation point to EActuationPoint enum.
 * @param str The string representation of actuation point.
 * @return The corresponding EActuationPoint enum value.
 */
static EActuationPoint StringToActuationPoint(std::string& str) {
	if (str == "THUMB") {
		return EActuationPoint::Thumb;
	}
	if (str == "INDEX") {
		return EActuationPoint::Index;
	}
	if (str == "MIDDLE") {
		return EActuationPoint::Middle;
	}
	if (str == "PALM") {
		return EActuationPoint::Palm;
	}
	if (str == "ANNULAR") {
		return EActuationPoint::Annular;
	}
	if (str == "PINKY") {
		return EActuationPoint::Pinky;
	}
	assert(false);
	return EActuationPoint::Thumb;
};

/**
 * @brief Converts EActuationPoint enum to its string representation.
 * @param ap The EActuationPoint enum value.
 * @return The string representation of the actuation point.
 */
static std::string ActuationPointToString(EActuationPoint ap) {
	if (ap == EActuationPoint::Thumb) {
		return "THUMB";
	}
	if (ap == EActuationPoint::Index) {
		return "INDEX";
	}
	if (ap == EActuationPoint::Middle) {
		return "MIDDLE";
	}
	if (ap == EActuationPoint::Palm) {
		return "PALM";
	}
	if (ap == EActuationPoint::Annular) {
		return "ANNULAR";
	}
	if (ap == EActuationPoint::Pinky) {
		return "PINKY";
	}
	assert(false);
	return "";
};

/**
* @brief Converts a string representation of tracking type to TrackingType enum.
* @param str The string representation of tracking type.
* @return The corresponding TrackingType enum value.
*/
static TrackingType StringToTrackingType(const std::string& str) {
	if (str == "TrackType1")
		return TrackingType::WEART_HAND;
	return TrackingType::DEFAULT;
}

/**
 * @brief Converts TrackingType enum to its string representation.
 * @param trackType The TrackingType enum value.
 * @return The string representation of the tracking type.
 */
static std::string TrackingTypeToString(TrackingType trackType) {
	switch (trackType) {
	case TrackingType::DEFAULT:
		return "";
	case TrackingType::WEART_HAND:
		return "TrackType1";
	}
	return "";
}

/**
 * @brief Converts MiddlewareStatus enum to its string representation.
 * @param status The MiddlewareStatus enum value.
 * @return The string representation of the middleware status.
 */
static std::string MiddlewareStatusToString(MiddlewareStatus status)
{
	switch (status)
	{
	case MiddlewareStatus::DISCONNECTED:
		return"DISCONNECTED";
	case MiddlewareStatus::IDLE:
		return"IDLE";
	case MiddlewareStatus::STARTING:
		return"STARTING";
	case MiddlewareStatus::RUNNING:
		return"RUNNING";
	case MiddlewareStatus::STOPPING:
		return"STOPPING";
	case MiddlewareStatus::UPLOADING_TEXTURES:
		return"UPLOADING_TEXTURES";
	case MiddlewareStatus::CONNECTING_DEVICE:
		return"CONNECTING_DEVICE";
	case MiddlewareStatus::CALIBRATION:
		return"CALIBRATION";
	}

	return "";
}

/**
 * @brief Converts a string representation of middleware status to MiddlewareStatus enum.
 * @param str The string representation of middleware status.
 * @return The corresponding MiddlewareStatus enum value.
 */
static MiddlewareStatus StringToMiddlewareStatus(const std::string& str) {
	if (str == "DISCONNECTED")
		return MiddlewareStatus::DISCONNECTED;
	if (str == "IDLE")
		return MiddlewareStatus::IDLE;
	if (str == "STARTING")
		return MiddlewareStatus::STARTING;
	if (str == "RUNNING")
		return MiddlewareStatus::RUNNING;
	if (str == "STOPPING")
		return MiddlewareStatus::STOPPING;
	if (str == "UPLOADING_TEXTURES")
		return MiddlewareStatus::UPLOADING_TEXTURES;
	if (str == "CONNECTING_DEVICE")
		return MiddlewareStatus::CONNECTING_DEVICE;
	if (str == "CALIBRATION")
		return MiddlewareStatus::CALIBRATION;

	return MiddlewareStatus::DISCONNECTED;
}

/// <summary>
/// Generic Weart message
/// </summary>
class WeArtMessage {

public:
	virtual ~WeArtMessage() = default;
	virtual std::string getID() = 0;
	//virtual std::vector<std::string> getValues() = 0;
	//virtual void setValues(std::vector<std::string>& values) = 0;

	virtual void setHandSide(EHandSide hs) = 0;
	virtual void setActuationPoint(EActuationPoint ap) = 0;

	virtual std::string serialize() = 0;
	virtual void deserialize(std::string message) = 0;
};

//! @private
class WeArtCsvMessage : public WeArtMessage {
public:
	const char field_separator = ':';

	virtual std::vector<std::string> getValues() = 0;
	virtual void setValues(std::vector<std::string>& values) = 0;

	virtual std::string serialize() override;
	virtual void deserialize(std::string message) override;
};

//! @private
class WeArtJsonMessage : public WeArtMessage {
public:
	WeArtJsonMessage();

	virtual std::string serialize() override;
	std::uint64_t timestamp() { return _timestamp; }
	virtual void deserialize(std::string message) override;

	virtual void setHandSide(EHandSide hs) override {};
	virtual void setActuationPoint(EActuationPoint ap) override {};

protected:
	virtual nlohmann::json serializePayload() { return nlohmann::json(); }
	virtual void deserializePayload(nlohmann::json payload) {}
	std::uint64_t _timestamp;
};

/// <summary>
/// Message without handside or actuation point parameters
/// </summary>
class WeArtMessageNoParams : public WeArtCsvMessage {

public:
	virtual void setHandSide(EHandSide hs) override {};
	virtual void setActuationPoint(EActuationPoint ap) override {};

	// This message carries no values.
	virtual std::vector<std::string> getValues() override { return std::vector<std::string>(); };
	virtual void setValues(std::vector<std::string>& values) override {};
};

/// <summary>
/// Message without handside or actuation point parameters
/// </summary>
class WeArtMessageObjectSpecific : public WeArtCsvMessage {

protected:
	EHandSide handSide;
	EActuationPoint actuationPoint;
public:
	virtual void setHandSide(EHandSide hs) override { handSide = hs; };
	virtual void setActuationPoint(EActuationPoint ap) override { actuationPoint = ap; };
};

/// <summary>
/// Message to middleware to start the connection
/// </summary>
class StartFromClientMessage : public WeArtMessageNoParams {

public:

	static constexpr const char* ID = "StartFromClient";
	
	virtual std::string getID() override { return ID; };

	virtual std::vector<std::string> getValues() override {
		std::vector<std::string> ret;
		if (_trackType != TrackingType::DEFAULT) {
			ret.push_back(WeArtConstants::WEART_SDK_TYPE);
			ret.push_back(WeArtConstants::WEART_SDK_VERSION);
			ret.push_back(WeArtConstants::WEART_SDK_TRACK_TYPE);
		}
		return ret;
	};

	virtual void setValues(std::vector<std::string>& values) override {
		if (values.empty())
			_trackType = TrackingType::DEFAULT;
		else
			_trackType = StringToTrackingType(values[2]);
	};

private:
	TrackingType _trackType;
};

/// <summary>
/// Message to middleware to stop the connection
/// </summary>
class StopFromClientMessage : public WeArtMessageNoParams {

public:
	static constexpr const char* ID = "StopFromClient";

	virtual std::string getID() override { return ID; };

};

/// <summary>
/// Message to the middleware to start the calibration
/// </summary>
class StartCalibrationMessage : public WeArtMessageNoParams {

public:
	static constexpr const char* ID = "StartCalibration";

	virtual std::string getID() override { return ID; };
};

/// <summary>
/// Message to the middleware to stop the calibration
/// </summary>
class StopCalibrationMessage : public WeArtMessageNoParams {

public:
	static constexpr const char* ID = "StopCalibration";

	virtual std::string getID() override { return ID; };
};

/// <summary>
/// Message to the middleware to exit the connection
/// </summary>
class ExitMessage : public WeArtMessageNoParams {

public:
	static constexpr const char* ID = "exit";

	virtual std::string getID() override { return ID; };
};

/// <summary>
/// Message to the middleware to disconnect
/// </summary>
class DisconnectMessage : public WeArtMessageNoParams {

public:
	static constexpr const char* ID = "disconnect";

	virtual std::string getID() override { return ID; };
};

/// <summary>
/// Message to the middleware to set the temperature of the effect
/// </summary>
class SetTemperatureMessage : public WeArtMessageObjectSpecific
{
public:
	SetTemperatureMessage(const float t) : temperature(t) {};
	SetTemperatureMessage() {};

	static constexpr const char* ID = "temperature";

	virtual std::string getID() override { return ID; };

	virtual std::vector<std::string> getValues() override {
		std::vector<std::string> ret;
		ret.push_back(std::to_string(temperature));
		ret.push_back(HandsideToString(handSide));
		ret.push_back(ActuationPointToString(actuationPoint));
		return ret;
	};

	virtual void setValues(std::vector<std::string>& values) override {
		assert(values.size() == 3);
		temperature = std::stof(values[0]);
		handSide = StringToHandside(values[1]);
		actuationPoint = StringToActuationPoint(values[2]);
	};

protected:

	float temperature;
};

/// <summary>
/// Message to the middleware to stop the temperature of the effect
/// </summary>
class StopTemperatureMessage : public WeArtMessageObjectSpecific
{

public:

	static constexpr const char* ID = "stopTemperature";

	virtual std::string getID() override { return ID; };

	virtual std::vector<std::string> getValues() override {
		std::vector<std::string> ret;
		ret.push_back(HandsideToString(handSide));
		ret.push_back(ActuationPointToString(actuationPoint));
		return ret;
	};

	virtual void setValues(std::vector<std::string>& values) override {
		assert(values.size() == 2);
		handSide = StringToHandside(values[0]);
		actuationPoint = StringToActuationPoint(values[1]);
	};
};

/// <summary>
/// Message to the middleware to set the force of the effect
/// </summary>
class SetForceMessage : public WeArtMessageObjectSpecific
{

public:
	SetForceMessage(const float f[3]) : force{ f[0], f[1], f[2] } {};
	SetForceMessage() {};

	static constexpr const char* ID = "force";

	virtual std::string getID() override { return ID; };

	virtual std::vector<std::string> getValues() override {
		std::vector<std::string> ret;
		ret.push_back(std::to_string(force[0]));
		ret.push_back(std::to_string(force[1]));
		ret.push_back(std::to_string(force[2]));
		ret.push_back(HandsideToString(handSide));
		ret.push_back(ActuationPointToString(actuationPoint));
		return ret;
	};

	virtual void setValues(std::vector<std::string>& values) override {
		assert(values.size() == 5);
		force[0] = std::stof(values[0]);
		force[1] = std::stof(values[1]);
		force[2] = std::stof(values[2]);
		handSide = StringToHandside(values[3]);
		actuationPoint = StringToActuationPoint(values[4]);
	};

protected:

	float force[3];
};

/// <summary>
/// Message to the middleware to stop the temperature of the effect
/// </summary>
class StopForceMessage : public WeArtMessageObjectSpecific
{

public:

	static constexpr const char* ID = "stopForce";

	virtual std::string getID() override { return ID; };

	virtual std::vector<std::string> getValues() override {
		std::vector<std::string> ret;
		ret.push_back(HandsideToString(handSide));
		ret.push_back(ActuationPointToString(actuationPoint));
		return ret;
	};

	virtual void setValues(std::vector<std::string>& values) override {
		assert(values.size() == 2);
		handSide = StringToHandside(values[0]);
		actuationPoint = StringToActuationPoint(values[1]);
	};
};

/// <summary>
/// Message to the middleware to set the texture of the effect
/// </summary>
class SetTextureMessage : public WeArtMessageObjectSpecific
{

public:
	SetTextureMessage(const int idx, const float vel[3], const float vol) : index(idx), velocity{ vel[0], vel[1], vel[2] }, volume(vol)  {};
	SetTextureMessage() {};

	static constexpr const char* ID = "texture";

	virtual std::string getID() override { return ID; };

	virtual std::vector<std::string> getValues() override {
		std::vector<std::string> ret;

		// avoinding texture id out of bounds
		if (index < WeArtConstants::minTextureIndex || index > WeArtConstants::maxTextureIndex) {
			index = WeArtConstants::nullTextureIndex;
		}

		ret.push_back(std::to_string(index));
		ret.push_back(std::to_string(velocity[0]));
		ret.push_back(std::to_string(velocity[1]));
		ret.push_back(std::to_string(velocity[2]));
		ret.push_back(std::to_string(volume));
		ret.push_back(HandsideToString(handSide));
		ret.push_back(ActuationPointToString(actuationPoint));
		return ret;
	};

	virtual void setValues(std::vector<std::string>& values) override {
		assert(values.size() == 6);
		index = std::stoi(values[0]);
		velocity[0] = std::stof(values[1]);
		velocity[1] = std::stof(values[2]);
		velocity[2] = std::stof(values[3]);
		volume = std::stof(values[4]);
		handSide = StringToHandside(values[5]);
		actuationPoint = StringToActuationPoint(values[6]);
	};

protected:

	int index;
	float velocity[3];
	float volume;
};

/// <summary>
/// Message to the middleware to stop the texture of the effect
/// </summary>
class StopTextureMessage : public WeArtMessageObjectSpecific
{

public:

	static constexpr const char* ID = "stopTexture";

	virtual std::string getID() override { return ID; };

	virtual std::vector<std::string> getValues() override {
		std::vector<std::string> ret;
		ret.push_back(HandsideToString(handSide));
		ret.push_back(ActuationPointToString(actuationPoint));
		return ret;
	};

	virtual void setValues(std::vector<std::string>& values) override {
		assert(values.size() == 2);
		handSide = StringToHandside(values[0]);
		actuationPoint = StringToActuationPoint(values[1]);
	};
};

//! @private 
class IFingerDataContainable
{
public:
	virtual ~IFingerDataContainable() = default;

	virtual float GetClosure(EHandSide HandSide, EActuationPoint actuationPoint) const = 0;
	virtual float GetAbduction(EHandSide HandSide, EActuationPoint actuationPoint) const = 0;
};

/// <summary>
/// Generic Tracking message, contains information on closure and abduction (based on tracking type)
/// </summary>
class TrackingMessage : public WeArtMessageNoParams, public IFingerDataContainable
{
	uint8 RightThumbClosure;
	uint8 RightIndexClosure;
	uint8 RightMiddleClosure;
	uint8 RightPalmClosure;
	uint8 RightAnnularClosure;
	uint8 RightPinkyClosure;
	
	uint8 RightThumbAbduction;
	uint8 RightIndexAbduction;
	uint8 RightMiddleAbduction;
	uint8 RightPalmAbduction;
	uint8 RightAnnularAbduction;
	uint8 RightPinkyAbduction;
	
	uint8 LeftThumbClosure;
	uint8 LeftIndexClosure;
	uint8 LeftMiddleClosure;
	uint8 LeftPalmClosure;
	uint8 LeftAnnularClosure;
	uint8 LeftPinkyClosure;
	
	uint8 LeftThumbAbduction;
	uint8 LeftIndexAbduction;
	uint8 LeftMiddleAbduction;
	uint8 LeftPalmAbduction;
	uint8 LeftAnnularAbduction;
	uint8 LeftPinkyAbduction;

	FQuat RightWristRotation;
	FQuat LeftWristRotation;

	TrackingType _trackingType;
	
public:

	static constexpr const char* ID = "Tracking";

	TrackingMessage()
		: RightThumbClosure(0), RightIndexClosure(0), RightMiddleClosure(0), RightPalmClosure(0),
		  RightAnnularClosure(0),
		  RightPinkyClosure(0),
		  RightThumbAbduction(0),
		  RightIndexAbduction(0),
		  RightMiddleAbduction(0),
		  RightPalmAbduction(0),
		  RightAnnularAbduction(0),
		  RightPinkyAbduction(0),
		  LeftThumbClosure(0),
		  LeftIndexClosure(0),
		  LeftMiddleClosure(0), LeftPalmClosure(0),
		  LeftAnnularClosure(0),
		  LeftPinkyClosure(0),
		  LeftThumbAbduction(0),
		  LeftIndexAbduction(0),
		  LeftMiddleAbduction(0),
		  LeftPalmAbduction(0),
		  LeftAnnularAbduction(0),
		  LeftPinkyAbduction(0),
		  _trackingType(TrackingType::WEART_HAND)
	{
	}

	virtual std::string getID() override { return ID; };

	struct Angles {
		float X;
		float Y;
		float Z;
	};

	virtual std::vector<std::string> getValues() override {
		std::vector<std::string> ret;
		ret.push_back(std::to_string(RightThumbClosure));
		ret.push_back(std::to_string(RightIndexClosure));
		ret.push_back(std::to_string(RightMiddleClosure));
		ret.push_back(std::to_string(RightPalmClosure));
		ret.push_back(std::to_string(LeftThumbClosure));
		ret.push_back(std::to_string(LeftIndexClosure));
		ret.push_back(std::to_string(LeftMiddleClosure));
		ret.push_back(std::to_string(LeftPalmClosure));
		return ret;
	};

	virtual void setValues(std::vector<std::string>& values) override {
		_trackingType = StringToTrackingType(values[0]);
		switch (_trackingType)
		{
			case TrackingType::DEFAULT:
			{
				assert(values.size() == 8);
				RightThumbClosure = std::stoi(values[0]);
				RightIndexClosure = std::stoi(values[1]);
				RightMiddleClosure = std::stoi(values[2]);
				RightPalmClosure = std::stoi(values[3]);
				LeftThumbClosure = std::stoi(values[4]);
				LeftIndexClosure = std::stoi(values[5]);
				LeftMiddleClosure = std::stoi(values[6]);
				LeftPalmClosure = std::stoi(values[7]);
				break;
			}
			case TrackingType::WEART_HAND:
			{
				assert(values.size() == 9);
				// Right
				RightIndexClosure = std::stoi(values[1]);
				RightThumbClosure = std::stoi(values[2]);
				RightThumbAbduction = std::stoi(values[3]);
				RightMiddleClosure = std::stoi(values[4]);
	
				// Left
				LeftIndexClosure = std::stoi(values[5]);
				LeftThumbClosure = std::stoi(values[6]);
				LeftThumbAbduction = std::stoi(values[7]);
				LeftMiddleClosure = std::stoi(values[8]);
				
			}
		}
	};

	virtual float GetClosure(EHandSide handSide, EActuationPoint actuationPoint) const override;
	virtual float GetAbduction(EHandSide handSide, EActuationPoint actuationPoint) const override;
};

//! @private
class FTdProTrackingMessage : public WeArtJsonMessage, public IFingerDataContainable
{
	static float RightThumbClosure;
	static float RightIndexClosure;
	static float RightMiddleClosure;
	static float RightPalmClosure;
	static float RightAnnularClosure;
	static float RightPinkyClosure;
	
	static float RightThumbAbduction;
	static float RightIndexAbduction;
	static float RightMiddleAbduction;
	static float RightPalmAbduction;
	static float RightAnnularAbduction;
	static float RightPinkyAbduction;
	
	static float LeftThumbClosure;
	static float LeftIndexClosure;
	static float LeftMiddleClosure;
	static float LeftPalmClosure;
	static float LeftAnnularClosure;
	static float LeftPinkyClosure;
	
	static float LeftThumbAbduction;
	static float LeftIndexAbduction;
	static float LeftMiddleAbduction;
	static float LeftPalmAbduction;
	static float LeftAnnularAbduction;
	static float LeftPinkyAbduction;

	static FQuat RightWristRotation;
	static FQuat LeftWristRotation;
	
public:
	static constexpr const char* ID = "TRACKING_BENDING_G2";

	virtual std::string getID() override { return ID; }
	virtual std::string serialize() override;
	
	virtual void deserialize(std::string message) override;
	virtual void setHandSide(EHandSide hs) override;
	virtual void setActuationPoint(EActuationPoint ap) override;

protected:
	virtual nlohmann::json serializePayload() override;
	virtual void deserializePayload(nlohmann::json payload) override;

private:
	virtual float GetClosure(EHandSide HandSide, EActuationPoint actuationPoint) const override;
	virtual float GetAbduction(EHandSide HandSide, EActuationPoint actuationPoint) const override;
};


//! @private
class RawDataOn : public WeArtJsonMessage {
public:
	static constexpr const char* ID = "RAW_DATA_ON";

	virtual std::string getID() override { return ID; }
	virtual void setHandSide(EHandSide hs) override {}
	virtual void setActuationPoint(EActuationPoint ap) override {}
};

//! @private
class RawDataOff : public WeArtJsonMessage {
public:
	static constexpr const char* ID = "RAW_DATA_OFF";

	virtual std::string getID() override { return ID; }
	virtual void setHandSide(EHandSide hs) override {}
	virtual void setActuationPoint(EActuationPoint ap) override {}
};

//! @private
class RawSensorsData : public WeArtJsonMessage {
public:
	static constexpr const char* ID = "RAW_DATA";

	virtual std::string getID() override { return ID; };
	virtual void setHandSide(EHandSide hs) override { hand = hs; }
	virtual void setActuationPoint(EActuationPoint ap) override {}

	EHandSide getHand() { return hand; }
	bool hasSensor(EActuationPoint ap);
	SensorData getSensor(EActuationPoint ap);

protected:
	virtual nlohmann::json serializePayload() override;
	virtual void deserializePayload(nlohmann::json payload) override;

	EHandSide hand;
	std::map<EActuationPoint, SensorData> sensors;
};

//! @private 
class TdProRawData : public RawSensorsData {
public:
	TdProRawData() : RawSensorsData() {}

	static constexpr const char* ID = "RAW_DATA_TD_PRO";
	virtual std::string getID() override { return ID; };

protected:
	virtual void deserializePayload(nlohmann::json payload) override;
};


//!@private
class GetMiddlewareStatus : public WeArtJsonMessage {
public:
	GetMiddlewareStatus() : WeArtJsonMessage() {}

	static constexpr const char* ID = "MW_GET_STATUS";
	virtual std::string getID() override { return ID; };
};

//!@private
class FMiddlewareStatusMessage final : public WeArtJsonMessage {
public:
	static constexpr const char* ID = "MW_STATUS";
	FMiddlewareStatusMessage() = default;
	virtual std::string getID() override { return ID; };

	MiddlewareStatusData data() { return _data; }

protected:
	virtual nlohmann::json serializePayload() override;
	virtual void deserializePayload(nlohmann::json payload) override;

private:
	MiddlewareStatusData _data;
};

//! @private 
class TdProWeArtAppStatus : public WeArtJsonMessage {
public:
	static constexpr const char* ID = "WA_STATUS";
	virtual std::string getID() override { return ID; };

	WeartAppStatusData data() { return _data; }
	
	TdProWeArtAppStatus() : WeArtJsonMessage(), _data()
	{
	}

protected:
	virtual nlohmann::json serializePayload() override;
	virtual void deserializePayload(nlohmann::json payload) override;
	
private:
	WeartAppStatusData _data;
};

//! @private 
class GetDevicesStatusMessage : public WeArtJsonMessage {
public:
	GetDevicesStatusMessage() : WeArtJsonMessage() {}
	static constexpr const char* ID = "DEVICES_GET_STATUS";
	virtual std::string getID() override { return ID; };
};

//! @private 
class DevicesStatusMessage : public WeArtJsonMessage {
public:
	DevicesStatusMessage() : WeArtJsonMessage() {}

	static constexpr const char* ID = "DEVICES_STATUS";
	virtual std::string getID() override { return ID; };

	virtual std::vector<ConnectedDeviceStatus> devices() { return _devices; }

protected:
	virtual nlohmann::json serializePayload() override;
	virtual void deserializePayload(nlohmann::json payload) override;

private:
	std::vector<ConnectedDeviceStatus> _devices;
};

//! @private 
class TdProDevicesStatusMessage : public WeArtJsonMessage {
public:
	TdProDevicesStatusMessage() : WeArtJsonMessage() {}

	static constexpr const char* ID = "WEART_TD_PRO_STATUS";
	std::vector<TdProConnectedDeviceStatus> getTdProDevices() { return _devices; }
	virtual std::string getID() override { return ID; };

protected:
	virtual nlohmann::json serializePayload() override;
	virtual void deserializePayload(nlohmann::json payload) override;

private:
	std::vector<TdProConnectedDeviceStatus> _devices;
};




