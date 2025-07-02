#include "WeArtMessages.h"
#include <sstream>
#include <chrono>
// CSV message

std::string WeArtCsvMessage::serialize()
{
	// Unlike C#, C++ does not allow dynamic runtime
	// reflection, hence we have to use dedicated functions
	// to get the values of the class members.
	std::string messageID = getID();
	std::vector<std::string> serializedValues = getValues();

	// Join the string arrays.
	serializedValues.insert(serializedValues.begin(), messageID);

	// Then merge using our separator.
	std::stringstream ss;
	auto it = serializedValues.begin();
	ss << *it++;
	for (; it != serializedValues.end(); it++) {
		ss << field_separator;
		ss << *it;
	}
	return ss.str();
}

void WeArtCsvMessage::deserialize(std::string message)
{
	// Split strings
	std::vector<std::string> strings;
	std::istringstream dataStream(message);
	std::string s;
	while (std::getline(dataStream, s, field_separator)) {
		strings.push_back(s);
	}

	// Set message values
	strings.erase(strings.begin());
	setValues(strings);
}


// JSON message

float FTdProTrackingMessage::RightThumbClosure = 0.f;
float FTdProTrackingMessage::RightIndexClosure = 0.f;
float FTdProTrackingMessage::RightMiddleClosure = 0.f;
float FTdProTrackingMessage::RightPalmClosure = 0.f;
float FTdProTrackingMessage::RightAnnularClosure = 0.f;
float FTdProTrackingMessage::RightPinkyClosure = 0.f;
	
float FTdProTrackingMessage::RightThumbAbduction = 0.f;
float FTdProTrackingMessage::RightIndexAbduction = 0.f;
float FTdProTrackingMessage::RightMiddleAbduction = 0.f;
float FTdProTrackingMessage::RightPalmAbduction = 0.f;
float FTdProTrackingMessage::RightAnnularAbduction = 0.f;
float FTdProTrackingMessage::RightPinkyAbduction = 0.f;

FQuat FTdProTrackingMessage::RightWristRotation = FQuat::Identity;

float FTdProTrackingMessage::LeftThumbClosure = 0.f;
float FTdProTrackingMessage::LeftIndexClosure = 0.f;
float FTdProTrackingMessage::LeftMiddleClosure = 0.f;
float FTdProTrackingMessage::LeftPalmClosure = 0.f;
float FTdProTrackingMessage::LeftAnnularClosure = 0.f;
float FTdProTrackingMessage::LeftPinkyClosure = 0.f;
	
float FTdProTrackingMessage::LeftThumbAbduction = 0.f;
float FTdProTrackingMessage::LeftIndexAbduction = 0.f;
float FTdProTrackingMessage::LeftMiddleAbduction = 0.f;
float FTdProTrackingMessage::LeftPalmAbduction = 0.f;
float FTdProTrackingMessage::LeftAnnularAbduction = 0.f;
float FTdProTrackingMessage::LeftPinkyAbduction = 0.f;

WeArtJsonMessage::WeArtJsonMessage()
{
	std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(
		std::chrono::system_clock::now().time_since_epoch()
		);
	_timestamp = ms.count();
}

std::string WeArtJsonMessage::serialize()
{
	
	nlohmann::json j;
	j["type"] = getID();
	j["ts"] = _timestamp;

	nlohmann::json payload = serializePayload();
	if (payload != nullptr)
		j["data"] = payload;
		
	return j.dump();
}

void WeArtJsonMessage::deserialize(std::string message)
{
	nlohmann::json j = nlohmann::json::parse(message);
	_timestamp = j["ts"].template get<std::uint64_t>();
	if (j["data"] != nullptr)
		deserializePayload(j["data"]);

}


