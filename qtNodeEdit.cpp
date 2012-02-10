#include "qtNodeEdit.h"
#include "ui_qtNodeEdit.h"

#include <QFile>
#include <QFileDialog>
#include <QTableWidgetItem>
#include <SDL/plSDLMgr.h>
#include <SDL/plStateDataRecord.h>
#include <Stream/hsRAMStream.h>

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
    ui->nodeDataArea->setTabEnabled(3, false);
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

void qtNodeEdit::setMgrs(plSDLMgr* sdl, plResManager* res) {
    sdlmgr = sdl;
    resmgr = res;
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
    plString extension, filter;
    int offset = 0;

    node->lockNode();
    switch(node->getInt32(0)) {
        case plVault::ImageTypes::kJPEG:
            extension = ".jpg";
            filter = "*.jpg *.jpeg";
            offset = 4;
            break;
        case plVault::ImageTypes::kPNG:
            extension = ".png";
            filter = "*.png";
            break;
    }
    QString fileName = QFileDialog::getSaveFileName(this, "Save Image", (node->getString64(0) + extension).cstr(), filter.cstr());
    QFile outFile(fileName);
    outFile.open(QIODevice::WriteOnly);
    outFile.write((const char*)node->getBlob(0).getData() + offset, node->getBlob(0).getSize() - offset);
    outFile.close();
    node->unlockNode();
}

void qtNodeEdit::loadNodeImage() {
    QString fileName = QFileDialog::getOpenFileName(this, "Load Image", "./", "*.jpg *.jpeg *.png");
    if(fileName.isEmpty()) {
        return;
    }
    int imageType = plVault::ImageTypes::kNone;
    int offset = 0;
    if(fileName.endsWith(".jpg") || fileName.endsWith(".jpeg")) {
        imageType = plVault::ImageTypes::kJPEG;
        offset = 4;
    } else if (fileName.endsWith(".png"))
        imageType = plVault::ImageTypes::kPNG;
    else
        return;
    node->lockNode();
    QFile inFile(fileName);
    inFile.open(QIODevice::ReadOnly);
    QByteArray data = inFile.readAll();
    inFile.close();
    uint32_t len = data.length();
    char* dataPtr = new char[len + offset];
    if(imageType == plVault::ImageTypes::kJPEG)
        *(uint32_t*)dataPtr = len;
    memcpy(dataPtr + offset, data.data(), len);
    plVaultBlob blob;
    blob.setData(len + offset, (const unsigned char*)dataPtr);
    node->setBlob(0, blob);
    node->setInt32(0, imageType);
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
                        int offset = 0;

                        if(node->getInt32(0) == plVault::ImageTypes::kJPEG) // JPEG Streams have an additional size at the start
                            offset = 4;

                        QImage image = QImage::fromData(node->getBlob(0).getData() + offset, (int)node->getBlob(0).getSize() - offset);
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
                    ui->nodeDataArea->setTabEnabled(3, false);
                }
                break;
            case plVault::kNodeChronicle:
            case plVault::kNodeTextNote:
                disconnect(ui->textNodeEdit, SIGNAL(textChanged()), this, SLOT(editNodeText()));
                ui->textNodeEdit->setPlainText(QString(node->getText(0)));
                ui->textNodeTitle->setText(QString(node->getString64(0)));
                ui->nodeDataArea->setTabEnabled(1, false);
                ui->nodeDataArea->setTabEnabled(2, true);
                ui->nodeDataArea->setTabEnabled(3, false);
                connect(ui->textNodeEdit, SIGNAL(textChanged()), this, SLOT(editNodeText()));
                break;
            case plVault::kNodeSDL:
                {
                    hsRAMStream S(PlasmaVer::pvMoul);
                    plVaultBlob blob = node->getBlob(0);
                    S.copyFrom(blob.getData(), blob.getSize());
                    plString name;
                    int version;
                    plStateDataRecord record;
                    record.ReadStreamHeader(&S, name, version, NULL);
                    record.setDescriptor(sdlmgr->GetDescriptor(name, version));
                    record.read(&S, resmgr);
                    for(size_t var = 0; var < record.getNumVars(); var++) {
                        plStateVariable* stateVar = record.get(var);
                        plVarDescriptor* varDesc = stateVar->getDescriptor();
                        QTableWidgetItem* fitem = new QTableWidgetItem();
                        fitem->setText(QString(varDesc->getName().cstr()));
                        fitem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
                        QTableWidgetItem* citem = new QTableWidgetItem();
                        citem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable);
                        if(varDesc->getType() != plVarDescriptor::kStateDescriptor) {
                            plSimpleStateVariable* simpleVar = (plSimpleStateVariable*)stateVar;
                            for(int i = 0; i < simpleVar->getCount(); i++) {
                                switch(varDesc->getType()) {
                                    case plVarDescriptor::kAgeTimeElapsed:
                                    case plVarDescriptor::kAgeTimeOfDay:
                                        if(&simpleVar->Time(i) != NULL)
                                            citem->setText(simpleVar->Time(i).format("MM/dd/yyyy hh:mm:ss").cstr());
                                        else
                                            citem->setText("null time");
                                        break;
                                    case plVarDescriptor::kBool:
                                        if(simpleVar->Bool(i))
                                            citem->setText("True");
                                        else
                                            citem->setText("False");
                                        break;
                                    case plVarDescriptor::kInt:
                                            citem->setText(plString::Format("%d", simpleVar->Int(i)).cstr());
                                        break;
                                    default:
                                        citem->setText("");
                                        break;
                                }
                            }
                        }
                        if(stateVar->isDirty() && !stateVar->isDefault()) {
                            QColor rowColor(200, 127, 127);
                            fitem->setBackground(QBrush(rowColor));
                            citem->setBackground(QBrush(rowColor));
                        }
                        ui->nodeSDLData->setItem(var, 0, fitem);
                        ui->nodeSDLData->setItem(var, 1, citem);
                    }
                    ui->nodeDataArea->setTabEnabled(1, false);
                    ui->nodeDataArea->setTabEnabled(2, false);
                    ui->nodeDataArea->setTabEnabled(3, true);
                    break;
                }
            default:
                ui->nodeDataArea->setTabEnabled(1, false);
                ui->nodeDataArea->setTabEnabled(2, false);
                ui->nodeDataArea->setTabEnabled(3, false);
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
