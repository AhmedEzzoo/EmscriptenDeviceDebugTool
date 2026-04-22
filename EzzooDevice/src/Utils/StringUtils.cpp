#include "ezpch.h"

#include "StringUtils.h"



namespace Ezzoo::Utils {


	std::string ByteToString(uint64_t byte)
	{

		uint32_t gigaByte = 1024 * 1024 * 1024;
		uint32_t megaByte = 1024 * 1024 ;
		uint32_t kiloByte = 1024;

		char buf[32 +1] {};


		if (byte > gigaByte)
		{
			snprintf(buf, 32, "%0.2f GB", (float)byte / (float)gigaByte);
		}

		else if (byte > megaByte)
		{
			snprintf(buf, 32, "%0.2f MB", (float)byte / (float)megaByte);
		}
		else if (byte > kiloByte)
		{
			snprintf(buf, 32, "%0.2f KB", (float)byte / (float)kiloByte);
		}


		return std::string(buf);


	}


}