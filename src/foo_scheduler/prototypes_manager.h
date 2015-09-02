#pragma once

template<class PrototypeInterface>
class PrototypesManager : private boost::noncopyable
{
public:
	bool RegisterPrototype(PrototypeInterface* pPrototype)
	{
		m_prototypes.push_back(pPrototype);
		return true;
	}

	// Returns a vector of prototypes sorted by priority.
	std::vector<PrototypeInterface*> GetPrototypes()
	{
		std::vector<PrototypeInterface*> result(m_prototypes.size());
		std::transform(m_prototypes.begin(), m_prototypes.end(), result.begin(), &boost::lambda::_1);

		// Sort by priority.
		std::sort(result.begin(), result.end(),
			boost::bind(&PrototypeInterface::GetPriority, _1) < boost::bind(&PrototypeInterface::GetPriority, _2));

		return result;
	}

	PrototypeInterface* GetPrototypeByGUID(const GUID& guid)
	{
		for (std::size_t i = 0; i < m_prototypes.size(); ++i)
			if (m_prototypes[i].GetPrototypeGUID() == guid)
				return &m_prototypes[i];

		return 0;
	}

private:
	boost::ptr_vector<PrototypeInterface> m_prototypes;
};