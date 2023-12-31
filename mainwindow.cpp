#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QStyle>

MainWindow::MainWindow(QWidget *parent) :
        QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);

    connect (ui->actionDownload, &QAction::triggered,
             this, &MainWindow::download);
    connect (ui->actionExit, &QAction::triggered,
             this, &QApplication::quit);
    connect (ui->actionOpen, &QAction::triggered,
            this, &MainWindow::open);

    ui->actionDownload->setIcon(style()->standardIcon(QStyle::SP_ArrowDown));
    ui->actionExit->setIcon(style()->standardIcon(QStyle::SP_DialogCloseButton));
    ui->actionOpen->setIcon(style()->standardIcon(QStyle::SP_DialogOpenButton));
    ui->actionSave->setIcon(style()->standardIcon(QStyle::SP_DialogSaveButton));

    resize(1500, 1000);
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event) {
    QApplication::quit();
    event->accept();
}

void MainWindow::download() {
    downldWindow = new Download(this);
   downldWindow->show();
}

void MainWindow::open() {
    QFileDialog *fDialog = new QFileDialog(this);
    fDialog->setAttribute(Qt::WA_DeleteOnClose);

    QString fileName = fDialog->getOpenFileName(this, "Open Document", QDir::cleanPath("/app/Datasets/"),
                                                "CSV files (*.csv)", nullptr, QFileDialog::DontUseNativeDialog);

    if (fileName.isEmpty()) {
        return;
    }

    QMdiSubWindow *existing = findMdiChild(fileName);
    if (existing) {
        ui->mdiArea->setActiveSubWindow(existing);
        return;
    }

    // Если такого файла ещё нет, то создаём новое окно
    MdiChild *child = createMdiChild();
    // и загружаем файл
    if (child->loadFile(fileName)) {
        // О чём сообщаем пользователю через нижнюю часть окна
        ui->statusbar->showMessage(tr("File loaded"), 2000);
        // Показываем окно
        child->show();
    } else {
        // Если загрузить не удалось, то удаляем только что
        // созданный экземпляр окна
        child->close();
    }
}

MdiChild *MainWindow::createMdiChild() {
    MdiChild *child = new MdiChild;
    ui->mdiArea->addSubWindow(child);

    return child;
}

MdiChild *MainWindow::activeMdiChild() {
    if (QMdiSubWindow *activeSubWindow = ui->mdiArea->activeSubWindow())
        return qobject_cast<MdiChild *>(activeSubWindow->widget());

    return nullptr;
}

QMdiSubWindow *MainWindow::findMdiChild(const QString &fileName) {
    QString canonicalFilePath = QFileInfo(fileName).canonicalFilePath();

    for (auto *window : ui->mdiArea->subWindowList()) {
        MdiChild *mdiChild = qobject_cast<MdiChild *>(window->widget());
        if (mdiChild && mdiChild->currentFile() == canonicalFilePath) {
            return window;
        }
    }

    return nullptr;
}