float TrackingMessage::GetClosure(EHandSide handSide, EActuationPoint actuationPoint) const
{
	uint8 byteValue = 0x00;
	switch (handSide)
	{
		case EHandSide::Left:
			switch (actuationPoint)
			{
				case EActuationPoint::Thumb:     byteValue = LeftThumbClosure;		break;
				case EActuationPoint::Index:     byteValue = LeftIndexClosure;		break;
				case EActuationPoint::Middle:    byteValue = LeftMiddleClosure;		break;
				case EActuationPoint::Palm:		 byteValue = LeftPalmClosure;	    break;
				case EActuationPoint::Annular:   byteValue = LeftAnnularClosure;    break;
				case EActuationPoint::Pinky:	 byteValue = LeftPinkyClosure;		break;
				default: 						 byteValue = LeftThumbClosure;		break;
		
			}
			break;
		case EHandSide::Right:
			switch (actuationPoint)
			{
				case EActuationPoint::Thumb:	byteValue = RightThumbClosure;		break;
				case EActuationPoint::Index:	byteValue = RightIndexClosure;		break;
				case EActuationPoint::Middle:	byteValue = RightMiddleClosure;		break;
				case EActuationPoint::Palm:		byteValue = RightPalmClosure;		break;
				case EActuationPoint::Annular:	byteValue = RightAnnularClosure;	break;
				case EActuationPoint::Pinky:	byteValue = RightPinkyClosure;		break;
				default:						byteValue = RightThumbClosure;  
			}
			break;
	}

	float num(byteValue);
	float denom(255);
	return num / denom;
}

float TrackingMessage::GetAbduction(EHandSide handSide, EActuationPoint actuationPoint) const
{
	constexpr float maxAbductionValue = 255.f;
	switch (handSide) {
	case EHandSide::Left:
		switch(actuationPoint)
		{
			case EActuationPoint::Thumb: return static_cast<float>(LeftThumbAbduction) / static_cast<float>(maxAbductionValue);
			case EActuationPoint::Index: return static_cast<float>(LeftIndexAbduction) / static_cast<float>(maxAbductionValue);
			case EActuationPoint::Middle: return static_cast<float>(LeftMiddleAbduction) / static_cast<float>(maxAbductionValue);
			case EActuationPoint::Palm: return static_cast<float>(LeftPalmAbduction) / static_cast<float>(maxAbductionValue);
			case EActuationPoint::Annular: return static_cast<float>(LeftAnnularAbduction) / static_cast<float>(maxAbductionValue);
			case EActuationPoint::Pinky: return static_cast<float>(LeftPinkyAbduction) / static_cast<float>(maxAbductionValue);
		default: ;
		}
		break;
	case EHandSide::Right:
		switch(actuationPoint)
		{
			case EActuationPoint::Thumb: return static_cast<float>(RightThumbAbduction) / static_cast<float>(maxAbductionValue);
			case EActuationPoint::Index: return static_cast<float>(RightIndexAbduction) / static_cast<float>(maxAbductionValue);
			case EActuationPoint::Middle: return static_cast<float>(RightMiddleAbduction) / static_cast<float>(maxAbductionValue);
			case EActuationPoint::Palm: return static_cast<float>(RightPalmAbduction) / static_cast<float>(maxAbductionValue);
			case EActuationPoint::Annular: return static_cast<float>(RightAnnularAbduction) / static_cast<float>(maxAbductionValue);
			case EActuationPoint::Pinky: return static_cast<float>(RightPinkyAbduction) / static_cast<float>(maxAbductionValue);
		default: ;
		}
		break;
	default: ;
	}
	return 0.0f;
}

std::string FTdProTrackingMessage::serialize()
{
	return WeArtJsonMessage::serialize();
}

void FTdProTrackingMessage::deserialize(std::string message)
{
	nlohmann::json j = nlohmann::json::parse(message);
	_timestamp = j["ts"].template get<std::uint64_t>();
	
	if (j["data"] != nullptr)
	{
		deserializePayload(j["data"]);
	}
}

FQuat FTdProTrackingMessage::LeftWristRotation = FQuat::Identity;

void FTdProTrackingMessage::setHandSide(EHandSide hs)
{
	WeArtJsonMessage::setHandSide(hs);
}

void FTdProTrackingMessage::setActuationPoint(EActuationPoint ap)
{
	WeArtJsonMessage::setActuationPoint(ap);
}

nlohmann::json FTdProTrackingMessage::serializePayload()
{
	return WeArtJsonMessage::serializePayload();
}

