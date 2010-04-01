#include "qtNodeEdit.h"
#include "ui_qtNodeEdit.h"

#include <QFile>
#include <QFileDialog>
#include <QTableWidgetItem>

qtNodeEdit::qtNodeEdit(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::qtNodeEdit)
{
    ui->setupUi(this);
    connect(ui->nodeData, SIGNAL(itemChanged(QTableWidgetItem*)), this, SLOT(dataRowChanged(QTableWidgetItem*)));
    connect(ui->loadImageButton, SIGNAL(clicked()), this, SLOT(loadNodeImage()));
    connect(ui->saveImageButton, SIGNAL(clicked()), this, SLOT(saveNodeImage()));
    connect(ui->textNodeTitle, SIGNAL(textEdited(QString)), this, SLOT(editNodeTitle(QString)));
    connect(ui->imageNodeTitle, SIGNAL(textEdited(QString)), this, SLOT(editNodeTitle(QString)));
    connect(ui->nodeDataArea, SIGNAL(currentChanged(int)), this, SLOT(tabActivated(int)));

    // disable aux tabs
    ui->nodeDataArea->setTabEnabled(1, false);
    ui->nodeDataArea->setTabEnabled(2, false);
}

qtNodeEdit::~qtNodeEdit()
{
    delete ui;
}

void qtNodeEdit::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void qtNodeEdit::setNode(qtVaultNode *pn) {
    textEdited = false;
    titleEdited = false;
    node = pn;
    update();
}

void qtNodeEdit::dataRowChanged(QTableWidgetItem* item) {
    qWarning("Node data changed");
    int field = item->row();
    node->setFieldFromString(field, plString(item->text().toUtf8().data()));
    update();
}

void qtNodeEdit::editNodeTitle(QString title) {
    newTitle = title;
    titleEdited = true;
}

void qtNodeEdit::editNodeText() {
    textEdited = true;
}

void qtNodeEdit::tabActivated(int tab) {
    if(titleEdited)
        node->setString64(0, plString(newTitle.toUtf8().data()));
    if(textEdited)
        node->setText(0, plString(ui->textNodeEdit->document()->toPlainText().toUtf8().data()));
    if(titleEdited || textEdited)
        update();
    titleEdited = false;
    textEdited = false;
}

void qtNodeEdit::saveNodeImage() {
    node->lockNode();
    QString fileName = QFileDialog::getSaveFileName(this, "Save Image", (node->getString64(0) + plString(".jpg")).cstr(), "*.jpg *.jpeg");
    QFile outFile(fileName);
    outFile.open(QIODevice::WriteOnly);
    outFile.write((const char*)node->getBlob(0).getData() + 4, node->getBlob(0).getSize() - 4);
    outFile.close();
    node->unlockNode();
}

void qtNodeEdit::loadNodeImage() {
    QString fileName = QFileDialog::getOpenFileName(this, "Load Image", "./", "*.jpg *.jpeg");
    if(fileName.isEmpty()) {
        return;
    }
    node->lockNode();
    QFile inFile(fileName);
    inFile.open(QIODevice::ReadOnly);
    QByteArray data = inFile.readAll();
    inFile.close();
    hsUint32 len = data.length();
    char* dataPtr = new char[len + 4];
    *(hsUint32*)dataPtr = len;
    memcpy(dataPtr + 4, data.data(), len);
    plVaultBlob blob;
    blob.setData(len + 4, (const unsigned char*)dataPtr);
    node->setBlob(0, blob);
    node->unlockNode();
    delete[] dataPtr;
    update();
}

void qtNodeEdit::update() {
    disconnect(ui->nodeData, SIGNAL(itemChanged(QTableWidgetItem*)), this, SLOT(dataRowChanged(QTableWidgetItem*)));
    emit isDirty(false);
    ui->nodeData->clearContents();
    if(node != NULL) {
        node->lockNode();
        ui->nodeData->setEnabled(true);
        for(int i = 0; i < qtVaultNode::kNumFields; i++) {
            QTableWidgetItem* fitem = new QTableWidgetItem();
            fitem->setText(QString(node->fieldName(i).cstr()));
            fitem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
            QTableWidgetItem* citem = new QTableWidgetItem();
            citem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable);
            citem->setText(QString(node->getFieldAsString(i).cstr()));
            if(node->hasDirty(i)) {
                QColor rowColor(200, 127, 127);
                fitem->setBackground(QBrush(rowColor));
                citem->setBackground(QBrush(rowColor));
                emit isDirty(true);
            }
            ui->nodeData->setItem(i, 0, fitem);
            ui->nodeData->setItem(i, 1, citem);
        }
        // set the type-specific content
        switch(node->getNodeType()) {
            case plVault::kNodeImage:
                {
                    if(node->getBlob(0).getSize()) {
                        QImage image = QImage::fromData(node->getBlob(0).getData() + 4, (int)node->getBlob(0).getSize() - 4, "JPEG");
                        qWarning("Image Size: %u, %u", image.width(), image.height());
                        ui->imageLabel->setPixmap(QPixmap::fromImage(image));
                    }else{
                        qWarning("No Image in Image Node");
                        ui->imageLabel->setPixmap(QPixmap());
                    }
                    ui->imageLabel->setMaximumSize(320, 240);
                    ui->imageNodeTitle->setText(QString(node->getString64(0).cstr()));
                    ui->nodeDataArea->setTabEnabled(1, true);
                    ui->nodeDataArea->setTabEnabled(2, false);
                }
                break;
            case plVault::kNodeTextNote:
                disconnect(ui->textNodeEdit, SIGNAL(textChanged()), this, SLOT(editNodeText()));
                ui->textNodeEdit->setPlainText(QString(node->getText(0)));
                ui->textNodeTitle->setText(QString(node->getString64(0)));
                ui->nodeDataArea->setTabEnabled(1, false);
                ui->nodeDataArea->setTabEnabled(2, true);
                connect(ui->textNodeEdit, SIGNAL(textChanged()), this, SLOT(editNodeText()));
                break;
            default:
                ui->nodeDataArea->setTabEnabled(1, false);
                ui->nodeDataArea->setTabEnabled(2, false);
                break;
        }
        node->unlockNode();
    }else{
        ui->nodeData->setEnabled(false);
    }
    connect(ui->nodeData, SIGNAL(itemChanged(QTableWidgetItem*)), this, SLOT(dataRowChanged(QTableWidgetItem*)));
}

qtVaultNode* qtNodeEdit::getNode() {
    return node;
}
