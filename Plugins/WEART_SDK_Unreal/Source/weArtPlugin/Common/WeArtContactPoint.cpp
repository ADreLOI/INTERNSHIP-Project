#include "WeArtContactPoint.h"
#include "WeArtContactPointType.h"

bool FWeArtContactPoint::IsObjectInRadius() const
{
	return ObjectInRadius;
}

void FWeArtContactPoint::SetObjectInRadius(bool bObjectInRadius)
{
	ObjectInRadius = bObjectInRadius;
}

float FWeArtContactPoint::GetMinClosure() const
{
	return MinClosure;
}

void FWeArtContactPoint::SetMinClosure(float MinClosureRef)
{
	this->MinClosure = MinClosureRef;
}

float FWeArtContactPoint::GetMinAbduction() const
{
	return MinAbduction;
}

void FWeArtContactPoint::SetMinAbduction(float MinAbductionRef)
{
	this->MinAbduction = MinAbductionRef;
}

float FWeArtContactPoint::GetContactAbduction() const
{
	return ContactAbduction;
}

void FWeArtContactPoint::SetContactAbduction(float ContactAbductionRef)
{
	this->ContactAbduction = ContactAbductionRef;
}

float FWeArtContactPoint::GetContactClosure() const
{
	return ContactClosure;
}

void FWeArtContactPoint::SetContactClosure(float ContactClosureRef)
{
	this->ContactClosure = ContactClosureRef;
}

UWeArtTouchableObject* FWeArtContactPoint::GetTouchedObject() const
{
	return TouchedObject;
}

void FWeArtContactPoint::SetTouchedObject(UWeArtTouchableObject* TouchedObjectRef)
{
	this->TouchedObject = TouchedObjectRef;
}

FString FWeArtContactPoint::GetContactSocketName() const
{
	return ContactSocketName;
}

FVector FWeArtContactPoint::GetContactNormal() const
{
	return ContactNormal;
}

void FWeArtContactPoint::SetContactNormal(const FVector& ContactNormalRef)
{
	this->ContactNormal = ContactNormalRef;
}

FVector FWeArtContactPoint::GetContactLocation() const
{
	return ContactLocation;
}

void FWeArtContactPoint::SetContactLocation(const FVector& ContactLocationRef)
{
	this->ContactLocation = ContactLocationRef;
}

bool FWeArtContactPoint::IsContacted() const
{
	return Contacted;
}

FWeArtContactPoint::FWeArtContactPoint(WeArtContactPointType ContactPointTypeRef, EWeArtFingerType FingerTypeRef,
                                       FString ContactPointSocket,
                                       bool bIsContacted): ObjectInRadius(false), TouchedObject(nullptr),
                                                           ContactClosure(0),
                                                           ContactAbduction(0), MinClosure(0),
                                                           MinAbduction(0)
{
	this->ContactPointType = ContactPointTypeRef;
	this->FingerType = FingerTypeRef;
	this->Contacted = bIsContacted;
	this->ContactSocketName = ContactPointSocket;
}

FWeArtContactPoint::FWeArtContactPoint(WeArtContactPointType ContactPointTypeRef, EWeArtFingerType FingerTypeRef,
                                       const FVector& LocationRef, const FVector& ContactNormalRef, bool bIsContacted):
	ObjectInRadius(false),
	TouchedObject(nullptr),
	ContactClosure(0),
	ContactAbduction(0), MinClosure(0),
	MinAbduction(
		0)
{
	this->ContactPointType = ContactPointTypeRef;
	this->FingerType = FingerTypeRef;
	this->Contacted = bIsContacted;
	this->ContactLocation = LocationRef;
	this->ContactNormal = ContactNormalRef;
}

FWeArtContactPoint::FWeArtContactPoint(): ContactPointType(), FingerType(), Contacted(false), ObjectInRadius(false),
                                          TouchedObject(nullptr),
                                          ContactClosure(0),
                                          ContactAbduction(0),
                                          MinClosure(0),
                                          MinAbduction(0)
{
}

WeArtContactPointType FWeArtContactPoint::GetContactPointType() const
{
	return ContactPointType;
}

void FWeArtContactPoint::SetContactPointType(WeArtContactPointType ContactPointTypeRef)
{
	this->ContactPointType = ContactPointTypeRef;
}

EWeArtFingerType FWeArtContactPoint::GetFingerType() const
{
	return FingerType;
}

void FWeArtContactPoint::SetFingerType(EWeArtFingerType FingerTypeRef)
{
	this->FingerType = FingerTypeRef;
}


void FWeArtContactPoint::SetIsContacted(bool bIsContacted)
{
	this->Contacted = bIsContacted;
}
