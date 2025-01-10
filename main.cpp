#include <QApplication>
#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QTimer>
#include <QTime>
#include <QSoundEffect>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QCloseEvent>
#include <QCheckBox>
#include <QPushButton>
#include <QTextStream>
#include <QFileDialog>
#include <QSlider>

bool bplay=0;
class CuckooClockWidget : public QWidget {
    Q_OBJECT

public:
    CuckooClockWidget(QWidget *parent = nullptr) : QWidget(parent) {
        setWindowTitle("Cuckoo Clock");
        resize(300, 150);

        QVBoxLayout *layout = new QVBoxLayout(this);

        clockLabel = new QLabel(this);
        clockLabel->setAlignment(Qt::AlignCenter);
        clockLabel->setStyleSheet("font-size: 24px; font-weight: bold;");
        layout->addWidget(clockLabel);

        QTimer *timer = new QTimer(this);
        connect(timer, &QTimer::timeout, this, &CuckooClockWidget::updateTime);
        timer->start(500); // Update the time every second



        QPushButton *selectSoundButton = new QPushButton("Select Cuckoo Sound", this);
        layout->addWidget(selectSoundButton);
        connect(selectSoundButton, &QPushButton::clicked, this, &CuckooClockWidget::selectSound);

        halfHourChimeCheckBox = new QCheckBox("Enable Half-Hour Chime", this);
        layout->addWidget(halfHourChimeCheckBox);
        halfHourChimeCheckBox->setChecked(1);

        updateTime();

        cuckooSound = new QSoundEffect(this);
        #ifdef __APPLE__
        cuckooSound->setSource(QUrl::fromLocalFile("/Applications/cuckooClock.app/Contents/MacOS/cuckoo.wav"));
        #else
       cuckooSound->setSource(QUrl::fromLocalFile("cuckoo.wav"));
        #endif

       loadSound(); // Load the saved sound file path on startup


       QSlider *volumeSlider = new QSlider(Qt::Horizontal, this);
       volumeSlider->setRange(0, 100);
       layout->addWidget(volumeSlider);
       connect(volumeSlider, &QSlider::valueChanged, this, &CuckooClockWidget::setVolume);

       loadVolume(volumeSlider); // Load saved volume on startup


        cuckooSound->setVolume(100);
        createTrayIcon();
    }

    ~CuckooClockWidget() {
        delete trayIcon;
    }

protected:
    void closeEvent(QCloseEvent *event) override {
        if (trayIcon && trayIcon->isVisible()) {
            this->hide();
            event->ignore();
        }
    }

private slots:
    void updateTime() {
        QTime currentTime = QTime::currentTime();
        clockLabel->setText(currentTime.toString("hh:mm:ss ap"));

        if (currentTime.minute() == 0 && currentTime.second() <= 10) {
            int hour12 = currentTime.hour() % 12;
            if (hour12 == 0) hour12 = 12; // Convert 0 to 12 for 12-hour format
            playCuckoo(hour12);
             bplay=true;
        }else if (halfHourChimeCheckBox->isChecked() && currentTime.minute() == 30 && currentTime.second() <= 10) {
            playCuckoo(1); // Play one cuckoo sound for the half-hour chime
            bplay=true;
        }       else
        {
            bplay=false;
        }

        if ( currentTime.minute() == 55 && currentTime.second() <= 5) {
        #ifdef __APPLE__
            trayIcon->setIcon(QIcon("/Applications/cuckooClock.app/Contents/MacOS/Icon2.png"));
        #else
           trayIcon->setIcon(QIcon("Icon2.png"));
        #endif
        }
        if (halfHourChimeCheckBox->isChecked() && currentTime.minute() == 25 && currentTime.second() <= 5) {
        #ifdef __APPLE__
             trayIcon->setIcon(QIcon("/Applications/cuckooClock.app/Contents/MacOS/Icon2.png"));
        #else
            trayIcon->setIcon(QIcon("Icon2.png"));
        #endif
         }

    }

    void selectSound() {
        #ifdef __APPLE__
        QString soundFilePath = QFileDialog::getOpenFileName(this, "Select Cuckoo Sound", "/Applications/cuckooClock.app/Contents/MacOS/", "Sound Files (*.wav)");
        #else
         QString soundFilePath = QFileDialog::getOpenFileName(this, "Select Cuckoo Sound", "./", "Sound Files (*.wav)");
        #endif
        if (!soundFilePath.isEmpty()) {
            cuckooSound->setSource(QUrl::fromLocalFile(soundFilePath));
            saveSound(soundFilePath);
        }
    }

