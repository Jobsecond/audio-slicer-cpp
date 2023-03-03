#include <QFileDialog>
#include <QMessageBox>
#include <QString>
#include <QStringList>
#include <QRegularExpression>
#include <QValidator>
#include <QThreadPool>
#include <QRunnable>
#include <QSysInfo>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "workthread.h"

#include "app_config.h"



MainWindow::MainWindow(QWidget *parent) :
        QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    m_threadpool = new QThreadPool(this);
    m_threadpool->setMaxThreadCount(1);

    connect(ui->pushButtonAddFiles, SIGNAL(clicked(bool)),
            this, SLOT(slot_add_audio_files()));
    connect(ui->pushButtonBrowse, SIGNAL(clicked(bool)),
            this, SLOT(slot_browse_output_dir()));
    connect(ui->pushButtonClearList, SIGNAL(clicked(bool)),
            this, SLOT(slot_clear_audio_list()));
    connect(ui->pushButtonAbout, SIGNAL(clicked(bool)),
            this, SLOT(slot_about()));
    connect(ui->pushButtonStart, SIGNAL(clicked(bool)),
            this, SLOT(slot_start()));

    ui->progressBar->setMinimum(0);
    ui->progressBar->setMaximum(100);
    ui->progressBar->setValue(0);

    auto validator = new QRegularExpressionValidator(QRegularExpression("\\d+"));
    ui->lineEditThreshold->setValidator(new QDoubleValidator());
    ui->lineEditMinLen->setValidator(validator);
    ui->lineEditMinInterval->setValidator(validator);
    ui->lineEditHopSize->setValidator(validator);
    ui->lineEditMaxSilence->setValidator(validator);

    m_workTotal = 0;
    m_workFinished = 0;
    m_processing = false;

    setWindowTitle(QApplication::applicationName());
    setAcceptDrops(true);

#ifdef Q_OS_WIN
    m_hwnd = (HWND)this->winId();
    ::CoInitialize(NULL);

    HRESULT hr = ::CoCreateInstance(CLSID_TaskbarList, NULL, CLSCTX_INPROC_SERVER, IID_ITaskbarList3, (void**)&m_pTaskbarList3);

    if (hr != S_OK)
    {
        m_pTaskbarList3 = NULL;
    }
#endif
}

MainWindow::~MainWindow()
{
#ifdef Q_OS_WIN
    if (m_pTaskbarList3)
    {
        m_pTaskbarList3->SetProgressState(m_hwnd, TBPF_NOPROGRESS);
        m_pTaskbarList3->Release();
        m_pTaskbarList3 = NULL;
    }
    ::CoUninitialize();
#endif
    delete ui;
}

void MainWindow::slot_browse_output_dir()
{
    QString path = QFileDialog::getExistingDirectory(this, "Browse Output Directory", ".");
    if (!path.isEmpty())
    {
        ui->lineEditOutputDir->setText(QDir::toNativeSeparators(path));
    }
}

void MainWindow::slot_add_audio_files()
{
    if (m_processing)
    {
        warningProcessNotFinished();
        return;
    }

    QStringList paths = QFileDialog::getOpenFileNames(this, "Select Audio Files", ".", "Wave Files (*.wav)");
    for (const QString& path : paths)
    {
        auto *item = new QListWidgetItem();
        item->setText(QFileInfo(path).fileName());
        item->setData(Qt::ItemDataRole::UserRole + 1, path);
        ui->listWidgetTaskList->addItem(item);
    }
}

void MainWindow::slot_clear_audio_list()
{
    if (m_processing)
    {
        warningProcessNotFinished();
        return;
    }

    ui->listWidgetTaskList->clear();
}

void MainWindow::slot_about()
{
    QString arch = QSysInfo::currentCpuArchitecture();
    QString aboutMsg = QString("Audio Slicer %1 (%2)\n%3\n\n%4\n\nQt Version %5")
            .arg(BUILD_VERSION, arch, COPYRIGHT_TEXT, ABOUT_TEXT_OTHER, qVersion());
    QMessageBox::information(this, "About", aboutMsg);
}