void FTdProTrackingMessage::deserializePayload(nlohmann::json payload)
{
	if (payload["handSide"] == "Right" || payload["handSide"] == "RIGHT")
	{
		RightIndexClosure = payload["index"]["closure"].template get <float>();
		RightIndexAbduction = payload["index"]["abduction"].template get<float>();
		
		RightMiddleClosure = payload["middle"]["closure"].template get<float>();
		RightMiddleAbduction = payload["middle"]["abduction"].template get<float>();
		
		RightAnnularClosure = payload["annular"]["closure"].template get<float>();
		RightAnnularAbduction = payload["annular"]["abduction"].template get<float>();
		
		RightPinkyClosure = payload["pinky"]["closure"].template get<float>();
		RightPinkyAbduction = payload["pinky"]["abduction"].template get<float>();
		
		RightThumbClosure = payload["thumb"]["closure"].template get<float>();
		RightThumbAbduction = payload["thumb"]["abduction"].template get<float>();

		RightPalmClosure = payload["palm"]["closure"].template get<float>();
		RightPalmClosure = payload["palm"]["abduction"].template get<float>();

		RightWristRotation.X = payload["wrist"]["quaternion"]["x"].template get<float>();
		RightWristRotation.Y = payload["wrist"]["quaternion"]["y"].template get<float>();
		RightWristRotation.Z = payload["wrist"]["quaternion"]["z"].template get<float>();
		RightWristRotation.W = payload["wrist"]["quaternion"]["w"].template get<float>();
	}
	else if (payload["handSide"] == "Left" || payload["handSide"] == "LEFT")
	{
		LeftIndexClosure = payload["index"]["closure"].template get<float>();
		LeftIndexAbduction = payload["index"]["abduction"].template get<float>();
		
		LeftMiddleClosure = payload["middle"]["closure"].template get<float>();
		LeftMiddleAbduction = payload["middle"]["abduction"].template get<float>();
		
		LeftAnnularClosure = payload["annular"]["closure"].template get<float>();
		LeftAnnularAbduction = payload["annular"]["abduction"].template get<float>();
		
		LeftPinkyClosure = payload["pinky"]["closure"].template get<float>();
		LeftPinkyAbduction = payload["pinky"]["abduction"].template get<float>();
		
		LeftThumbClosure = payload["thumb"]["closure"].template get<float>();
		LeftThumbAbduction = payload["thumb"]["abduction"].template get<float>();

		LeftPalmClosure = payload["palm"]["closure"].template get<float>();
		LeftPalmClosure = payload["palm"]["abduction"].template get<float>();

		LeftWristRotation.X = payload["wrist"]["quaternion"]["x"].template get<float>();
		LeftWristRotation.Y = payload["wrist"]["quaternion"]["y"].template get<float>();
		LeftWristRotation.Z = payload["wrist"]["quaternion"]["z"].template get<float>();
		LeftWristRotation.W = payload["wrist"]["quaternion"]["w"].template get<float>();
	}
}

float FTdProTrackingMessage::GetClosure(EHandSide HandSide, EActuationPoint actuationPoint) const
{
	switch (HandSide)
	{
		case EHandSide::Left:
			switch (actuationPoint)
			{
				case EActuationPoint::Thumb:     return LeftThumbClosure;
				case EActuationPoint::Index:     return LeftIndexClosure;
				case EActuationPoint::Middle:    return LeftMiddleClosure;
				case EActuationPoint::Palm:		 return LeftPalmClosure;	    
				case EActuationPoint::Annular:   return LeftAnnularClosure;    
				case EActuationPoint::Pinky:	 return LeftPinkyClosure;		
				default: 						 return LeftThumbClosure;		
			}
		case EHandSide::Right:
			switch (actuationPoint)
			{
				case EActuationPoint::Thumb:	return RightThumbClosure;		
				case EActuationPoint::Index:	return RightIndexClosure;		
				case EActuationPoint::Middle:	return RightMiddleClosure;		
				case EActuationPoint::Palm:		return RightPalmClosure;		
				case EActuationPoint::Annular:	return RightAnnularClosure;	
				case EActuationPoint::Pinky:	return RightPinkyClosure;		
				default:						return RightThumbClosure;  
			}
	}
	return 0.f;
}

float FTdProTrackingMessage::GetAbduction(EHandSide HandSide, EActuationPoint actuationPoint) const
{
	switch (HandSide)
	{
	case EHandSide::Left:
		switch (actuationPoint)
		{
			case EActuationPoint::Thumb:     return LeftThumbAbduction;
			case EActuationPoint::Index:     return LeftIndexAbduction;
			case EActuationPoint::Middle:    return LeftMiddleAbduction;
			case EActuationPoint::Palm:		 return LeftPalmAbduction;	    
			case EActuationPoint::Annular:   return LeftAnnularAbduction;    
			case EActuationPoint::Pinky:	 return LeftPinkyAbduction;		
			default: 						 return LeftThumbAbduction;		
		}
	case EHandSide::Right:
		switch (actuationPoint)
		{
			case EActuationPoint::Thumb:	return RightThumbAbduction;		
			case EActuationPoint::Index:	return RightIndexAbduction;		
			case EActuationPoint::Middle:	return RightMiddleAbduction;		
			case EActuationPoint::Palm:		return RightPalmAbduction;		
			case EActuationPoint::Annular:	return RightAnnularAbduction;	
			case EActuationPoint::Pinky:	return RightPinkyAbduction;		
			default:						return RightThumbAbduction;  
		}
	}
	return 0.f;
}

