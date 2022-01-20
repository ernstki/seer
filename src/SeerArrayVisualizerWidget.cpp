#include "SeerArrayVisualizerWidget.h"
#include "SeerUtl.h"
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QFileDialog>
#include <QtGui/QIntValidator>
#include <QtGui/QIcon>
#include <QtPrintSupport/QPrinter>
#include <QtPrintSupport/QPrintDialog>
#include <QtCore/QRegExp>
#include <QtCore/QSettings>
#include <QtCore/QDebug>

SeerArrayVisualizerWidget::SeerArrayVisualizerWidget (QWidget* parent) : QWidget(parent) {

    // Init variables.
    _variableId = Seer::createID(); // Create two id's for queries.
    _memoryId   = Seer::createID();

    // Set up UI.
    setupUi(this);

    // Setup the widgets
    setWindowIcon(QIcon(":/seer/resources/seer_64x64.png"));
    setWindowTitle("Seer Array Visualizer");

    arrayLengthLineEdit->setValidator(new QIntValidator(1, 9999999, this));
    columnCountSpinBox->setValue(memoryHexEditor->bytesPerLine());

    arrayDisplayFormatComboBox->setCurrentIndex(0);

    // Connect things.
    QObject::connect(refreshToolButton,             &QToolButton::clicked,                                     this,  &SeerArrayVisualizerWidget::handleRefreshButton);
    QObject::connect(arrayLengthLineEdit,           &QLineEdit::returnPressed,                                 this,  &SeerArrayVisualizerWidget::handleRefreshButton);
    QObject::connect(variableNameLineEdit,          &QLineEdit::returnPressed,                                 this,  &SeerArrayVisualizerWidget::handleVariableNameLineEdit);
    QObject::connect(arrayDisplayFormatComboBox,    QOverload<int>::of(&QComboBox::currentIndexChanged),       this,  &SeerArrayVisualizerWidget::handleArrayDisplayFormatComboBox);
    QObject::connect(columnCountSpinBox,            QOverload<int>::of(&QSpinBox::valueChanged),               this,  &SeerArrayVisualizerWidget::handleColumnCountSpinBox);
    QObject::connect(printToolButton,               &QToolButton::clicked,                                     this,  &SeerArrayVisualizerWidget::handlePrintButton);
    QObject::connect(saveToolButton,                &QToolButton::clicked,                                     this,  &SeerArrayVisualizerWidget::handleSaveButton);

    // Restore window settings.
    readSettings();
}

SeerArrayVisualizerWidget::~SeerArrayVisualizerWidget () {
}

void SeerArrayVisualizerWidget::setVariableName (const QString& name) {

    setWindowTitle("Seer Array Visualizer - '" + name + "'");

    variableNameLineEdit->setText(name);
    setVariableAddress("");

    if (variableNameLineEdit->text() == "") {
        return;
    }

    // Clear old contents.
    QByteArray array;

    memoryHexEditor->setData(new SeerHexWidget::DataStorageArray(array));

    // Send signal to get variable address.
    emit evaluateVariableExpression(_variableId, variableNameLineEdit->text());
}

QString SeerArrayVisualizerWidget::variableName () const {
    return variableNameLineEdit->text();
}

void SeerArrayVisualizerWidget::setVariableAddress (const QString& address) {

    unsigned long offset = 0;
    bool ok = false;

    if (address.startsWith("0x")) {

        offset = address.toULong(&ok, 16);

        if (ok == true) {
            variableAddressLineEdit->setText(address);
        }else{
            variableAddressLineEdit->setText("not an address");
            offset = 0;
        }

    }else{
        variableAddressLineEdit->setText("not an address");
        offset = 0;
    }

    //qDebug() << address << offset << ok;

    memoryHexEditor->setAddressOffset(offset);
}

QString SeerArrayVisualizerWidget::variableAddress () const {
    return variableAddressLineEdit->text();
}

void SeerArrayVisualizerWidget::handleText (const QString& text) {

    //qDebug() << text;

    if (text.contains(QRegExp("^([0-9]+)\\^done,value="))) {

        // 10^done,value="1"
        // 11^done,value="0x7fffffffd538"

        QString id_text = text.section('^', 0,0);

        if (id_text.toInt() == _variableId) {

            QStringList words = Seer::filterEscapes(Seer::parseFirst(text, "value=", '"', '"', false)).split(' ', QString::SkipEmptyParts);

            setVariableAddress(words.first());
        }

    }else if (text.contains(QRegExp("^([0-9]+)\\^done,memory="))) {

        // 3^done,memory=[{begin="0x0000000000613e70",offset="0x0000000000000000",end="0x0000000000613e71",contents="00"}]
        // 4^done,memory=[{begin="0x0000000000613e70",offset="0x0000000000000000",end="0x0000000000613ed4",contents="000000000000000000000000"}]

        QString id_text = text.section('^', 0,0);

        if (id_text.toInt() == _memoryId) {

            //qDebug() << text;

            QString memory_text = Seer::parseFirst(text, "memory=", '[', ']', false);

            QStringList range_list = Seer::parse(memory_text, "", '{', '}', false);

            // Loop through the memory ranges.
            for ( const auto& range_text : range_list  ) {

                QString contents_text = Seer::parseFirst(range_text, "contents=", '"', '"', false);

                //qDebug() << contents_text;

                // Convert hex string to byte array.
                QByteArray array;

                for (int i = 0; i<contents_text.size(); i += 2) {
                    QString num = contents_text.mid(i, 2);
                    bool ok = false;
                    array.push_back(num.toInt(&ok, 16));
                    assert(ok);
                }

                // Give the byte array to the hex widget.
                memoryHexEditor->setData(new SeerHexWidget::DataStorageArray(array));

                break; // Take just the first range for now.
            }
        }

    }else if (text.contains(QRegExp("^([0-9]+)\\^error,msg="))) {

        // 12^error,msg="No symbol \"return\" in current context."
        // 13^error,msg="No symbol \"cout\" in current context."
        // 3^error,msg="Unable to read memory."

        QString id_text = text.section('^', 0,0);

        if (id_text.toInt() == _variableId) {
            variableAddressLineEdit->setText( Seer::filterEscapes(Seer::parseFirst(text, "msg=", '"', '"', false)) );
        }

        if (id_text.toInt() == _memoryId) {
            // Display the error message.
            QString msg_text = Seer::parseFirst(text, "msg=", false);

            if (msg_text != "") {
                QMessageBox::warning(this, "Error.", Seer::filterEscapes(msg_text));
            }
        }

    }else{
        // Ignore anything else.
    }
}

