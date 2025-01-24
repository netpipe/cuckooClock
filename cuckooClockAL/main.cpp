#include <QApplication>
#include <QMainWindow>
#include <QTimer>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QAction>
#include <QCheckBox>
#include <QPushButton>
#include <QFileDialog>
#include <QSettings>
#include <QVBoxLayout>
#include <QLabel>
#include <ctime>
#include <QTime>
//#include <AL/al.h>
//#include <AL/alc.h>
#include <QThread>
#include <AL/alut.h>
#include <QDebug>
#include <QSlider>



#include <QCloseEvent>
#include <fstream>
#include <iostream>

        ALuint buffer;
        float volume;
bool loaded=0;
bool bplay=0;
bool halfhour=0;
        struct WAVHeader {
            char riff[4];
            uint32_t chunkSize;
            char wave[4];
            char fmt[4];
            uint32_t subchunk1Size;
            uint16_t audioFormat;
            uint16_t numChannels;
            uint32_t sampleRate;
            uint32_t byteRate;
            uint16_t blockAlign;
            uint16_t bitsPerSample;
            char data[4];
            uint32_t dataSize;
        };

class CuckooClock : public QMainWindow {
    Q_OBJECT

public:
    CuckooClock() {
        setupUI();
        setupTrayIcon();
        setupTimer();
    }

    ~CuckooClock() {
        saveSettings();
        if (trayIcon) delete trayIcon;
    }

private:
    QPushButton *loadSoundButton;
    QCheckBox *halfHourChime;
    QLabel *statusLabel;
    QSystemTrayIcon *trayIcon;
    QMenu *trayMenu;
    QString soundFile;
    QTimer *clockTimer;
    QSettings *settings;
        QLabel *clockLabel;
QSlider *volumeSlider;
QCheckBox *grandclock;

    void setupUI() {
        auto *centralWidget = new QWidget(this);
        auto *layout = new QVBoxLayout(centralWidget);

        loadSoundButton = new QPushButton("Load Sound", this);
        connect(loadSoundButton, &QPushButton::clicked, this, &CuckooClock::loadSound);

        halfHourChime = new QCheckBox("Enable Half-Hour Chime", this);
        grandclock = new QCheckBox("Enable grandclock", this);

        statusLabel = new QLabel("Status: Ready", this);

        clockLabel = new QLabel(this);
        clockLabel->setAlignment(Qt::AlignCenter);
        clockLabel->setStyleSheet("font-size: 24px; font-weight: bold;");
        layout->addWidget(clockLabel);

        volumeSlider = new QSlider(Qt::Horizontal, this);
        volumeSlider->setRange(0, 100);
        layout->addWidget(volumeSlider);
        connect(volumeSlider, &QSlider::valueChanged, this, &CuckooClock::setVolume);

        layout->addWidget(loadSoundButton);
        layout->addWidget(halfHourChime);
        layout->addWidget(grandclock);
        layout->addWidget(statusLabel);

        setCentralWidget(centralWidget);
        setWindowTitle("Cuckoo Clock");
        resize(300, 150);


loadSettings();

if (grandclock->isChecked()){loaded=1;}
     //    volumeSlider->setValue(100);
       // loadWavFile("cuckoo.wav",buffer);
        // loadWavFile(soundFile.toStdString().c_str(),buffer);

//playSound(1);

    }

    void setupTrayIcon() {
        trayIcon = new QSystemTrayIcon(this);
#ifdef __APPLE__
        trayIcon->setIcon(QIcon("/Applications/grandFatherClock.app/Contents/MacOS/icon.png"));
#else
        trayIcon->setIcon(QIcon("icon.png"));
#endif
        trayMenu = new QMenu(this);

        auto *quitAction = new QAction("Quit", this);
        connect(quitAction, &QAction::triggered, this, &QApplication::quit);

        trayMenu->addAction(quitAction);
        trayIcon->setContextMenu(trayMenu);
        trayIcon->show();

        connect(trayIcon, &QSystemTrayIcon::activated, this, &CuckooClock::onTrayIconActivated);
    }

    void setVolume(float value) {
        //if (loaded) {
        volume = value;// / 100.0f; // Convert to 0.0 - 1.0 range
 //volumeSlider->setValue(volume);
 //qDebug() << value;
       // volume = volume;
       // saveVolume(value);

        saveSettings();
//}
    }

    void setupTimer() {
        clockTimer = new QTimer(this);
        connect(clockTimer, &QTimer::timeout, this, &CuckooClock::checkTime);
        clockTimer->start(500); // Check every second
    }

    void loadSettings() {
        settings = new QSettings("MyCompany", "grandFatherClock", this);
        soundFile = settings->value("soundFile", "").toString();
        halfHourChime->setChecked(settings->value("halfHourChime", false).toBool());
        grandclock->setChecked(settings->value("grandclock", false).toBool());
        volume = settings->value("volume", "").toFloat();
        //qDebug() << "test3" << volume;
        setVolume(volume);
        volumeSlider->setValue(volume);



       // if (soundFile.isEmpty()){
#ifndef __APPLE__
        if (!loadWavFile("cuckoo.wav",buffer)) {
            loadWavFile(soundFile.toStdString().c_str(),buffer);
        }
#else
         if (!loadWavFile("/Applications/grandFatherClock.app/Contents/MacOS/grandfclock-chime.wav",buffer)) {
             loadWavFile(soundFile.toStdString().c_str(),buffer);
         }
#endif


    }

