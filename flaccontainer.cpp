#include <iostream>
#include "flaccontainer.h"

std::pair<std::string, std::string> FlacContainer::get_metadata_tag(const std::string& comment_key)
{
	std::pair<std::string, std::string> comment_block;
	comment_block.first = "NULL";
	comment_block.second = "NULL";
	if (_chain_read)
	{
		int title_entry_idx = _comment_block.find_entry_from(0, comment_key.c_str());
		if (title_entry_idx > -1)
		{
			FLAC::Metadata::VorbisComment::Entry requested_entry = _comment_block.get_comment(title_entry_idx);
			comment_block.first = requested_entry.get_field_name();
			comment_block.second = requested_entry.get_field_value();
		}
	}
	return comment_block;
}

void FlacContainer::set_vorbis_block()
{
	FLAC::Metadata::Chain song_chain;
	if (!song_chain.is_valid() || !song_chain.read(_file_path.c_str()))
	{
		std::cerr << "Could Not Read FLAC File: " << _file_path << std::endl;
		throw FlacException();
	}
	_chain_read = true;
	// Chain read successfully
	// Create new iterator to loop thru metadata
	FLAC::Metadata::Iterator it;
	it.init(song_chain);
	
	do
	{
		FLAC::Metadata::Prototype* pPtype = nullptr;
		pPtype = static_cast<FLAC::Metadata::Prototype*>(it.get_block());
		if (pPtype->get_type() == FLAC__METADATA_TYPE_VORBIS_COMMENT)
		{
			// Need to store it like this because it makes a copy of it. Else the memory is freed
			_comment_block = static_cast<FLAC::Metadata::VorbisComment>(*pPtype);
			break;
		}
	}
	while (it.next());

	if (!_comment_block.is_valid() || _comment_block.get_type() != FLAC__METADATA_TYPE_VORBIS_COMMENT)
	{
		std::cerr << "Comment Block Not Allocatead" << std::endl;
		throw FlacException();
	}
}

void FlacContainer::set_metadata(std::string comment_key, std::string comment_value)
{
	FLAC::Metadata::Chain song_chain;
	if (!song_chain.is_valid() || !song_chain.read(_file_path.c_str()))
	{
		std::cerr << "Could Not Read FLAC File: " << _file_path << std::endl;
		throw FlacException();
	}
	FLAC::Metadata::VorbisComment::Entry new_comment(comment_key.c_str(), comment_value.c_str());
	_comment_block.replace_comment(new_comment, false);
	song_chain.write();
}
