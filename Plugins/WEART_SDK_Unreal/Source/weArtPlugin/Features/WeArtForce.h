#pragma once

#include "CoreMinimal.h"
#include "WeArtCommon.h"
#include "WeArtForce.generated.h"

/// <summary>
/// Force value to be applied to an effect
/// </summary>
USTRUCT(BlueprintType)
struct FWeArtForce {
	GENERATED_BODY()

	// The force is a 3D vector. Only the first component is currently used
	// to represent the actual force, usage for the remaining components might come later.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (InlineEditConditionToggle))
	bool active;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (DisplayName = "Value", EditCondition = "active", ClampMin = "0.0", ClampMax = "1.0"))
	float value;
	// Applies fixed force value
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool DisableDynamicForce;
	
	/** Default constructor initializes force as inactive with default value. */
	FWeArtForce() : active(false), value(WeArtConstants::defaultForce), DisableDynamicForce(false)
	{
	};

	/**
	 * @brief Equality operator for comparing two force objects.
	 * @param other The other force object to compare.
	 * @return True if the two force objects are equal, false otherwise.
	 */
	bool operator== (const FWeArtForce& other) const
	{
		return (active == other.active && value == other.value);
	};
	
	/**
	 * @brief Inequality operator for comparing two force objects.
	 * @param other The other force object to compare.
	 * @return True if the two force objects are not equal, false otherwise.
	 */
	bool operator!= (const FWeArtForce& other) const
	{
		return (active != other.active || value != other.value);
	};
};
