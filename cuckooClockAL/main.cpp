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


#include <QApplication>
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QVBoxLayout>
#include <QCheckBox>
#include <QTimer>
#include <QTime>
#include <QPainter>
#include <QWidget>
#include <QtMath>


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


        class ClockWidget : public QOpenGLWidget, protected QOpenGLFunctions {
            Q_OBJECT

        public:
            explicit ClockWidget(QWidget *parent = nullptr) : QOpenGLWidget(parent), is24HourMode(false) {
                timer = new QTimer(this);
                connect(timer, &QTimer::timeout, this, QOverload<>::of(&ClockWidget::update));
                timer->start(1000); // Update every second
            }

            void set24HourMode(bool enabled) {
                is24HourMode = enabled;
                update();
            }

        protected:
            void initializeGL() override {
                initializeOpenGLFunctions();
                glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
            }

            void resizeGL(int w, int h) override {
                glViewport(0, 0, w, h);
            }

            void paintGL() override {
                glClear(GL_COLOR_BUFFER_BIT);

                glMatrixMode(GL_PROJECTION);
                glLoadIdentity();
                glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);

                glMatrixMode(GL_MODELVIEW);
                glLoadIdentity();

                drawClockFace();
                drawClockHands();
            }

        private:
            QTimer *timer;
            bool is24HourMode;

            void drawClockFace() {
                glColor3f(0.0f, 0.0f, 0.0f);

                // Draw hour markers
                for (int i = 0; i < 60; ++i) {
                    glPushMatrix();
                    glRotatef(i * 6.0f, 0.0f, 0.0f, 1.0f);

                    if (i % 5 == 0) {
                        glBegin(GL_LINES);
                        glVertex2f(0.0f, 0.9f);
                        glVertex2f(0.0f, 0.8f);
                        glEnd();
                    } else {
                        glBegin(GL_LINES);
                        glVertex2f(0.0f, 0.9f);
                        glVertex2f(0.0f, 0.85f);
                        glEnd();
                    }

                    glPopMatrix();
                }

                // Draw numbers using QPainter
                QPainter painter(this);
                painter.setRenderHint(QPainter::Antialiasing);
                painter.setPen(Qt::black);
                QFont font("Arial", 10);
                painter.setFont(font);

                for (int i = 1; i <= (is24HourMode ? 24 : 12); ++i) {
                    float angle = qDegreesToRadians(360.0f / (is24HourMode ? 24.0f : 12.0f) * i - 90.0f);
                    float x = 0.75f * qCos(angle);
                    float y = 0.75f * qSin(angle);

                    QPointF position(width() / 2 + x * width() / 2, height() / 2 + y * height() / 2);
                    painter.drawText(position, QString::number(i));
                }

                painter.end();
            }


            void drawClockHands() {
                QTime time = QTime::currentTime();

                // Hour hand
                glColor3f(0.0f, 0.0f, 0.0f);
                glPushMatrix();
                int hours = time.hour();
                if (!is24HourMode) {
                    hours = hours % 12;
                }
                float hourAngle = -30.0f * (hours % (is24HourMode ? 24 : 12)) - time.minute() / 2.0f;
                glRotatef(hourAngle, 0.0f, 0.0f, 1.0f);
                glBegin(GL_QUADS);
                glVertex2f(-0.02f, 0.0f);
                glVertex2f(0.02f, 0.0f);
                glVertex2f(0.01f, 0.5f);
                glVertex2f(-0.01f, 0.5f);
                glEnd();
                glPopMatrix();

                // Minute hand
                glColor3f(0.0f, 0.0f, 0.0f);
                glPushMatrix();
                float minuteAngle = -6.0f * time.minute() - time.second() / 10.0f;
                glRotatef(minuteAngle, 0.0f, 0.0f, 1.0f);
                glBegin(GL_QUADS);
                glVertex2f(-0.015f, 0.0f);
                glVertex2f(0.015f, 0.0f);
                glVertex2f(0.01f, 0.7f);
                glVertex2f(-0.01f, 0.7f);
                glEnd();
                glPopMatrix();

                // Second hand
                glColor3f(1.0f, 0.0f, 0.0f);
                glPushMatrix();
                float secondAngle = -6.0f * time.second();
                glRotatef(secondAngle, 0.0f, 0.0f, 1.0f);
                glBegin(GL_QUADS);
                glVertex2f(-0.01f, 0.0f);
                glVertex2f(0.01f, 0.0f);
                glVertex2f(0.005f, 0.8f);
                glVertex2f(-0.005f, 0.8f);
                glEnd();
                glPopMatrix();
            }
};


        class ClockWidget2 : public QOpenGLWidget {
            Q_OBJECT

        public:
            explicit ClockWidget2(QWidget *parent = nullptr) : QOpenGLWidget(parent), is24HourMode(false) {
                timer = new QTimer(this);
                connect(timer, &QTimer::timeout, this, QOverload<>::of(&ClockWidget::update));
                timer->start(1000); // Update every second
            }

            void set24HourMode(bool enabled) {
                is24HourMode = enabled;
                update();
            }

        protected:
            void paintEvent(QPaintEvent *event) override {
                Q_UNUSED(event);

                QPainter painter(this);
                painter.setRenderHint(QPainter::Antialiasing);

                int side = qMin(width(), height());
                painter.translate(width() / 2, height() / 2);
                painter.scale(side / 200.0, side / 200.0);

                drawClockFace(painter);
                drawClockHands(painter);
            }

        private:
            QTimer *timer;
            bool is24HourMode;

            void drawClockFace(QPainter &painter) {
                painter.setPen(Qt::NoPen);
                painter.setBrush(Qt::white);
                painter.drawEllipse(-100, -100, 200, 200);

                painter.setPen(Qt::black);
                for (int i = 0; i < 60; ++i) {
                    if (i % 5 == 0) {
                        painter.drawLine(0, -88, 0, -100);
                    } else {
                        painter.drawLine(0, -92, 0, -100);
                    }
                    painter.rotate(6.0);
                }

                painter.setFont(QFont("Arial", 8));
                for (int i = 1; i <= (is24HourMode ? 24 : 12); ++i) {
                    int angle = 360 / (is24HourMode ? 24 : 12) * i;
                    double x = 85 * qCos(qDegreesToRadians(angle - 90.0));
                    double y = 85 * qSin(qDegreesToRadians(angle - 90.0));
                    painter.drawText(QRectF(x - 10, y - 10, 20, 20), Qt::AlignCenter, QString::number(i));
                }
            }

            void drawClockHands(QPainter &painter) {
                QTime time = QTime::currentTime();

                painter.setPen(Qt::NoPen);
                painter.setBrush(Qt::black);

                // Hour hand
                painter.save();
                int hours = time.hour();
                if (!is24HourMode) {
                    hours = hours % 12;
                }
                painter.rotate(30.0 * ((hours % (is24HourMode ? 24 : 12)) + time.minute() / 60.0));
                painter.drawConvexPolygon(QPolygon({QPoint(-5, 0), QPoint(5, 0), QPoint(0, -50)}));
                painter.restore();

                // Minute hand
                painter.save();
                painter.rotate(6.0 * (time.minute() + time.second() / 60.0));
                painter.drawConvexPolygon(QPolygon({QPoint(-3, 0), QPoint(3, 0), QPoint(0, -70)}));
                painter.restore();

                // Second hand
                painter.setBrush(Qt::red);
                painter.save();
                painter.rotate(6.0 * time.second());
                painter.drawConvexPolygon(QPolygon({QPoint(-2, 0), QPoint(2, 0), QPoint(0, -90)}));
                painter.restore();
            }
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
    QCheckBox *halfsound;
    QPushButton *loadGrandSoundButton;
    ClockWidget *clockWidget;

    void setupUI() {
        auto *centralWidget = new QWidget(this);
        auto *layout = new QVBoxLayout(centralWidget);

        loadSoundButton = new QPushButton("Load Sound", this);
        connect(loadSoundButton, &QPushButton::clicked, this, &CuckooClock::loadSound);

        loadGrandSoundButton = new QPushButton("Load Grand Sound", this);
        connect(loadGrandSoundButton, &QPushButton::clicked, this, &CuckooClock::loadGrandSound);

        clockWidget = new ClockWidget(this);
layout->addWidget(clockWidget);
        halfHourChime = new QCheckBox("Enable Half-Hour Chime", this);
        grandclock = new QCheckBox("Enable grandclock", this);
        halfsound = new QCheckBox("Enable grand halfhour chime", this);
        halfsound->setChecked(1);
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
         layout->addWidget(halfsound);
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
        halfsound->setChecked(settings->value("halfsound", false).toBool());
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
        if (!loadWavFile(grandsoundFile.toStdString().c_str(),buffer2)) {
             loadWavFile("grandfclock.wav",buffer2);
        }
#else
         if ( !loadWavFile(soundFile.toStdString().c_str(),buffer)) {
              loadWavFile("/Applications/grandFatherClock.app/Contents/MacOS/grandfclock-chime.wav",buffer);
         }
         if (!loadWavFile(grandsoundFile.toStdString().c_str(),buffer2)) {
              loadWavFile("/Applications/grandFatherClock.app/Contents/MacOS/grandfclock.wav",buffer2);
         }
#endif


    }

    void saveSettings() {
        qDebug() <<  "save settings";
        settings->setValue("soundFile", soundFile);
        settings->setValue("grandsoundFile", grandsoundFile);
        settings->setValue("halfHourChime", halfHourChime->isChecked());
        settings->setValue("halfsound", halfsound->isChecked());
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

       // loadWavFile("/Applications/grandFatherClock.app/Contents/MacOS/grandfclock.wav",buffer2);
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
            if (halfsound->isChecked() && halfhour){
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

            }else               {  alSourcePlay(source);

            // Wait for the sound to finish playing
            ALint state;
            do {
                alGetSourcei(source, AL_SOURCE_STATE, &state);
            } while (state == AL_PLAYING);
            }
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
      //  QString test = filePath;
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
    clock.resize(600, 600);
    clock.show();

    return app.exec();
}

#include "main.moc"
