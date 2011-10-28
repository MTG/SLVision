#pragma once

#include "GlobalConfigDetail.h"

namespace datasaver
{
	class GlobalConfig /*: public Singleton<GlobalConfig>*/
	{
	public:
		template<typename T>
		static T & getRef(const std::string& k, const T & defaultvalue = T())
		{
			return GlobalConfigDetail::getRef<T>("SLVision:"+k,defaultvalue);
		}
	};
}