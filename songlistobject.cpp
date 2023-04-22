#include "songlistobject.h"

SongListObject::SongListObject(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	ui.artist->setStyleSheet("font-style: italic");
}

SongListObject::~SongListObject()
{
}

QString SongListObject::getSongName()
{
	return _song_name;
}
QString SongListObject::getSongNum()
{
	return _song_num;
}
QString SongListObject::getArtist()
{
	return _artist;
}

QString SongListObject::getDiscNum()
{
	return _disc_num;
}

void SongListObject::setSongName(const QString& name)
{
	_song_name = name;
	ui.song_title->setText(_song_name);
}
void SongListObject::setSongNum(const QString& num)
{
	_song_num = num;
	ui.num_song->setText(_song_num);
}
void SongListObject::setArtist(const QString& artist_name)
{
	_artist = artist_name;
	ui.artist->setText(_artist);
}

void SongListObject::setDiscNum(const QString& disc_num)
{
	_disc_num = disc_num;
	ui.disc_num->setText(_disc_num);
}
