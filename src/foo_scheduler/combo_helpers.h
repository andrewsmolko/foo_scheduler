#pragma once

namespace ComboHelpers
{
	inline void InitCombo(CComboBox& combo, const std::vector<std::pair<std::wstring, int>>& items, int selected)
	{
		for (std::size_t i = 0; i < items.size(); ++i)
			combo.SetItemData(combo.AddString(items[i].first.c_str()), items[i].second);

		for (int i = 0; i < combo.GetCount(); ++i)
			if (combo.GetItemData(i) == selected)
			{
				combo.SetCurSel(i);
				break;
			}
	}

	template<typename T>
	inline T GetSelectedItem(CComboBox& combo)
	{
		return static_cast<T>(combo.GetItemData(combo.GetCurSel()));
	}

} // namespace ComboHelpers