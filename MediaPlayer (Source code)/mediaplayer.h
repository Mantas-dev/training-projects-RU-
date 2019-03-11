#ifndef MEDIAPLAYER_H
#define MEDIAPLAYER_H

#include <QWidget>
#include <QFileDialog>
#include <QStyleFactory>
#include <QUrl>
#include <QMediaPlayer>
#include <QMediaPlaylist>
#include <QStandardItemModel>
#include <QStyle>

namespace Ui {
class MediaPlayer;
}

class MediaPlayer : public QWidget
{
    Q_OBJECT

public:
    explicit MediaPlayer(QWidget *parent = 0);
    ~MediaPlayer();

protected:
    QMediaPlayer        *m_player;
    QMediaPlaylist      *m_playlist;
    QStandardItemModel  *m_playlistModel;

private:
    Ui::MediaPlayer *ui;
    QString msecsToString(qint64);
    QString getFileNameWithoutExt(const QString &);
    void    addDurationToSongName(const qint64 &);
    void    initializePlayerButtons(QPushButton *, QStyle::StandardPixmap);

private slots:
    void slotOpen();
    void slotPlay();
    void slotSetSliderPosition(qint64);
    void slotSetMediaPosition(int);
    void slotSetDuration(qint64);
    void slotStatusChanged(QMediaPlayer::State);
    void slotSetTextCurrentSong(int);
    void slotSetPlaylistCurrentIndex(const QModelIndex &);
};

#endif // MEDIAPLAYER_H
