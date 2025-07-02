#pragma once
#include "WeArtContactPoint.h"
#include "WeArtSettings.h"

// TODO: Add touch diver pro components
class FWeArtFingersDictionary
{
	
	EDeviceGeneration DiverType;
	
public:
	// Default constructor
	FWeArtFingersDictionary()
		: DiverType(EDeviceGeneration::TouchDiver) // Assuming 'DefaultDiver' is a valid value for EWeArtDiverType
	{
	}

	// Constructor with one parameter
	FWeArtFingersDictionary(EDeviceGeneration InDiverType)
		: DiverType(InDiverType)
	{
	}

	// Original constructor with three parameters
	FWeArtFingersDictionary(EDeviceGeneration InDiverType,
		const TMap<WeArtContactPointPartType, FWeArtContactPoint>& InWeArtFingersSlaveDictionary,
		const TMap<WeArtContactPointPartType, FWeArtContactPoint>& InWeArtFingersMasterDictionary)
		: DiverType(InDiverType),
		  WeArtFingersSlaveDictionary(InWeArtFingersSlaveDictionary),
		  WeArtFingersMasterDictionary(InWeArtFingersMasterDictionary)
	{
	}
	
	TMap<WeArtContactPointPartType, FWeArtContactPoint> WeArtFingersSlaveDictionary = {
		{
			WeArtContactPointPartType::Index,
			FWeArtContactPoint(WeArtContactPointType::Slave, EWeArtFingerType::Index, "SocketIndex", false)
		},
		{
			WeArtContactPointPartType::Middle,
			FWeArtContactPoint(WeArtContactPointType::Slave, EWeArtFingerType::Middle, "SocketMiddle", false)
		},
		{
			WeArtContactPointPartType::Ring,
			FWeArtContactPoint(WeArtContactPointType::Slave, DiverType == EDeviceGeneration::TouchDiverPro ? EWeArtFingerType::Ring : EWeArtFingerType::Middle, "SocketRing", false)
		},
		{
			WeArtContactPointPartType::RingFalang1,
			FWeArtContactPoint(WeArtContactPointType::Slave, DiverType == EDeviceGeneration::TouchDiverPro ? EWeArtFingerType::Ring : EWeArtFingerType::Middle, "SocketRing_Falang_1", false)
		},
		{
			WeArtContactPointPartType::RingFalang2,
			FWeArtContactPoint(WeArtContactPointType::Slave, DiverType == EDeviceGeneration::TouchDiverPro ? EWeArtFingerType::Ring : EWeArtFingerType::Middle, "SocketRing_Falang_2_2", false)
		},
		{
			WeArtContactPointPartType::Pinky,
			FWeArtContactPoint(WeArtContactPointType::Slave, DiverType == EDeviceGeneration::TouchDiverPro ? EWeArtFingerType::Pinky : EWeArtFingerType::Middle, "SocketPinky", false)
		},
		{
			WeArtContactPointPartType::PinkyFalang1,
			FWeArtContactPoint(WeArtContactPointType::Slave, DiverType == EDeviceGeneration::TouchDiverPro ? EWeArtFingerType::Pinky : EWeArtFingerType::Middle, "SocketPinky_Falang_1", false)
		},
		{
			WeArtContactPointPartType::PinkyFalang2,
			FWeArtContactPoint(WeArtContactPointType::Slave, DiverType == EDeviceGeneration::TouchDiverPro ? EWeArtFingerType::Pinky : EWeArtFingerType::Middle, "SocketPinky_Falang_2_2", false)
		},
		{
			WeArtContactPointPartType::IndexFalang1,
			FWeArtContactPoint(WeArtContactPointType::Secondary, EWeArtFingerType::Index, "SocketIndex_Falang_1",
			                   false)
		},
		{
			WeArtContactPointPartType::MiddleFalang1,
			FWeArtContactPoint(WeArtContactPointType::Secondary, EWeArtFingerType::Middle, "SocketMiddle_Falang_1",
			                   false)
		},
		{
			WeArtContactPointPartType::IndexFalang2,
			FWeArtContactPoint(WeArtContactPointType::Secondary, EWeArtFingerType::Index, "SocketIndex_Falang_2_2", false)
		},
		{
			WeArtContactPointPartType::MiddleFalang2,
			FWeArtContactPoint(WeArtContactPointType::Secondary, EWeArtFingerType::Middle, "SocketMiddle_Falang_2_2", false)
		},
		{
			WeArtContactPointPartType::ThumbFalang1,
			FWeArtContactPoint(WeArtContactPointType::Slave, EWeArtFingerType::Thumb, "SocketThumb_Falang_1",
			                   false)
		},
		{
			WeArtContactPointPartType::ThumbFalang2,
			FWeArtContactPoint(WeArtContactPointType::Secondary, EWeArtFingerType::Thumb, "SocketThumb_Falang_2_2", false)
		}
	};
	TMap<WeArtContactPointPartType, FWeArtContactPoint> WeArtFingersMasterDictionary = {
		{
			WeArtContactPointPartType::Thumb,
			FWeArtContactPoint(WeArtContactPointType::Master, EWeArtFingerType::Thumb, "SocketThumb", false)
		},
		{
			WeArtContactPointPartType::Palm,
			FWeArtContactPoint(WeArtContactPointType::Master, EWeArtFingerType::Palm, "SocketCenter", false)
		}
	};
};
