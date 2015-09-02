#pragma once

#include "foobar_stream.h"

namespace S11nBlocks
{

typedef pfc::array_t<t_uint8, pfc::alloc_fast> ByteBuffer;

//------------------------------------------------------------------------------
// Exception
//------------------------------------------------------------------------------

class Exception
{
public:
	static Exception StreamReadError();
	static Exception RequiredFieldNotSet(int fieldID);
	std::wstring GetMessage() const;

private:
	explicit Exception(const std::wstring& msg);

private:
	std::wstring m_msg;
};

//------------------------------------------------------------------------------
// IStreamSerializable
//------------------------------------------------------------------------------

class IStreamSerializable
{
public:
	virtual void ParseFromStream(foobar_stream_reader& input) = 0;
	virtual void SerializeToStream(foobar_stream_writer& output) const = 0;

protected:
	~IStreamSerializable() {}
};

//------------------------------------------------------------------------------
// IField
//------------------------------------------------------------------------------

struct IField : public IStreamSerializable
{
	virtual int GetID() const = 0;
	virtual bool Exists() const = 0;
	virtual bool Required() const = 0;
	virtual ~IField() {}
};

//------------------------------------------------------------------------------
// SerializeOperations
//------------------------------------------------------------------------------

template<typename T, bool serializable>
struct SerializeOperations
{
	static void ParseFromStream(foobar_stream_reader& input, T& value)
	{
		try
		{
			input >> value;
		}
		catch (exception_io&)
		{
			throw Exception::StreamReadError();
		}			
	}

	static void SerializeToStream(foobar_stream_writer& output, const T& value)
	{
		output << value;
	}
};

template<typename T>
struct SerializeOperations<T, true>
{
	static void ParseFromStream(foobar_stream_reader& input, T& value)
	{
		value.ParseFromStream(input);
	}

	static void SerializeToStream(foobar_stream_writer& output, const T& value)
	{
		value.SerializeToStream(output);
	}
};

//------------------------------------------------------------------------------
// Field
//------------------------------------------------------------------------------

template<typename T, int uid, bool required = false>
class Field : public IField
{
public:
	Field() : m_exists(false), m_value() {}

	void SetValue(const T& value)
	{
		m_value = value;
		m_exists = true;
	}

	const T& GetValue() const
	{
		_ASSERTE(m_exists);
		return m_value;
	}

	bool GetValueIfExists(T& out) const
	{
		if (m_exists)
		{
			out = m_value;
			return true;
		}

		return false;
	}

public:
	virtual int GetID() const
	{
		return uid;
	}

	virtual bool Exists() const
	{
		return m_exists;
	}

	virtual bool Required() const
	{
		return required;
	}

	virtual void ParseFromStream(foobar_stream_reader& input)
	{
		SerializeOperations<T, boost::is_base_of<IStreamSerializable, T>::value>::
			ParseFromStream(input, m_value);

		m_exists = true;
	}

	virtual void SerializeToStream(foobar_stream_writer& output) const
	{
		SerializeOperations<T, boost::is_base_of<IStreamSerializable, T>::value>::
			SerializeToStream(output, m_value);
	}

private:
	bool m_exists;
	T m_value;
};

template<typename T, int uid, bool required = false>
class RepeatedField : public IField
{
public:
	RepeatedField() : m_exists(false) {}

	int GetSize() const
	{
		_ASSERTE(m_exists);
		return static_cast<int>(m_container.size());
	}

	const T& GetAt(int index) const
	{
		_ASSERTE(m_exists);
		_ASSERTE(index >= 0 && index < static_cast<int>(m_container.size()));

		return m_container[index];
	}

	void Add(const T& elem)
	{
		m_container.push_back(elem);
		m_exists = true;
	}

public:
	virtual int GetID() const
	{
		return uid;
	}

	virtual bool Exists() const
	{
		return m_exists;
	}

	virtual bool Required() const
	{
		return required;
	}