void SeerArrayVisualizerWidget::handleRefreshButton () {

    if (variableNameLineEdit->text() == "") {
        return;
    }

    if (variableAddressLineEdit->text() == "") {
        return;
    }

    if (variableAddressLineEdit->text() == "not an address") {
        return;
    }

    emit evaluateMemoryExpression(_memoryId, variableAddressLineEdit->text(), arrayLengthLineEdit->text().toInt());
}

void SeerArrayVisualizerWidget::handleVariableNameLineEdit () {

    setVariableName (variableNameLineEdit->text());
}

void SeerArrayVisualizerWidget::handleArrayDisplayFormatComboBox (int index) {

    memoryHexEditor->setMemoryMode(SeerHexWidget::HexMemoryMode);
}

void SeerArrayVisualizerWidget::handleColumnCountSpinBox (int value) {

    memoryHexEditor->setBytesPerLine(value);
}

void SeerArrayVisualizerWidget::handlePrintButton () {

    QPrinter printer;

    QPrintDialog* dlg = new QPrintDialog(&printer, this);

    if (dlg->exec() != QDialog::Accepted) {
        return;
    }


    // Make a copy so we can temporarily add a header.
    QTextDocument* clone = memoryHexEditor->document()->clone(this);

    QTextCursor cursor(clone);
    QTextCharFormat format = cursor.charFormat();
    format.setBackground(QBrush(Qt::transparent));

    cursor.insertText("\n", format);
    cursor.insertText("name=" + variableName() + " address=" + variableAddress() + " bytesPerLine=" + QString::number(memoryHexEditor->bytesPerLine()) + " bytes=" + QString::number(memoryHexEditor->size()) + " memory=" + memoryHexEditor->memoryModeString() + " char=" + memoryHexEditor->charModeString() + "\n", format);
    cursor.insertText("\n", format);

    clone->print(&printer);

    delete clone;
}

void SeerArrayVisualizerWidget::handleSaveButton () {

    QFileDialog dialog(this, "Seer visualizer file", "", "Logs (*.log);;Text files (*.txt);;All files (*.*)");
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    dialog.setFileMode(QFileDialog::AnyFile);
    dialog.setDefaultSuffix("log");
    dialog.selectFile("array.log");

    if (dialog.exec() != QDialog::Accepted) {
        return;
    }

    QStringList files = dialog.selectedFiles();

    if (files.size() == 0) {
        return;
    }

    if (files.size() > 1) {
        QMessageBox::critical(this, tr("Error"), tr("Select only 1 file."));
        return;
    }

    QFile file(files[0]);

    if (file.open(QIODevice::ReadWrite)) {
        QTextStream stream(&file);

        stream << "\n";
        stream << "name=" << variableName() << " address=" << variableAddress() << " bytesPerLine=" << memoryHexEditor->bytesPerLine() << " bytes=" << memoryHexEditor->size() << " memory=" << memoryHexEditor->memoryModeString() << " char=" << memoryHexEditor->charModeString() << "\n";
        stream << "\n";
        stream << memoryHexEditor->toPlainText();
        stream << "\n";

        file.flush();
        file.close();

    }else{
        QMessageBox::critical(this, tr("Error"), tr("Cannot save display to file."));
        return;
    }
}

void SeerArrayVisualizerWidget::writeSettings() {

    QSettings settings;

    settings.beginGroup("arrayvisualizerwindow");
    settings.setValue("size", size());
    settings.endGroup();

    //qDebug() << size();
}

void SeerArrayVisualizerWidget::readSettings() {

    QSettings settings;

    settings.beginGroup("arrayvisualizerwindow");
    resize(settings.value("size", QSize(800, 400)).toSize());
    settings.endGroup();

    //qDebug() << size();
}

void SeerArrayVisualizerWidget::resizeEvent (QResizeEvent* event) {

    writeSettings();

    QWidget::resizeEvent(event);
}

