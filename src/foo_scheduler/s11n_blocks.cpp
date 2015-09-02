#include "pch.h"
#include "s11n_blocks.h"

namespace S11nBlocks
{

//------------------------------------------------------------------------------
// Exception
//------------------------------------------------------------------------------

Exception::Exception(const std::wstring& msg) : m_msg(msg)
{

}

S11nBlocks::Exception Exception::StreamReadError()
{
	return Exception(L"Stream read error");
}

S11nBlocks::Exception Exception::RequiredFieldNotSet(int fieldID)
{
	return Exception(boost::str(boost::wformat(L"Required field with id = %1% doesn't exist") % fieldID));
}

std::wstring Exception::GetMessage() const
{
	return m_msg;
}

//------------------------------------------------------------------------------
// ArchiveImpl
//------------------------------------------------------------------------------

ArchiveImpl::CRegProxy::CRegProxy(ArchiveImpl& archive) : m_archive(archive)
{

}

ArchiveImpl::CRegProxy& ArchiveImpl::CRegProxy::operator () (IField& field)
{
	m_archive.RegFieldImpl(field);
	return *this;
}


S11nBlocks::ArchiveImpl::CRegProxy ArchiveImpl::RegisterFields(IField& field)
{
	RegFieldImpl(field);
	return CRegProxy(*this);
}

void ArchiveImpl::SaveToStream(foobar_stream_writer& output)
{
	unsigned int numFields = 0;

	// Calculate number of fields to write and check whether all required fields are set.
	for (std::size_t i = 0; i < m_fields.size(); ++i)
	{
		if (m_fields[i]->Exists())
			++numFields;

		if (m_fields[i]->Required() && !m_fields[i]->Exists())
			throw Exception::RequiredFieldNotSet(m_fields[i]->GetID());
	}

	output << numFields;

	for (std::size_t i = 0; i < m_fields.size(); ++i)
	{
		IField* pField = m_fields[i];

		if (!pField->Exists())
			continue;

		output << pField->GetID();

		// Writing to byte buffer instead of writing directly to stream, to allow reading fields without
		// parsing them.
		foobar_stream_buffer_writer bufferStream;
		pField->SerializeToStream(bufferStream);

		output.write_byte_block(bufferStream.m_buffer);
	}
}

void ArchiveImpl::LoadFromStream(foobar_stream_reader& input)
{
	unsigned int numFields = 0;

	try
	{
		input >> numFields;
	}
	catch (exception_io&)
	{
		throw Exception::StreamReadError();
	}

	// Creating map of all existing fields.
	std::map<int, ByteBuffer> fieldsMap;

	for (unsigned int i = 0; i < numFields; ++i)
	{
		int fieldID = -1;

		try
		{
			input >> fieldID;
		}
		catch (exception_io&)
		{
			throw Exception::StreamReadError();
		}		

		// Reading field without parsing its structure.
		ByteBuffer buffer;

		try
		{
			input.read_byte_block(buffer);
		}
		catch (exception_io&)
		{
			throw Exception::StreamReadError();
		}	

		fieldsMap.insert(std::make_pair(fieldID, buffer));
	}

	// Check whether all required fields are read.
	for (std::size_t i = 0; i < m_fields.size(); ++i)
	{
		auto it = fieldsMap.find(m_fields[i]->GetID());

		// If a field is required but not found, exit.
		if (m_fields[i]->Required() && it == fieldsMap.end())
			throw Exception::RequiredFieldNotSet(m_fields[i]->GetID());
	}

	for (auto it = fieldsMap.begin(); it != fieldsMap.end(); ++it)
	{
		IField* pField = FindFieldWithID(it->first);

		// Found unknown id, ignore.
		if (pField == 0)
			continue;

		foobar_stream_buffer_reader bufferStream(it->second);
		pField->ParseFromStream(bufferStream);
	}
}

void ArchiveImpl::RegFieldImpl(IField& field)
{
	if (!FindFieldWithID(field.GetID()))
		m_fields.push_back(&field);
}

IField* ArchiveImpl::FindFieldWithID(int id) const
{
	for (std::size_t i = 0; i < m_fields.size(); ++i)
		if (m_fields[i]->GetID() == id)
			return m_fields[i];

	return 0;
}

} // namespace S11nBlocks