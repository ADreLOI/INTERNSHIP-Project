#pragma once
#include "WeArtContactPoint.h"
#include "WeArtFingerState.generated.h"
USTRUCT(BlueprintType)
struct FUFingerState
{
public:
	GENERATED_BODY()
	UPROPERTY(BlueprintReadWrite)
	float MinClosure;
	UPROPERTY(BlueprintReadWrite)
	float CurrentClosure;
	UPROPERTY(BlueprintReadWrite)
	float ContactClosure = 1;
	UPROPERTY(BlueprintReadWrite)
	float MaxAbdcation;
	UPROPERTY(BlueprintReadWrite)
	float MinAbdcation;
	UPROPERTY(BlueprintReadWrite)
	float CurrentAbduction;
	UPROPERTY(BlueprintReadWrite)
	bool Collided;
	UPROPERTY(BlueprintReadWrite)
	bool Allowed = true;
	UPROPERTY(BlueprintReadWrite)
	bool ObjectInRange;
	UPROPERTY(BlueprintReadWrite)
	float MaxClosureByCalculation = 1;
	UPROPERTY(BlueprintReadWrite)
	float MaxAbdcationByCalculation = 1;
	
};
