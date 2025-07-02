#pragma once

class WeArtUtilities
{
public:
	template <typename TEnum>
	static TEnum StringToEnum(const FString& EnumString)
	{
		UEnum* EnumPtr = StaticEnum<TEnum>();
		if (!EnumPtr)
		{
			return TEnum(0);
		}
	
		return static_cast<TEnum>(EnumPtr->GetValueByName(FName(EnumString)));
	}
	
	template <class TEnum>
	static FString EnumToFString(TEnum EnumValue)
	{
		return StaticEnum<TEnum>()->GetNameByValue(static_cast<int64>(EnumValue)).ToString();
	}

	static FString BoolToString(bool Value)
	{
		return Value ? "true" : "false";
	}

	template <typename T>
	static T* FindChildComponentOfType(USceneComponent* ParentComponent)
	{
		if (!ParentComponent) return nullptr;

		// Check if the parent itself is of the desired type
		if (T* FoundComponent = Cast<T>(ParentComponent))
		{
			return FoundComponent;
		}

		// Recursively search through child components
		TArray<USceneComponent*> Children;
		ParentComponent->GetChildrenComponents(true, Children); // true = recursive search

		for (USceneComponent* Child : Children)
		{
			if (T* FoundChild = Cast<T>(Child))
			{
				return FoundChild; // Return the first found component of the desired type
			}
		}

		return nullptr; // No matching component found
	}
	// TODO: Create static method for auto-logic FString with auto generated unique ID with static counter on call
};