bool RawSensorsData::hasSensor(EActuationPoint ap)
{
	return sensors.find(ap) != sensors.end();
}

SensorData RawSensorsData::getSensor(EActuationPoint ap)
{
	return sensors[ap];
}

nlohmann::json RawSensorsData::serializePayload()
{
	nlohmann::json j;
	j["handSide"] = hand;
	for (const auto& s : sensors) {
		std::string actuationPoint = ActuationPointToString(s.first);
		std::transform(actuationPoint.begin(), actuationPoint.end(), actuationPoint.begin(),
			[](unsigned char c) { return std::tolower(c); });

		j[actuationPoint] = s.second;
	}
	return j;
}

void RawSensorsData::deserializePayload(nlohmann::json payload)
{
	std::string hs = payload["handSide"].get<std::string>();	
	hand = StringToHandside(hs);

	if (payload.contains("index") && payload["index"].is_object()) 
	{
		sensors[EActuationPoint::Index] = payload["index"].get<SensorData>();
	} 
	if (payload.contains("middle") && payload["middle"].is_object()) 
	{
		sensors[EActuationPoint::Middle] = payload["middle"].get<SensorData>();
	}
	if (payload.contains("thumb") && payload["thumb"].is_object()) 
	{
		sensors[EActuationPoint::Thumb] = payload["thumb"].get<SensorData>();
	}
}

void TdProRawData::deserializePayload(nlohmann::json payload)
{
	std::string hs = payload["handSide"].get<std::string>();	
	hand = StringToHandside(hs);

	if (payload.contains("index") && payload["index"].is_object()) 
	{
		sensors[EActuationPoint::Index] = payload["index"].get<SensorData>();
	} 
	if (payload.contains("middle") && payload["middle"].is_object()) 
	{
		sensors[EActuationPoint::Middle] = payload["middle"].get<SensorData>();
	}
	if (payload.contains("thumb") && payload["thumb"].is_object()) 
	{
		sensors[EActuationPoint::Thumb] = payload["thumb"].get<SensorData>();
	}
	if (payload.contains("annular") && payload["annular"].is_object()) 
	{
		sensors[EActuationPoint::Annular] = payload["annular"].get<SensorData>();
	}
	if (payload.contains("pinky") && payload["pinky"].is_object()) 
	{
		sensors[EActuationPoint::Pinky] = payload["pinky"].get<SensorData>();
	}
	if (payload.contains("palm") && payload["palm"].is_object()) 
	{
		sensors[EActuationPoint::Palm] = payload["palm"].get<SensorData>();
	}
}

nlohmann::json FMiddlewareStatusMessage::serializePayload()
{
	nlohmann::json json = _data;
	return json;
}

void FMiddlewareStatusMessage::deserializePayload(nlohmann::json payload)
{
	_data = payload.template get<MiddlewareStatusData>();
	_data.timestamp = _timestamp;
}

nlohmann::json TdProWeArtAppStatus::serializePayload()
{
	nlohmann::json json = _data;
	return json;
}

void TdProWeArtAppStatus::deserializePayload(nlohmann::json payload)
{
	_data = payload.template get<WeartAppStatusData>();
	_data.timestamp = _timestamp;
}

nlohmann::json DevicesStatusMessage::serializePayload()
{
	nlohmann::json json;
	json["devices"] = _devices;
	return json;
}

void DevicesStatusMessage::deserializePayload(nlohmann::json payload)
{
	if (payload["devices"] == nullptr)
		return;
	_devices = payload["devices"].get<std::vector<ConnectedDeviceStatus>>();
}


nlohmann::json TdProDevicesStatusMessage::serializePayload()
{
	nlohmann::json json;
	json["devices"] = _devices;
	return json;
}

void TdProDevicesStatusMessage::deserializePayload(nlohmann::json payload)
{
	if (payload["devices"] == nullptr)
		return;
	_devices = payload["devices"].get<std::vector<TdProConnectedDeviceStatus>>();
}
