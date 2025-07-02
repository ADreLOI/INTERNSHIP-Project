
#pragma once

#include "CoreMinimal.h"
#include "Engine/UserDefinedEnum.h"
#include "WeArtEnums.generated.h"

/**
 * @class UWeArtEnums
 * @brief Defines custom enums for the WeArt plugin.
 */
UCLASS()
class WEARTPLUGIN_API UWeArtEnums : public UUserDefinedEnum
{
	GENERATED_BODY()
	
	enum EHandTest
	{
		Left,	/**< Left hand */
		Right,	/**< Right hand */
		Test	/**< Test value */
	};
};