    void saveSettings() {
        settings->setValue("soundFile", soundFile);
        settings->setValue("halfHourChime", halfHourChime->isChecked());
        settings->setValue("grandclock", grandclock->isChecked());
       //  qDebug() <<  "test" << volume;
        settings->setValue("volume", volume);
       // qDebug() << settings->value("volume", "").toFloat();

    }

    void playSound(int hour) {
        if (bplay){
    //    if (filePath.isEmpty()) {
     //       statusLabel->setText("Status: No sound file loaded");
    //        return;
     //   }
        alutInit(nullptr, nullptr);

       // ALuint buffer =
              //  loadWavFile(filePath,buffer);
        if (buffer == AL_NONE) {
            alutExit();
            return;
        }

        ALuint source;
        alGenSources(1, &source);
        alSourcei(source, AL_BUFFER, buffer);


        float value = volume / 100.0f;
        alSourcef(source, AL_GAIN, value);


        ALuint buffer2;
        ALuint source2;
        if (loaded && !halfhour ){

        loadWavFile("/Applications/grandFatherClock.app/Contents/MacOS/grandfclock.wav",buffer2);


        alGenSources(1, &source2);
        alSourcei(source2, AL_BUFFER, buffer2);

        alSourcef(source2, AL_GAIN, value);
        alSourcePlay(source2);
        ALint state;
            do {
                alGetSourcei(source2, AL_SOURCE_STATE, &state);
            } while (state == AL_PLAYING);


        }

        for (int i = 0; i < hour; ++i) {
            alSourcePlay(source);

            // Wait for the sound to finish playing
            ALint state;
            do {
                alGetSourcei(source, AL_SOURCE_STATE, &state);
            } while (state == AL_PLAYING);

            // Optional: small delay between chimes
           // QThread::sleep(1);
        }

        // Clean up
        alDeleteSources(1, &source);
        alDeleteSources(1, &source2);
        alDeleteBuffers(1, &buffer);
        alDeleteBuffers(1, &buffer2);
        alutExit();
        statusLabel->setText("Status: Chime played");
        }
    }

    bool loadWavFile(const char* filePath,ALuint &buffer) {
        std::ifstream file(filePath, std::ios::binary);
        qDebug() << filePath;
        if (!file.is_open()) {
            std::cerr << "Failed to open WAV file: " << filePath << std::endl;
            return 0;
        }

        WAVHeader header;
        file.read(reinterpret_cast<char*>(&header), sizeof(WAVHeader));

        if (std::strncmp(header.riff, "RIFF", 4) != 0 || std::strncmp(header.wave, "WAVE", 4) != 0) {
            std::cerr << "Invalid WAV file format: " << filePath << std::endl;
            return 0;
        }

        ALenum format;
        if (header.numChannels == 1) {
            format = (header.bitsPerSample == 8) ? AL_FORMAT_MONO8 : AL_FORMAT_MONO16;
        } else if (header.numChannels == 2) {
            format = (header.bitsPerSample == 8) ? AL_FORMAT_STEREO8 : AL_FORMAT_STEREO16;
        } else {
            std::cerr << "Unsupported channel count: " << header.numChannels << std::endl;
            return 0;
        }

        std::vector<char> data(header.dataSize);
        file.read(data.data(), header.dataSize);
        file.close();

        //ALuint buffer;
        alGenBuffers(1, &buffer);
        alBufferData(buffer, format, data.data(), header.dataSize, header.sampleRate);

       // return buffer;
    }


    void checkTime() {
        time_t now = time(nullptr);
        struct tm *localTime = localtime(&now);

        QTime currentTime = QTime::currentTime();
        clockLabel->setText(currentTime.toString("hh:mm:ss ap"));

        int hour = localTime->tm_hour % 12;
        if (hour == 0) hour = 12;  // Convert 0-hour to 12 for 12-hour format

        if (currentTime.minute() == 0 && currentTime.second() <= 10) {
            int hour12 = currentTime.hour() % 12;
            if (hour12 == 0) hour12 = 12; // Convert 0 to 12 for 12-hour format
            playSound(hour12);
            bplay=false;
        }else if (halfHourChime->isChecked() && currentTime.minute() == 30 && currentTime.second() <= 10) {
             halfhour=true;
            playSound(1); // Play one cuckoo sound for the half-hour chime
            bplay=false;
        }else{
            bplay=true;
            halfhour=false;
        }


    }

private slots:
    void loadSound() {
        QString file = QFileDialog::getOpenFileName(this, "Select WAV File", "", "WAV Files (*.wav)");
loadWavFile(file.toLatin1(),buffer);
      //  if (!loadWavFile(soundFile.toStdString().c_str(),buffer) ) {
      //      statusLabel->setText("Status: Failed to load sound file");
        //    return;
       // }
soundFile = file.toLatin1();
saveSettings();
//        if (!file.isEmpty()) {
//            soundFile = file;
           statusLabel->setText("Status: Sound file loaded");
//        }
    }

    void closeEvent(QCloseEvent *event) override {
        if (trayIcon && trayIcon->isVisible()) {
            this->hide();
            event->ignore();
        }
        saveSettings();
    }

    void onTrayIconActivated(QSystemTrayIcon::ActivationReason reason) {
        if (reason == QSystemTrayIcon::Trigger) {
            this->show();
        }
    }
};


int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    CuckooClock clock;
    clock.show();

    return app.exec();
}

#include "main.moc"
