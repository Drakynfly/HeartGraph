// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "FlakesStructs.h"

namespace Flakes::NetBinary
{
	/*
	 * A binary serialization provider optimized for sending data over the network. This provider assumes that the data
	 * is never written to disk. It requires about ~50% of the memory the regular binary provider uses.
	 */
	SERIALIZATION_PROVIDER_HEADER(HEARTNET_API, NetBinary)
}