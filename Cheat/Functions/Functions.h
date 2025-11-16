#pragma once
#include "../../IL2CPPResolver/il2cpp_resolver.hpp"
#include "../../IL2CPPResolver/Unity/Structures/Engine.hpp"
#include "../Utils/Offsets.h"

// Put "inline" keyword to every function, regards

namespace GameFunctions {
	// DEPRECATED: Use Unity::Camera::GetMain() instead
	// This function is kept for backward compatibility but may cause crashes
	inline Unity::CCamera* GetUnityCamera()
	{
		// Validate offset before attempting to call
		if (Offsets::GetCameraMainOffset == 0)
		{
			printf("[ @zzzmate ] ERROR: GetCameraMainOffset is NULL, returning nullptr\n");
			return nullptr;
		}

		Unity::CCamera* (UNITY_CALLING_CONVENTION GetCameraTemplate)();
		return reinterpret_cast<decltype(GetCameraTemplate)>(Offsets::GetCameraMainOffset)();
	}
}