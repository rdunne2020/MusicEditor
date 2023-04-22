#pragma once
#include <string>
#include <exception>
#include <utility>
#include <FLAC++/metadata.h>

struct FlacException : public std::exception 
{
	const char* what() const throw ()
	{
		return "Could Not Read FLAC File";
	}
};

// TODO: Create a parent Container class to inherit from so the
// List of files can be a default container for music
class FlacContainer
{
public:
	FlacContainer() : _file_path(""), _disc_num(0), _track_num(0) {};
	FlacContainer(std::string path_to_song) :	_file_path(path_to_song),
												_disc_num(0),
												_track_num(0){};
	std::string _title;
	std::string _artist;
	std::string _file_path;
	int _disc_num = 0;
	int _track_num = 0;

	std::pair<std::string, std::string> get_metadata_tag(const std::string& comment_key);

	void set_vorbis_block();
	void set_metadata(std::string comment_key, std::string comment_value);

private:
	//FLAC::Metadata::Chain _song_chain;
	bool _chain_read = false;
	FLAC::Metadata::VorbisComment _comment_block;
};