    void saveSound(const QString &filePath) {
        #ifdef __APPLE__
        QFile file("/Applications/cuckooClock.app/Contents/MacOS/cuckoo_sound.txt");
        #else
        QFile file("cuckoo_sound.txt");
        #endif
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&file);
            out << filePath;
            file.close();
        }
    }

    void loadSound() {
        #ifdef __APPLE__
        QFile file("/Applications/cuckooClock.app/Contents/MacOS/cuckoo_sound.txt");
        #else
        QFile file("cuckoo_sound.txt");
        #endif

        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream in(&file);
            QString filePath = in.readLine();
            file.close();
            if (!filePath.isEmpty() && QFile::exists(filePath)) {
                cuckooSound->setSource(QUrl::fromLocalFile(filePath));
            }
        }
    }

    void setVolume(int value) {
        float volume = value / 100.0f; // Convert to 0.0 - 1.0 range
        cuckooSound->setVolume(volume);
        saveVolume(value);
    }

    void saveVolume(int value) {
        #ifdef __APPLE__
            QFile file("/Applications/cuckooClock.app/Contents/MacOS/cuckoo_volume.txt");
        #else
             QFile file("cuckoo_volume.txt");
        #endif
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&file);
            out << value;
            file.close();
        }
    }

    void loadVolume(QSlider *slider) {
        #ifdef __APPLE__
        QFile file("/Applications/cuckooClock.app/Contents/MacOS/cuckoo_volume.txt");
        #else
        QFile file("cuckoo_volume.txt");
        #endif
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream in(&file);
            int value;
            in >> value;
            file.close();
            slider->setValue(value);
            setVolume(value);
        } else {
            slider->setValue(50); // Default volume
            setVolume(50);
        }
    }

    void playCuckoo(int hours) {
        if (!bplay) {
        cuckooSound->setLoopCount(1);
        #ifdef __APPLE__
        trayIcon->setIcon(QIcon("/Applications/cuckooClock.app/Contents/MacOS/Icon3.png"));
         #else
        trayIcon->setIcon(QIcon("/Applications/cuckooClock.app/Contents/MacOS/Icon3.png"));
        #endif

        for (int i = 0; i < hours; ++i) {
            QTimer::singleShot(i * 1000, cuckooSound, &QSoundEffect::play);
          //  cuckooSound->play();
        }
        }
        #ifdef __APPLE__
        trayIcon->setIcon(QIcon("/Applications/cuckooClock.app/Contents/MacOS/Icon.png"));
        #else
        trayIcon->setIcon(QIcon("Icon.png"));
        #endif

    }

    void showMainWidget() {
        this->show();
        this->raise();
        this->activateWindow();
    }

    void exitApplication() {
        QApplication::quit();
    }

private:
    void createTrayIcon() {
        trayIcon = new QSystemTrayIcon(this);

#ifdef __APPLE__
               trayIcon->setIcon(QIcon("/Applications/cuckooClock.app/Contents/MacOS/Icon.png"));
#else
        trayIcon->setIcon(QIcon("Icon.png"));
#endif

        trayIcon->setToolTip("Cuckoo Clock");

        QMenu *trayMenu = new QMenu(this);
        QAction *showAction = new QAction("Show Cuckoo Clock", this);
        QAction *exitAction = new QAction("Exit", this);

        connect(showAction, &QAction::triggered, this, &CuckooClockWidget::showMainWidget);
        connect(exitAction, &QAction::triggered, this, &CuckooClockWidget::exitApplication);

        trayMenu->addAction(showAction);
        trayMenu->addAction(exitAction);

        trayIcon->setContextMenu(trayMenu);
        trayIcon->show();
    }

    QLabel *clockLabel;
    QSoundEffect *cuckooSound;
    QSystemTrayIcon *trayIcon;
        QCheckBox *halfHourChimeCheckBox;
};




int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    CuckooClockWidget clockWidget;
    clockWidget.show();

    return app.exec();
}

#include "main.moc"
