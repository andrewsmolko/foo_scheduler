#pragma once

namespace DurationUnits
{
	enum Type
	{
		seconds = 0,
		minutes,
		hours,

		numTypes
	};

	inline std::wstring Label(Type type)
	{
		switch (type)
		{
		case seconds:
			return L"seconds";

		case minutes:
			return L"minutes";

		case hours:
			return L"hours";
		}

		_ASSERTE(false);
		return std::wstring();
	}

} // namespace DurationUnits