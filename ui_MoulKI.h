/********************************************************************************
** Form generated from reading UI file 'MoulKI.ui'
**
** Created: Wed Mar 24 14:40:12 2010
**      by: Qt User Interface Compiler version 4.6.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MOULKI_H
#define UI_MOULKI_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QMainWindow>
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QPlainTextEdit>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QStatusBar>
#include <QtGui/QTabWidget>
#include <QtGui/QTableWidget>
#include <QtGui/QTextEdit>
#include <QtGui/QTreeWidget>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MoulKIClass
{
public:
    QAction *actionAdd;
    QAction *actionCreate_Node;
    QAction *actionSubscribe;
    QAction *actionSave_Vault;
    QAction *actionLogin;
    QAction *actionLogout;
    QAction *actionSet_Active;
    QAction *actionJoin_Age;
    QAction *actionLeave_Age;
    QAction *actionLoad_Vault;
    QAction *actionFind_Node;
    QWidget *centralWidget;
    QVBoxLayout *verticalLayout;
    QTabWidget *tabWidget;
    QWidget *tab;
    QHBoxLayout *horizontalLayout;
    QTreeWidget *vaultTree;
    QVBoxLayout *verticalLayout_5;
    QTabWidget *nodeDataArea;
    QWidget *rawTab;
    QVBoxLayout *verticalLayout_3;
    QTableWidget *nodeData;
    QWidget *imageTab;
    QVBoxLayout *verticalLayout_6;
    QHBoxLayout *horizontalLayout_4;
    QLabel *label_2;
    QLineEdit *imageNodeTitle;
    QLabel *imageLabel;
    QSpacerItem *verticalSpacer;
    QHBoxLayout *horizontalLayout_3;
    QPushButton *saveImageButton;
    QPushButton *loadImageButton;
    QWidget *textTab;
    QVBoxLayout *verticalLayout_4;
    QHBoxLayout *horizontalLayout_5;
    QLabel *label_3;
    QLineEdit *textNodeTitle;
    QPlainTextEdit *textNodeEdit;
    QPushButton *applyButton;
    QWidget *tab_2;
    QVBoxLayout *verticalLayout_2;
    QHBoxLayout *horizontalLayout_2;
    QTextEdit *chatPane;
    QTreeWidget *playersTree;
    QLineEdit *chatEntry;
    QStatusBar *statusBar;
    QMenuBar *menuBar;
    QMenu *menuAuth;
    QMenu *menuVault;
    QMenu *menuGame;

    void setupUi(QMainWindow *MoulKIClass)
    {
        if (MoulKIClass->objectName().isEmpty())
            MoulKIClass->setObjectName(QString::fromUtf8("MoulKIClass"));
        MoulKIClass->resize(600, 400);
        actionAdd = new QAction(MoulKIClass);
        actionAdd->setObjectName(QString::fromUtf8("actionAdd"));
        actionCreate_Node = new QAction(MoulKIClass);
        actionCreate_Node->setObjectName(QString::fromUtf8("actionCreate_Node"));
        actionSubscribe = new QAction(MoulKIClass);
        actionSubscribe->setObjectName(QString::fromUtf8("actionSubscribe"));
        actionSave_Vault = new QAction(MoulKIClass);
        actionSave_Vault->setObjectName(QString::fromUtf8("actionSave_Vault"));
        actionLogin = new QAction(MoulKIClass);
        actionLogin->setObjectName(QString::fromUtf8("actionLogin"));
        actionLogout = new QAction(MoulKIClass);
        actionLogout->setObjectName(QString::fromUtf8("actionLogout"));
        actionSet_Active = new QAction(MoulKIClass);
        actionSet_Active->setObjectName(QString::fromUtf8("actionSet_Active"));
        actionJoin_Age = new QAction(MoulKIClass);
        actionJoin_Age->setObjectName(QString::fromUtf8("actionJoin_Age"));
        actionLeave_Age = new QAction(MoulKIClass);
        actionLeave_Age->setObjectName(QString::fromUtf8("actionLeave_Age"));
        actionLoad_Vault = new QAction(MoulKIClass);
        actionLoad_Vault->setObjectName(QString::fromUtf8("actionLoad_Vault"));
        actionFind_Node = new QAction(MoulKIClass);
        actionFind_Node->setObjectName(QString::fromUtf8("actionFind_Node"));
        centralWidget = new QWidget(MoulKIClass);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        verticalLayout = new QVBoxLayout(centralWidget);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        tabWidget = new QTabWidget(centralWidget);
        tabWidget->setObjectName(QString::fromUtf8("tabWidget"));
        tab = new QWidget();
        tab->setObjectName(QString::fromUtf8("tab"));
        horizontalLayout = new QHBoxLayout(tab);
        horizontalLayout->setSpacing(6);
        horizontalLayout->setContentsMargins(11, 11, 11, 11);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        vaultTree = new QTreeWidget(tab);
        QTreeWidgetItem *__qtreewidgetitem = new QTreeWidgetItem();
        __qtreewidgetitem->setText(0, QString::fromUtf8("1"));
        vaultTree->setHeaderItem(__qtreewidgetitem);
        vaultTree->setObjectName(QString::fromUtf8("vaultTree"));
        vaultTree->setRootIsDecorated(true);
        vaultTree->header()->setVisible(false);

        horizontalLayout->addWidget(vaultTree);

        verticalLayout_5 = new QVBoxLayout();
        verticalLayout_5->setSpacing(6);
        verticalLayout_5->setObjectName(QString::fromUtf8("verticalLayout_5"));
        nodeDataArea = new QTabWidget(tab);
        nodeDataArea->setObjectName(QString::fromUtf8("nodeDataArea"));
        rawTab = new QWidget();
        rawTab->setObjectName(QString::fromUtf8("rawTab"));
        verticalLayout_3 = new QVBoxLayout(rawTab);
        verticalLayout_3->setSpacing(6);
        verticalLayout_3->setContentsMargins(11, 11, 11, 11);
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        nodeData = new QTableWidget(rawTab);
        if (nodeData->columnCount() < 2)
            nodeData->setColumnCount(2);
        QTableWidgetItem *__qtablewidgetitem = new QTableWidgetItem();
        nodeData->setHorizontalHeaderItem(0, __qtablewidgetitem);
        QTableWidgetItem *__qtablewidgetitem1 = new QTableWidgetItem();
        nodeData->setHorizontalHeaderItem(1, __qtablewidgetitem1);
        if (nodeData->rowCount() < 32)
            nodeData->setRowCount(32);
        nodeData->setObjectName(QString::fromUtf8("nodeData"));
        nodeData->setEnabled(false);
        nodeData->setAlternatingRowColors(true);
        nodeData->setSelectionBehavior(QAbstractItemView::SelectRows);
        nodeData->setRowCount(32);
        nodeData->setColumnCount(2);
        nodeData->horizontalHeader()->setStretchLastSection(true);
        nodeData->verticalHeader()->setVisible(false);

        verticalLayout_3->addWidget(nodeData);

        nodeDataArea->addTab(rawTab, QString());
        imageTab = new QWidget();
        imageTab->setObjectName(QString::fromUtf8("imageTab"));
        verticalLayout_6 = new QVBoxLayout(imageTab);
        verticalLayout_6->setSpacing(6);
        verticalLayout_6->setContentsMargins(11, 11, 11, 11);
        verticalLayout_6->setObjectName(QString::fromUtf8("verticalLayout_6"));
        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setSpacing(6);
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        label_2 = new QLabel(imageTab);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        horizontalLayout_4->addWidget(label_2);

        imageNodeTitle = new QLineEdit(imageTab);
        imageNodeTitle->setObjectName(QString::fromUtf8("imageNodeTitle"));

        horizontalLayout_4->addWidget(imageNodeTitle);


        verticalLayout_6->addLayout(horizontalLayout_4);

        imageLabel = new QLabel(imageTab);
        imageLabel->setObjectName(QString::fromUtf8("imageLabel"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(imageLabel->sizePolicy().hasHeightForWidth());
        imageLabel->setSizePolicy(sizePolicy);
        imageLabel->setScaledContents(true);

        verticalLayout_6->addWidget(imageLabel);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_6->addItem(verticalSpacer);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setSpacing(6);
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        saveImageButton = new QPushButton(imageTab);
        saveImageButton->setObjectName(QString::fromUtf8("saveImageButton"));

        horizontalLayout_3->addWidget(saveImageButton);

        loadImageButton = new QPushButton(imageTab);
        loadImageButton->setObjectName(QString::fromUtf8("loadImageButton"));

        horizontalLayout_3->addWidget(loadImageButton);


        verticalLayout_6->addLayout(horizontalLayout_3);

        nodeDataArea->addTab(imageTab, QString());
        textTab = new QWidget();
        textTab->setObjectName(QString::fromUtf8("textTab"));
        verticalLayout_4 = new QVBoxLayout(textTab);
        verticalLayout_4->setSpacing(6);
        verticalLayout_4->setContentsMargins(11, 11, 11, 11);
        verticalLayout_4->setObjectName(QString::fromUtf8("verticalLayout_4"));
        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setSpacing(6);
        horizontalLayout_5->setObjectName(QString::fromUtf8("horizontalLayout_5"));
        label_3 = new QLabel(textTab);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        horizontalLayout_5->addWidget(label_3);

        textNodeTitle = new QLineEdit(textTab);
        textNodeTitle->setObjectName(QString::fromUtf8("textNodeTitle"));

        horizontalLayout_5->addWidget(textNodeTitle);


        verticalLayout_4->addLayout(horizontalLayout_5);

        textNodeEdit = new QPlainTextEdit(textTab);
        textNodeEdit->setObjectName(QString::fromUtf8("textNodeEdit"));

        verticalLayout_4->addWidget(textNodeEdit);

        nodeDataArea->addTab(textTab, QString());

        verticalLayout_5->addWidget(nodeDataArea);

        applyButton = new QPushButton(tab);
        applyButton->setObjectName(QString::fromUtf8("applyButton"));
        applyButton->setEnabled(false);

        verticalLayout_5->addWidget(applyButton);


        horizontalLayout->addLayout(verticalLayout_5);

        tabWidget->addTab(tab, QString());
        tab_2 = new QWidget();
        tab_2->setObjectName(QString::fromUtf8("tab_2"));
        verticalLayout_2 = new QVBoxLayout(tab_2);
        verticalLayout_2->setSpacing(6);
        verticalLayout_2->setContentsMargins(11, 11, 11, 11);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        chatPane = new QTextEdit(tab_2);
        chatPane->setObjectName(QString::fromUtf8("chatPane"));

        horizontalLayout_2->addWidget(chatPane);

        playersTree = new QTreeWidget(tab_2);
        QTreeWidgetItem *__qtreewidgetitem1 = new QTreeWidgetItem();
        __qtreewidgetitem1->setText(0, QString::fromUtf8("1"));
        playersTree->setHeaderItem(__qtreewidgetitem1);
        playersTree->setObjectName(QString::fromUtf8("playersTree"));
        playersTree->setMaximumSize(QSize(150, 16777215));
        playersTree->header()->setVisible(false);

        horizontalLayout_2->addWidget(playersTree);


        verticalLayout_2->addLayout(horizontalLayout_2);

        chatEntry = new QLineEdit(tab_2);
        chatEntry->setObjectName(QString::fromUtf8("chatEntry"));

        verticalLayout_2->addWidget(chatEntry);

        tabWidget->addTab(tab_2, QString());

        verticalLayout->addWidget(tabWidget);

        MoulKIClass->setCentralWidget(centralWidget);
        statusBar = new QStatusBar(MoulKIClass);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        MoulKIClass->setStatusBar(statusBar);
        menuBar = new QMenuBar(MoulKIClass);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 600, 25));
        menuAuth = new QMenu(menuBar);
        menuAuth->setObjectName(QString::fromUtf8("menuAuth"));
        menuVault = new QMenu(menuBar);
        menuVault->setObjectName(QString::fromUtf8("menuVault"));
        menuGame = new QMenu(menuBar);
        menuGame->setObjectName(QString::fromUtf8("menuGame"));
        MoulKIClass->setMenuBar(menuBar);

        menuBar->addAction(menuAuth->menuAction());
        menuBar->addAction(menuVault->menuAction());
        menuBar->addAction(menuGame->menuAction());
        menuAuth->addAction(actionLogin);
        menuAuth->addAction(actionLogout);
        menuVault->addAction(actionSubscribe);
        menuVault->addAction(actionSet_Active);
        menuVault->addAction(actionFind_Node);
        menuVault->addSeparator();
        menuVault->addAction(actionSave_Vault);
        menuVault->addAction(actionLoad_Vault);
        menuGame->addAction(actionJoin_Age);
        menuGame->addAction(actionLeave_Age);

        retranslateUi(MoulKIClass);

        tabWidget->setCurrentIndex(0);
        nodeDataArea->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(MoulKIClass);
    } // setupUi

    void retranslateUi(QMainWindow *MoulKIClass)
    {
        MoulKIClass->setWindowTitle(QApplication::translate("MoulKIClass", "MoulKI", 0, QApplication::UnicodeUTF8));
        actionAdd->setText(QApplication::translate("MoulKIClass", "Add", 0, QApplication::UnicodeUTF8));
        actionCreate_Node->setText(QApplication::translate("MoulKIClass", "Create Node", 0, QApplication::UnicodeUTF8));
        actionSubscribe->setText(QApplication::translate("MoulKIClass", "Fetch Tree", 0, QApplication::UnicodeUTF8));
        actionSave_Vault->setText(QApplication::translate("MoulKIClass", "Save Vault", 0, QApplication::UnicodeUTF8));
        actionLogin->setText(QApplication::translate("MoulKIClass", "Login", 0, QApplication::UnicodeUTF8));
        actionLogout->setText(QApplication::translate("MoulKIClass", "Logout", 0, QApplication::UnicodeUTF8));
        actionSet_Active->setText(QApplication::translate("MoulKIClass", "Set Active", 0, QApplication::UnicodeUTF8));
        actionJoin_Age->setText(QApplication::translate("MoulKIClass", "Join Age", 0, QApplication::UnicodeUTF8));
        actionLeave_Age->setText(QApplication::translate("MoulKIClass", "Leave Age", 0, QApplication::UnicodeUTF8));
        actionLoad_Vault->setText(QApplication::translate("MoulKIClass", "Load Vault", 0, QApplication::UnicodeUTF8));
        actionFind_Node->setText(QApplication::translate("MoulKIClass", "Find Node", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem = nodeData->horizontalHeaderItem(0);
        ___qtablewidgetitem->setText(QApplication::translate("MoulKIClass", "Field", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem1 = nodeData->horizontalHeaderItem(1);
        ___qtablewidgetitem1->setText(QApplication::translate("MoulKIClass", "Value", 0, QApplication::UnicodeUTF8));
        nodeDataArea->setTabText(nodeDataArea->indexOf(rawTab), QApplication::translate("MoulKIClass", "Node", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("MoulKIClass", "Title:", 0, QApplication::UnicodeUTF8));
        imageLabel->setText(QString());
        saveImageButton->setText(QApplication::translate("MoulKIClass", "Save Image", 0, QApplication::UnicodeUTF8));
        loadImageButton->setText(QApplication::translate("MoulKIClass", "Load Image", 0, QApplication::UnicodeUTF8));
        nodeDataArea->setTabText(nodeDataArea->indexOf(imageTab), QApplication::translate("MoulKIClass", "Image", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("MoulKIClass", "Title:", 0, QApplication::UnicodeUTF8));
        nodeDataArea->setTabText(nodeDataArea->indexOf(textTab), QApplication::translate("MoulKIClass", "Text", 0, QApplication::UnicodeUTF8));
        applyButton->setText(QApplication::translate("MoulKIClass", "Apply", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(tab), QApplication::translate("MoulKIClass", "Vault", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(tab_2), QApplication::translate("MoulKIClass", "Game", 0, QApplication::UnicodeUTF8));
        menuAuth->setTitle(QApplication::translate("MoulKIClass", "Auth", 0, QApplication::UnicodeUTF8));
        menuVault->setTitle(QApplication::translate("MoulKIClass", "Vault", 0, QApplication::UnicodeUTF8));
        menuGame->setTitle(QApplication::translate("MoulKIClass", "Game", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class MoulKIClass: public Ui_MoulKIClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MOULKI_H
