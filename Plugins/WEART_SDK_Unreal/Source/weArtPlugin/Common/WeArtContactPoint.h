#pragma once
#include "WeArtContactPointPartType.h"
#include "WeArtContactPointType.h"
#include "EWeArtFingerType.h"
#include "WeArtTouchableObject.h"


class FWeArtContactPoint
{
private:
	WeArtContactPointType ContactPointType;
	EWeArtFingerType FingerType;
	bool Contacted;
	bool ObjectInRadius;
	UWeArtTouchableObject* TouchedObject;
	float ContactClosure;
	float ContactAbduction;
	float MinClosure;

public:
	float GetMinClosure() const;
	void SetMinClosure(float MinClosure);
	float GetMinAbduction() const;
	void SetMinAbduction(float MinAbduction);

private:
	float MinAbduction;
	FVector ContactLocation;
	FVector ContactNormal;
	FString ContactSocketName;

public:
	float GetContactAbduction() const;
	void SetContactAbduction(float ContactAbductionRef);
	bool IsObjectInRadius() const;
	void SetObjectInRadius(bool bObjectInRadius);
	float GetContactClosure() const;
	void SetContactClosure(float ContactClosureRef);
	UWeArtTouchableObject* GetTouchedObject() const;
	void SetTouchedObject(UWeArtTouchableObject* TouchedObjectRef);
	FString GetContactSocketName() const;
	FVector GetContactNormal() const;
	void SetContactNormal(const FVector& ContactNormal);
	FVector GetContactLocation() const;
	void SetContactLocation(const FVector& ContactLocation);
	bool IsContacted() const;
	FWeArtContactPoint(WeArtContactPointType ContactPointType, EWeArtFingerType FingerType, FString ContactSocketNameRef,
	                   bool bIsContacted);
	FWeArtContactPoint(WeArtContactPointType ContactPointType, EWeArtFingerType FingerType, const FVector& Location,
	                   const FVector& ContactNormalRef, bool bIsContacted);
	FWeArtContactPoint();
	WeArtContactPointType GetContactPointType() const;
	void SetContactPointType(WeArtContactPointType ContactPointType);
	EWeArtFingerType GetFingerType() const;
	void SetFingerType(EWeArtFingerType FingerType);
	void SetIsContacted(bool bIsContacted);
};
