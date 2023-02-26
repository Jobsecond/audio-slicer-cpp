/********************************************************************************
** Form generated from reading UI file 'ui_mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.15.7
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout_2;
    QPushButton *pushButtonAddFiles;
    QSpacerItem *horizontalSpacer;
    QHBoxLayout *horizontalLayout;
    QGroupBox *groupBox;
    QVBoxLayout *verticalLayout_2;
    QListWidget *listWidgetTaskList;
    QPushButton *pushButtonClearList;
    QGroupBox *groupBox_2;
    QVBoxLayout *verticalLayout_3;
    QFormLayout *formLayout;
    QLabel *label_2;
    QLineEdit *lineEditThreshold;
    QLabel *label_3;
    QLineEdit *lineEditMinLen;
    QLabel *label_4;
    QLineEdit *lineEditMinInterval;
    QLabel *label_5;
    QLineEdit *lineEditHopSize;
    QLabel *label_6;
    QLineEdit *lineEditMaxSilence;
    QLabel *label_7;
    QHBoxLayout *horizontalLayout_4;
    QLineEdit *lineEditOutputDir;
    QPushButton *pushButtonBrowse;
    QSpacerItem *verticalSpacer;
    QHBoxLayout *horizontalLayout_3;
    QPushButton *pushButtonAbout;
    QProgressBar *progressBar;
    QPushButton *pushButtonStart;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(768, 480);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        verticalLayout = new QVBoxLayout(centralwidget);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        pushButtonAddFiles = new QPushButton(centralwidget);
        pushButtonAddFiles->setObjectName(QString::fromUtf8("pushButtonAddFiles"));
        pushButtonAddFiles->setEnabled(true);
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(pushButtonAddFiles->sizePolicy().hasHeightForWidth());
        pushButtonAddFiles->setSizePolicy(sizePolicy);

        horizontalLayout_2->addWidget(pushButtonAddFiles);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer);


        verticalLayout->addLayout(horizontalLayout_2);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        groupBox = new QGroupBox(centralwidget);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        QSizePolicy sizePolicy1(QSizePolicy::Preferred, QSizePolicy::Minimum);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(groupBox->sizePolicy().hasHeightForWidth());
        groupBox->setSizePolicy(sizePolicy1);
        verticalLayout_2 = new QVBoxLayout(groupBox);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        listWidgetTaskList = new QListWidget(groupBox);
        listWidgetTaskList->setObjectName(QString::fromUtf8("listWidgetTaskList"));
        listWidgetTaskList->setFrameShadow(QFrame::Plain);

        verticalLayout_2->addWidget(listWidgetTaskList);

        pushButtonClearList = new QPushButton(groupBox);
        pushButtonClearList->setObjectName(QString::fromUtf8("pushButtonClearList"));

        verticalLayout_2->addWidget(pushButtonClearList);


        horizontalLayout->addWidget(groupBox);

        groupBox_2 = new QGroupBox(centralwidget);
        groupBox_2->setObjectName(QString::fromUtf8("groupBox_2"));
        sizePolicy1.setHeightForWidth(groupBox_2->sizePolicy().hasHeightForWidth());
        groupBox_2->setSizePolicy(sizePolicy1);
        verticalLayout_3 = new QVBoxLayout(groupBox_2);
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        formLayout = new QFormLayout();
        formLayout->setObjectName(QString::fromUtf8("formLayout"));
        label_2 = new QLabel(groupBox_2);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        formLayout->setWidget(0, QFormLayout::LabelRole, label_2);

        lineEditThreshold = new QLineEdit(groupBox_2);
        lineEditThreshold->setObjectName(QString::fromUtf8("lineEditThreshold"));
        lineEditThreshold->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        formLayout->setWidget(0, QFormLayout::FieldRole, lineEditThreshold);

        label_3 = new QLabel(groupBox_2);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        formLayout->setWidget(1, QFormLayout::LabelRole, label_3);

        lineEditMinLen = new QLineEdit(groupBox_2);
        lineEditMinLen->setObjectName(QString::fromUtf8("lineEditMinLen"));
        lineEditMinLen->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        formLayout->setWidget(1, QFormLayout::FieldRole, lineEditMinLen);

        label_4 = new QLabel(groupBox_2);
        label_4->setObjectName(QString::fromUtf8("label_4"));

        formLayout->setWidget(2, QFormLayout::LabelRole, label_4);

        lineEditMinInterval = new QLineEdit(groupBox_2);
        lineEditMinInterval->setObjectName(QString::fromUtf8("lineEditMinInterval"));
        lineEditMinInterval->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        formLayout->setWidget(2, QFormLayout::FieldRole, lineEditMinInterval);

        label_5 = new QLabel(groupBox_2);
        label_5->setObjectName(QString::fromUtf8("label_5"));

        formLayout->setWidget(3, QFormLayout::LabelRole, label_5);

        lineEditHopSize = new QLineEdit(groupBox_2);
        lineEditHopSize->setObjectName(QString::fromUtf8("lineEditHopSize"));
        lineEditHopSize->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        formLayout->setWidget(3, QFormLayout::FieldRole, lineEditHopSize);

        label_6 = new QLabel(groupBox_2);
        label_6->setObjectName(QString::fromUtf8("label_6"));

        formLayout->setWidget(4, QFormLayout::LabelRole, label_6);

        lineEditMaxSilence = new QLineEdit(groupBox_2);
        lineEditMaxSilence->setObjectName(QString::fromUtf8("lineEditMaxSilence"));
        lineEditMaxSilence->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        formLayout->setWidget(4, QFormLayout::FieldRole, lineEditMaxSilence);


        verticalLayout_3->addLayout(formLayout);

        label_7 = new QLabel(groupBox_2);
        label_7->setObjectName(QString::fromUtf8("label_7"));

        verticalLayout_3->addWidget(label_7);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        lineEditOutputDir = new QLineEdit(groupBox_2);
        lineEditOutputDir->setObjectName(QString::fromUtf8("lineEditOutputDir"));
        lineEditOutputDir->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);

        horizontalLayout_4->addWidget(lineEditOutputDir);

        pushButtonBrowse = new QPushButton(groupBox_2);
        pushButtonBrowse->setObjectName(QString::fromUtf8("pushButtonBrowse"));

        horizontalLayout_4->addWidget(pushButtonBrowse);


        verticalLayout_3->addLayout(horizontalLayout_4);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_3->addItem(verticalSpacer);


        horizontalLayout->addWidget(groupBox_2);


        verticalLayout->addLayout(horizontalLayout);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        pushButtonAbout = new QPushButton(centralwidget);
        pushButtonAbout->setObjectName(QString::fromUtf8("pushButtonAbout"));

        horizontalLayout_3->addWidget(pushButtonAbout);

        progressBar = new QProgressBar(centralwidget);
        progressBar->setObjectName(QString::fromUtf8("progressBar"));
        progressBar->setValue(0);

        horizontalLayout_3->addWidget(progressBar);

        pushButtonStart = new QPushButton(centralwidget);
        pushButtonStart->setObjectName(QString::fromUtf8("pushButtonStart"));

        horizontalLayout_3->addWidget(pushButtonStart);


        verticalLayout->addLayout(horizontalLayout_3);

        MainWindow->setCentralWidget(centralwidget);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        pushButtonAddFiles->setText(QCoreApplication::translate("MainWindow", "Add Audio Files...", nullptr));
        groupBox->setTitle(QCoreApplication::translate("MainWindow", "Task List", nullptr));
        pushButtonClearList->setText(QCoreApplication::translate("MainWindow", "Clear List", nullptr));
        groupBox_2->setTitle(QCoreApplication::translate("MainWindow", "Settings", nullptr));
        label_2->setText(QCoreApplication::translate("MainWindow", "Threshold (dB)", nullptr));
        lineEditThreshold->setText(QCoreApplication::translate("MainWindow", "-40", nullptr));
        label_3->setText(QCoreApplication::translate("MainWindow", "Minimum Length (ms)", nullptr));
        lineEditMinLen->setText(QCoreApplication::translate("MainWindow", "5000", nullptr));
        label_4->setText(QCoreApplication::translate("MainWindow", "Minimum Interval (ms)", nullptr));
        lineEditMinInterval->setText(QCoreApplication::translate("MainWindow", "300", nullptr));
        label_5->setText(QCoreApplication::translate("MainWindow", "Hop Size (ms)", nullptr));
        lineEditHopSize->setText(QCoreApplication::translate("MainWindow", "10", nullptr));
        label_6->setText(QCoreApplication::translate("MainWindow", "Maximum Silence Length (ms)", nullptr));
        lineEditMaxSilence->setText(QCoreApplication::translate("MainWindow", "1000", nullptr));
        label_7->setText(QCoreApplication::translate("MainWindow", "Output Directory (default to the same as the audio)", nullptr));
        lineEditOutputDir->setText(QString());
        pushButtonBrowse->setText(QCoreApplication::translate("MainWindow", "Browse...", nullptr));
        pushButtonAbout->setText(QCoreApplication::translate("MainWindow", "About", nullptr));
        pushButtonStart->setText(QCoreApplication::translate("MainWindow", "Start", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
