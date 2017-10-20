#include <Utilities/precompiled.h>
#include <Systems/Graphics/GraphicsUtilities/InputData.h>
#include <Systems/Graphics/GraphicsUtilities/GraphicsTypes.h> //to define DataFormat

InputData::InputData(const std::string& semanticName_, const DataFormat& format_, const bool instanceData_)
	: semanticName(semanticName_), format(format_), instanceData(instanceData_)
{
}