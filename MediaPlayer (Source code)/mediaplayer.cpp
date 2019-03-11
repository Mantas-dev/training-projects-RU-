#include "mediaplayer.h"
#include "ui_mediaplayer.h"

MediaPlayer::MediaPlayer(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MediaPlayer)
{
    ui->setupUi(this);

    int defaultVolume = 50;

    m_player = new QMediaPlayer(this);
    m_playlist = new QMediaPlaylist(m_player);
    m_playlistModel = new QStandardItemModel(this);

    m_player->setPlaylist(m_playlist);
    m_player->setVolume(defaultVolume);

    ui->playlistView->setModel(m_playlistModel);
    ui->playlistView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->playlistView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->playlistView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    initializePlayerButtons(ui->PlayButton, QStyle::SP_MediaPlay);
    initializePlayerButtons(ui->StopButton, QStyle::SP_MediaStop);
    initializePlayerButtons(ui->PrevButton, QStyle::SP_MediaSkipBackward);
    initializePlayerButtons(ui->NextButton, QStyle::SP_MediaSkipForward);

    ui->PositionSlider->setRange(0, 0);

    ui->VolumeSlider->setRange(0, 100);
    ui->VolumeSlider->setValue(defaultVolume);

    ui->labelCurrent->setText(msecsToString(0));
    ui->labelRemain->setText(msecsToString(0));

    ui->labelCurrentSong->setText("");

    connect(ui->OpenButton, SIGNAL(clicked()), SLOT(slotOpen()));
    connect(ui->PlayButton, SIGNAL(clicked()), SLOT(slotPlay()));
    connect(ui->StopButton, SIGNAL(clicked()), m_player, SLOT(stop()));
    connect(ui->NextButton, SIGNAL(clicked()), m_playlist, SLOT(next()));
    connect(ui->PrevButton, SIGNAL(clicked()), m_playlist, SLOT(previous()));
    connect(ui->VolumeSlider, SIGNAL(valueChanged(int)), m_player, SLOT(setVolume(int)));
    connect(ui->PositionSlider, SIGNAL(sliderMoved(int)), SLOT(slotSetMediaPosition(int)));
    connect(m_player, SIGNAL(positionChanged(qint64)), SLOT(slotSetSliderPosition(qint64)));
    connect(m_player, SIGNAL(durationChanged(qint64)), SLOT(slotSetDuration(qint64)));
    connect(m_player, SIGNAL(stateChanged(QMediaPlayer::State)), SLOT(slotStatusChanged(QMediaPlayer::State)));
    connect(m_playlist, SIGNAL(currentIndexChanged(int)), SLOT(slotSetTextCurrentSong(int)));
    connect(ui->playlistView, SIGNAL(doubleClicked(const QModelIndex &)), SLOT(slotSetPlaylistCurrentIndex(const QModelIndex &)));
}

MediaPlayer::~MediaPlayer()
{
    delete ui;
    delete m_playlistModel;
    delete m_playlist;
    delete m_player;
}

void MediaPlayer::initializePlayerButtons(QPushButton *button, QStyle::StandardPixmap pixmap)
{
    button->setText("");
    button->setEnabled(false);
    button->setIcon(style()->standardIcon(pixmap));
}

void MediaPlayer::slotOpen()
{
    QStringList files = QFileDialog::getOpenFileNames(this,"Open File", "", "Audio files (*.mp3)");
    foreach (QString filePath, files)
    {
        m_playlistModel->appendRow(new QStandardItem(getFileNameWithoutExt(filePath)));
        m_playlist->addMedia(QUrl(filePath));
        ui->PlayButton->setEnabled(true);
        ui->StopButton->setEnabled(true);
        ui->NextButton->setEnabled(true);
        ui->PrevButton->setEnabled(true);
    }
}

void MediaPlayer::slotPlay()
{
    switch (m_player->state())
    {
        case QMediaPlayer::PlayingState:
            m_player->pause();
            break;
        default:
            m_player->play();
            break;
    }
}

void MediaPlayer::slotStatusChanged(QMediaPlayer::State state)
{
    switch (state)
    {
        case QMediaPlayer::PlayingState:
            ui->PlayButton->setIcon(style()->standardIcon(QStyle::SP_MediaPause));
            break;
        default:
            ui->PlayButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
            break;
    }
}

void MediaPlayer::slotSetMediaPosition(int n)
{
    m_player->setPosition(n);
}

void MediaPlayer::slotSetDuration(qint64 n)
{
    if (n != 0)
    {
        addDurationToSongName(n);
    }
    ui->PositionSlider->setRange(0, n);
    ui->labelCurrent->setText(msecsToString(0));
    ui->labelRemain->setText(msecsToString(n));
}

void MediaPlayer::slotSetSliderPosition(qint64 n)
{
    int duration = ui->PositionSlider->maximum();
    ui->PositionSlider->setValue(n);
    ui->labelCurrent->setText(msecsToString(n));
    ui->labelRemain->setText(msecsToString(duration - n));
}

QString MediaPlayer::msecsToString(qint64 n)
{
    QString result;

    int minutes = ((n % (60 * 60 * 1000)) / (60 * 1000));
    int seconds = ((n % (60 * 1000)) / 1000);

    QTextStream(&result) << minutes << ":" << QString::number(seconds).rightJustified(2, '0');

    return result;
}

QString MediaPlayer::getFileNameWithoutExt(const QString &path)
{
    QUrl url(path);
    QString resultFileName = "";
    QStringList splittedFileName = url.fileName().split('.');
    for (int i = 0; i < splittedFileName.size() - 1; i++)
    {
        resultFileName += splittedFileName.at(i);
        if (i < splittedFileName.size() - 2)
            resultFileName += '.';
    }
    return resultFileName;
}

void MediaPlayer::slotSetTextCurrentSong(int index)
{
    QString currentSong = m_playlistModel->data(m_playlistModel->index(index, 0)).toString();
    currentSong.remove(QRegExp("\\[[0-9]*\\:[0-9]{2}\\]"));
    ui->labelCurrentSong->setText(currentSong);
}

void MediaPlayer::slotSetPlaylistCurrentIndex(const QModelIndex &index)
{
    m_playlist->setCurrentIndex(index.row());
    ui->StopButton->click();
    ui->PlayButton->click();
}

void MediaPlayer::addDurationToSongName(const qint64 &duration)
{
    QRegExp re(".*\\[[0-9]*\\:[0-9]{2}\\]");
    int index = m_playlist->currentIndex();
    QModelIndex modelIndex = m_playlistModel->index(index, 0);
    QVariant dataFromModel = m_playlistModel->data(modelIndex);
    ui->playlistView->setCurrentIndex(modelIndex);
    if (re.exactMatch(dataFromModel.toString()))
        return;
    QString renamedSong = dataFromModel.toString();
    renamedSong += " [" + msecsToString(duration) + "]";
    dataFromModel = QVariant(renamedSong);
    m_playlistModel->setData(modelIndex, dataFromModel);
}