	virtual void ParseFromStream(foobar_stream_reader& input)
	{
		int size = 0;
		
		try
		{
			input >> size;
		}
		catch (exception_io&)
		{
			throw Exception::StreamReadError();
		}		

		m_container.resize(size);

		for (std::size_t i = 0; i < m_container.size(); ++i)
		{
			SerializeOperations<T, boost::is_base_of<IStreamSerializable, T>::value>::
				ParseFromStream(input, m_container[i]);
		}

		m_exists = true;
	}

	virtual void SerializeToStream(foobar_stream_writer& output) const
	{
		output << static_cast<int>(m_container.size());

		for (std::size_t i = 0; i < m_container.size(); ++i)
		{
			SerializeOperations<T, boost::is_base_of<IStreamSerializable, T>::value>::
				SerializeToStream(output, m_container[i]);
		}
	}

private:
	bool m_exists;
	std::vector<T> m_container;
};

class ArchiveImpl
{
public:
	class CRegProxy
	{
	public:
		explicit CRegProxy(ArchiveImpl& archive);
		CRegProxy& operator () (IField& field);

	private:
		ArchiveImpl& m_archive;
	};

	CRegProxy RegisterFields(IField& field);

	void SaveToStream(foobar_stream_writer& output);
	void LoadFromStream(foobar_stream_reader& input);

private:
	void RegFieldImpl(IField& field);
	IField* FindFieldWithID(int id) const;

private:
	std::vector<IField*> m_fields;
};

template<class T>
class Block : public IStreamSerializable
{
public:
	// Throws Exception.
	virtual void ParseFromStream(foobar_stream_reader& input)
	{
		ArchiveImpl archive;

		T* pT = static_cast<T*>(this);
		pT->RegisterFields(archive);

		archive.LoadFromStream(input);
	}

	// Throws Exception.
	virtual void SerializeToStream(foobar_stream_writer& output) const
	{
		ArchiveImpl archive;

		T* pT = const_cast<T*>(static_cast<const T*>(this));
		pT->RegisterFields(archive);

		archive.SaveToStream(output);
	}
};

/*
Sample block definition:

struct CTestBlock : public S11nBlocks::Block<CTestBlock>
{
	S11nBlocks::Field<QString, 1, true> name;
	S11nBlocks::Field<CSmallBlock, 2> smallMessage;
	S11nBlocks::RepeatedField<QString, 5> files;
	S11nBlocks::RepeatedField<CSmallBlock, 6> items;

	template<class Archive>
	void RegisterFields(Archive& ar)
	{
		ar.RegisterFields(name)(smallMessage)(files)(items);
	}
};

Field identifiers (the 2nd template parameter) must be unique in a block.
Even if you do not need a field any more do not reuse its id, mark it as deprecated.

A field may be required or optional (see the 3rd template parameter).
Required fields cause exception during serialization if they are not set.

Field<T, 1> myField;
For T an operator << >> must exist.

A block can't be used is containers, use RepeatedField.

Saving sample:

	CTestBlock msg;
	msg.name.SetValue("Hello!");
	msg.smallMessage.SetValue(smallMsg);
	msg.secondName.SetValue("Second name!!!");
	msg.numPeople.SetValue(5);

	msg.files.Add("file1.txt");
	msg.files.Add("file2.txt");
	msg.files.Add("file3.txt");

	for (int i = 0; i < 100; ++i)
	{
		CSmallMessage smallMsg;
		smallMsg.numItems.SetValue(i);
		smallMsg.sizeOfItem.SetValue(123);
		msg.items.Add(smallMsg);
	}

	try
	{
		msg.SerializeToStream(stream);
	}
	catch (S11nBlocks::CException& ex)
	{
		QString s = ex.GetMessage();
	}

Loading sample:

	CTestBlock msg;

	try
	{
		msg.ParseFromStream(stream);
	}
	catch (S11nBlocks::CException& ex)
	{
		QString s = ex.GetMessage();
	}

	if (msg.name.Exists())
	{
		QString s = msg.name.GetValue();
	}

	if (msg.files.Exists())
	{
		for (int i = 0; i < msg.files.GetSize(); ++i)
		{
			QString s = msg.files.GetAt(i);
		}
	}
*/

} // namespace S11nBlocks