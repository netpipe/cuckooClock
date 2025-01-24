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
        ALuint buffer2;
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

    QString grandsoundFile;
    QTimer *clockTimer;
    QSettings *settings;
        QLabel *clockLabel;
QSlider *volumeSlider;
QCheckBox *grandclock;
QPushButton *loadGrandSoundButton;

    void setupUI() {
        auto *centralWidget = new QWidget(this);
        auto *layout = new QVBoxLayout(centralWidget);

        loadSoundButton = new QPushButton("Load Sound", this);
        connect(loadSoundButton, &QPushButton::clicked, this, &CuckooClock::loadSound);

        loadGrandSoundButton = new QPushButton("Load Grand Sound", this);
        connect(loadGrandSoundButton, &QPushButton::clicked, this, &CuckooClock::loadGrandSound);

        halfHourChime = new QCheckBox("Enable Half-Hour Chime", this);
        grandclock = new QCheckBox("Enable grandclock", this);
        grandclock->setChecked(1);
        halfHourChime->setChecked(1);

        statusLabel = new QLabel("Status: Ready", this);

        clockLabel = new QLabel(this);
        clockLabel->setAlignment(Qt::AlignCenter);
        clockLabel->setStyleSheet("font-size: 24px; font-weight: bold;");
        layout->addWidget(clockLabel);

        volumeSlider = new QSlider(Qt::Horizontal, this);
        volumeSlider->setRange(0, 100);
        volumeSlider->setValue(100);
        layout->addWidget(volumeSlider);
        connect(volumeSlider, &QSlider::valueChanged, this, &CuckooClock::setVolume);

        layout->addWidget(loadSoundButton);
        layout->addWidget(loadGrandSoundButton);
        layout->addWidget(halfHourChime);
        layout->addWidget(grandclock);
        layout->addWidget(statusLabel);

        setCentralWidget(centralWidget);
        setWindowTitle("GrandFather Clock");
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
        grandsoundFile = settings->value("grandsoundFile", "").toString();
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
         if ( !loadWavFile(soundFile.toStdString().c_str(),buffer)) {
              loadWavFile("/Applications/grandFatherClock.app/Contents/MacOS/grandfclock-chime.wav",buffer);
         }
#endif


    }

    void saveSettings() {
        qDebug() <<  "save settings";
        settings->setValue("soundFile", soundFile);
        settings->setValue("grandsoundFile", soundFile);
        settings->setValue("halfHourChime", halfHourChime->isChecked());
        settings->setValue("grandclock", grandclock->isChecked());
       //  qDebug() <<  "test" << volume;
        settings->setValue("volume", volume);
       // qDebug() << settings->value("volume", "").toFloat();

    }

    bool playSound(int hour) {
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
            return 0;
        }

        ALuint source;
        alGenSources(1, &source);
        alSourcei(source, AL_BUFFER, buffer);


        float value = volume / 100.0f;
        alSourcef(source, AL_GAIN, value);



        ALuint source2;
        if (loaded && !halfhour ){

        //loadWavFile("/Applications/grandFatherClock.app/Contents/MacOS/grandfclock.wav",buffer2);
#ifndef __APPLE__
loadWavFile("grandfclock.wav",buffer2);
#else
           if (!loadWavFile(grandsoundFile.toStdString().c_str(),buffer2)) {

                loadWavFile("/Applications/grandFatherClock.app/Contents/MacOS/grandfclock.wav",buffer2);
           }
#endif

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
      //  alDeleteBuffers(1, &buffer);
        alDeleteBuffers(1, &buffer2);
        alutExit();
        statusLabel->setText("Status: Chime played");
        }
        return 1;
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

        return 1;
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


    #ifdef __APPLE__
    QString file = QFileDialog::getOpenFileName(this, "Select Cuckoo Sound", "/Applications/grandFatherClock.app/Contents/MacOS/", "Sound Files (*.wav)");
    #else
     QString file = QFileDialog::getOpenFileName(this, "Select Cuckoo Sound", "./", "Sound Files (*.wav)");
    #endif


    loadWavFile(file.toLatin1(),buffer);
      //  if (!loadWavFile(file.toLatin1(),buffer) ) {
      //      statusLabel->setText("Status: Failed to load sound file");
        //    return;
       // }
    soundFile = file.toLatin1();
    if (!soundFile.isEmpty()){
    saveSettings();
    }
//        if (!file.isEmpty()) {
//            soundFile = file;
           statusLabel->setText("Status: Sound file loaded");
//        }
    }

    void loadGrandSound() {


    #ifdef __APPLE__
    QString file = QFileDialog::getOpenFileName(this, "Select Cuckoo Sound", "/Applications/grandFatherClock.app/Contents/MacOS/", "Sound Files (*.wav)");
    #else
     QString file = QFileDialog::getOpenFileName(this, "Select Cuckoo Sound", "./", "Sound Files (*.wav)");
    #endif


    loadWavFile(file.toLatin1(),buffer2);
      //  if (!loadWavFile(file.toLatin1(),buffer) ) {
      //      statusLabel->setText("Status: Failed to load sound file");
        //    return;
       // }
    soundFile = file.toLatin1();
    if (!soundFile.isEmpty()){
    saveSettings();
    }
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