void MainWindow::slot_start()
{
    if (m_processing)
    {
        warningProcessNotFinished();
        return;
    }

    int item_count = ui->listWidgetTaskList->count();
    if (item_count == 0)
    {
        return;
    }

    m_workFinished = 0;
    m_workTotal = item_count;

    ui->progressBar->setMinimum(0);
    ui->progressBar->setMaximum(item_count);
    ui->progressBar->setValue(0);

#ifdef Q_OS_WIN
    // Show taskbar progress (Windows 7 and later)
    if (m_pTaskbarList3)
    {
        m_pTaskbarList3->SetProgressState(m_hwnd, TBPF_NORMAL);
        m_pTaskbarList3->SetProgressValue(m_hwnd, (ULONGLONG)0, (ULONGLONG)m_workTotal);
    }
#endif

    setProcessing(true);
    for (int i = 0; i < item_count; i++)
    {
        auto item = ui->listWidgetTaskList->item(i);
        auto path = item->data(Qt::ItemDataRole::UserRole + 1).toString();
        auto runnable = new WorkThread(
                path,
                ui->lineEditOutputDir->text(),
                ui->lineEditThreshold->text().toDouble(),
                ui->lineEditMinLen->text().toULongLong(),
                ui->lineEditMinInterval->text().toULongLong(),
                ui->lineEditHopSize->text().toULongLong(),
                ui->lineEditMaxSilence->text().toULongLong());
        connect(runnable, SIGNAL(oneFinished()),
                this, SLOT(slot_oneFinished()));
        connect(runnable, SIGNAL(oneError(const QString &)),
                this, SLOT(slot_oneError(const QString &)));
        m_threadpool->start(runnable);
    }
}

void MainWindow::slot_oneFinished()
{
    m_workFinished++;
    ui->progressBar->setValue(m_workFinished);
#ifdef Q_OS_WIN
    if (m_pTaskbarList3)
    {
        m_pTaskbarList3->SetProgressState((HWND)this->winId(), TBPF_NORMAL);
        m_pTaskbarList3->SetProgressValue((HWND)this->winId(), (ULONGLONG)m_workFinished, (ULONGLONG)m_workTotal);
    }
#endif

    if (m_workFinished == m_workTotal)
    {
        slot_threadFinished();
    }
}

void MainWindow::slot_oneError(const QString& errmsg)
{
    m_workFinished++;
    ui->progressBar->setValue(m_workFinished);
#ifdef Q_OS_WIN
    if (m_pTaskbarList3)
    {
        m_pTaskbarList3->SetProgressState((HWND)this->winId(), TBPF_NORMAL);
        m_pTaskbarList3->SetProgressValue((HWND)this->winId(), (ULONGLONG)m_workFinished, (ULONGLONG)m_workTotal);
    }
#endif
    // QMessageBox::critical(this, "Error", errmsg);

    if (m_workFinished == m_workTotal)
    {
        slot_threadFinished();
    }
}

void MainWindow::slot_threadFinished()
{
    setProcessing(false);
    m_workFinished = 0;
    m_workTotal = 0;
#ifdef Q_OS_WIN
    if (m_pTaskbarList3)
    {
        m_pTaskbarList3->SetProgressState((HWND)this->winId(), TBPF_NOPROGRESS);
    }
#endif
    QMessageBox::information(
            this, QApplication::applicationName(), "Slicing complete!");
}

void MainWindow::warningProcessNotFinished()
{
    QMessageBox::warning(this, QApplication::applicationName(), "Please wait for slicing to complete!");
}

void MainWindow::setProcessing(bool processing)
{
    bool enabled = !processing;
    ui->pushButtonStart->setText(processing ? "Slicing..." : "Start");
    ui->pushButtonStart->setEnabled(enabled);
    ui->pushButtonAddFiles->setEnabled(enabled);
    ui->listWidgetTaskList->setEnabled(enabled);
    ui->pushButtonClearList->setEnabled(enabled);
    ui->lineEditThreshold->setEnabled(enabled);
    ui->lineEditMinLen->setEnabled(enabled);
    ui->lineEditMinInterval->setEnabled(enabled);
    ui->lineEditHopSize->setEnabled(enabled);
    ui->lineEditMaxSilence->setEnabled(enabled);
    ui->lineEditOutputDir->setEnabled(enabled);
    ui->pushButtonBrowse->setEnabled(enabled);
    m_processing = processing;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (m_processing)
    {
        warningProcessNotFinished();
        event->ignore();
    }
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    auto urls = event->mimeData()->urls();
    bool has_wav = false;
    for (const auto &url: urls)
    {
        if (!url.isLocalFile())
        {
            continue;
        }
        auto path = url.toLocalFile();
        auto ext = QFileInfo(path).suffix();
        if (ext.compare("wav", Qt::CaseInsensitive) == 0)
        {
            has_wav = true;
            break;
        }
    }
    if (has_wav)
    {
        event->accept();
    }
}

void MainWindow::dropEvent(QDropEvent *event)
{
    auto urls = event->mimeData()->urls();
    for (const auto &url: urls)
    {
        if (!url.isLocalFile())
        {
            continue;
        }
        auto path = url.toLocalFile();
        auto ext = QFileInfo(path).suffix();
        if (ext.compare("wav", Qt::CaseInsensitive) != 0)
        {
            continue;
        }

        auto *item = new QListWidgetItem();
        item->setText(QFileInfo(path).fileName());
        item->setData(Qt::ItemDataRole::UserRole + 1, path);
        ui->listWidgetTaskList->addItem(item);
    }
}
