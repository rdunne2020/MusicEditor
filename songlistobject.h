#pragma once

#include <QWidget>
#include "flaccontainer.h"
#include "ui_songlistobject.h"

class SongListObject : public QWidget
{
	Q_OBJECT

public:
	SongListObject(QWidget *parent = nullptr);
	~SongListObject();
	QString getSongName();
	QString getSongNum();
	QString getArtist();
	QString getDiscNum();

	void setSongName(const QString &name);
	void setSongNum(const QString& num);
	void setArtist(const QString& artist_name);
	void setDiscNum(const QString& disc_num);

	FlacContainer flac_data;

private:
	QString _song_name;
	QString _song_num;
	QString _artist;
	QString _disc_num;
	Ui::SongListObject ui;
};
