// avida_wrapper.h

#pragma once

#include "avida/Avida.h"

#include "avida/viewer/Driver.h"

using namespace System;

namespace avida_wrapper {

	public ref class Avida_Wrapper
	{
	public:
		static void Initialize()
		{
			Avida::Initialize();

		}
		// TODO: Add your methods for this class here.
	};
}
