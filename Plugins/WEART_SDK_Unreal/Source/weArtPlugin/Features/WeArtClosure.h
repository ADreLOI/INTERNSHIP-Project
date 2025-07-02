#pragma once

#include "CoreMinimal.h"

/**
 * @namespace WeArt
 * @brief Namespace for the WeArt plugin.
 */
namespace WeArt{
	/**
	 * @struct Force
	 * @brief Represents a force in the WeArt plugin.
	 */
	struct Force {

		float values;	/**< The value of the force. */
		bool active;	/**< Flag indicating if the force is active. */
	};
}