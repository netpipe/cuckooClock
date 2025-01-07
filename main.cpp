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
        timer->start(1000); // Update the time every second

        updateTime();

        cuckooSound = new QSoundEffect(this);
#ifdef __APPLE__
        cuckooSound->setSource(QUrl::fromLocalFile("/Applications/cuckooClock.app/Contents/MacOS/cuckoo.wav"));
#else
       cuckooSound->setSource(QUrl::fromLocalFile("cuckoo.wav"));
#endif

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

        if (currentTime.minute() == 0 && currentTime.second() <= 3) {
            playCuckoo(currentTime.hour());
        }
        if (currentTime.minute() == 30 && currentTime.second() <= 3) {
            playCuckoo(1);
        }
    }

    void playCuckoo(int hours) {
        for (int i = 0; i < hours; ++i) {
            QTimer::singleShot(i * 1000, cuckooSound, &QSoundEffect::play);
        }
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
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    CuckooClockWidget clockWidget;
    clockWidget.show();

    return app.exec();
}

#include "main.moc"
