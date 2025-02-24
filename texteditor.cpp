#include "texteditor.h"
#include "ui_texteditor.h"
#include <QFile>
#include <QFileDialog>
#include <QTextStream>
#include <QMessageBox>
#include <QDateTime>
#include <QFileInfo>
#include <QTextCursor>
#include <QTextCharFormat>
#include <QRegularExpression>
#include <QDebug>
#include <QCloseEvent>
#include <QMessageBox>

TextEditor::TextEditor(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::TextEditor)
{
    ui->setupUi(this);
    this->setCentralWidget(ui->textEdit);
    ui->textEdit->setStyleSheet("background-color: white; color: black;");

    connect(ui->textEdit, &QTextEdit::textChanged, this, &TextEditor::on_textEdit_textChanged);
}

TextEditor::~TextEditor()
{
    delete ui;
}


void TextEditor::on_actionNew_triggered()
{
    file_path.clear();
    ui->textEdit->clear();
}

void TextEditor::on_actionOpen_triggered()
{
    QString file_name = QFileDialog::getOpenFileName(this, "Open File");
    if (file_name.isEmpty()) {
        QMessageBox::information(this, "Open File", "No file selected.");
        return;
    }

    QFile file(file_name);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "Error", "Could not open the file: " + file.errorString());
        return;
    }

    QTextStream in(&file);
    QString text = in.readAll();
    ui->textEdit->setText(text);
    file.close();
    file_path = file_name;
}

void TextEditor::on_actionSave_As_triggered()
{
    QString file_name = QFileDialog::getSaveFileName(this, "Save As");

    if (file_name.isEmpty())
    {
        QMessageBox::information(this, "Save As", "Save As operation canceled.");
        return;
    }

    file_path = file_name;
    QDateTime currentDateTime = QDateTime::currentDateTime();
    QString timestamp = currentDateTime.toString("yyyyMMddhhmmss");
    QString file_name_with_timestamp = file_path + "_" + timestamp;

    QFile file(file_name_with_timestamp);
    file_path = file_name_with_timestamp;
    QMessageBox::information(this, "File Saved", "File saved as: " + file_name_with_timestamp);

    if (!file.open(QFile::WriteOnly | QFile::Text))
    {
        QMessageBox::warning(this, "..", "File not open");
        return;
    }

    QTextStream out(&file);
    QString text = ui->textEdit->toPlainText();

    removeExtraSpaces(text);


    ui->textEdit->setText(text);
    out << text;
    file.flush();
    file.close();
}

void TextEditor::on_actionSave_triggered()
{
    if (file_path.isEmpty())
    {
        on_actionSave_As_triggered(); // If file_path is empty, perform Save As
        return;
    }

    QFile file(file_path);
    if (!file.open(QFile::WriteOnly | QFile::Text))
    {
        QMessageBox::warning(this, "..", "File not open");
        return;
    }

    QTextStream out(&file);
    QString text = ui->textEdit->toPlainText();

    removeExtraSpaces(text);
    ui->textEdit->setText(text);

    out << text;
    file.flush();
    file.close();
}


void TextEditor::on_actionCut_triggered()
{
    ui->textEdit->cut();
}

void TextEditor::on_actionCopy_triggered()
{
    ui->textEdit->copy();
}

void TextEditor::on_actionPaste_triggered()
{
    ui->textEdit->paste();
}

void TextEditor::on_actionUndo_triggered()
{
    ui->textEdit->undo();
}

void TextEditor::on_actionRedo_triggered()
{
    ui->textEdit->redo();
}

void TextEditor::on_actionAbout_triggered()
{
    QMessageBox::about(this, "About", "This is a simple text editor, Created By R. Priyatharsan");
}

void TextEditor::on_actionZoom_in_triggered()
{
    ui->textEdit->zoomIn();
}

void TextEditor::on_actionZoom_out_triggered()
{
    ui->textEdit->zoomOut();
}

void TextEditor::on_textEdit_textChanged()
{
    highlightErrors(); // Highlight errors whenever text changes
    capitalizeFirstLetterOfSentences(); // Capitalize first letter of sentences
    capitalizeFirstLetter();
}

void TextEditor::capitalizeFirstLetter()
{
    // Get the text content from the QTextEdit
    QString text = ui->textEdit->toPlainText();

    // Ensure the text is not empty
    if (text.isEmpty()) {
        return;
    }

    // Capitalize the first letter of the text
    QString newText = text;
    newText[0] = newText[0].toUpper();

    // Update the QTextEdit if the text has changed
    if (newText != text) {
        ui->textEdit->setPlainText(newText);

        // Move the cursor to the end to avoid losing focus
        QTextCursor cursor(ui->textEdit->document());
        cursor.movePosition(QTextCursor::End);
        ui->textEdit->setTextCursor(cursor);
    }
}

void TextEditor::removeExtraSpaces(QString &text)
{
    while (text.contains("  "))
    {
        text.replace("  ", " ");
    }
}

void TextEditor::highlightErrors()
{
    QList<QTextEdit::ExtraSelection> extraSelections;
    QTextEdit::ExtraSelection selection;

    // Define the wavy underline format
    QTextCharFormat format;
    format.setUnderlineStyle(QTextCharFormat::WaveUnderline);
    format.setUnderlineColor(Qt::red);
    format.setBackground(QBrush(QColor(255, 200, 200)));
    format.setFontWeight(QFont::Bold);


    // Get the text content
    QString text = ui->textEdit->toPlainText();

    // Regular expression to find occurrences where a period is not followed by exactly one space
    QRegularExpression regex("([.])([^\\s])");
    QRegularExpressionMatchIterator matches = regex.globalMatch(text);

    QTextCursor cursor(ui->textEdit->document());

    while (matches.hasNext()) {
        QRegularExpressionMatch match = matches.next();
        int start = match.capturedStart(2);
        int end = match.capturedEnd(2);

        // Set cursor positions and format
        cursor.setPosition(start);
        cursor.setPosition(end, QTextCursor::KeepAnchor);

        selection.cursor = cursor;
        selection.format = format;
        extraSelections.append(selection);

        // Reset the cursor for the next match
        cursor.clearSelection();
    }

    // Apply extra selections to the text edit
    ui->textEdit->setExtraSelections(extraSelections);
}

void TextEditor::capitalizeFirstLetterOfSentences()
{
    QTextCursor cursor(ui->textEdit->document());
    cursor.select(QTextCursor::Document);
    QString text = cursor.selectedText();

    // Regular expression to find the start of a sentence
    QRegularExpression regex("([.!?]\\s+)(\\w)");
    QString result = text;
    QRegularExpressionMatchIterator matches = regex.globalMatch(text);

    while (matches.hasNext()) {
        QRegularExpressionMatch match = matches.next();
        int start = match.capturedStart(2);
        int end = match.capturedEnd(2);

        if (start < text.length()) {
            // Capitalize the first letter
            QString capitalizedLetter = text.mid(start, 1).toUpper();
            result.replace(start, 1, capitalizedLetter);
        }
    }

    // Update the text if any changes were made
    if (result != text) {
        ui->textEdit->setText(result);
        // Move cursor to the end of the document
        cursor.movePosition(QTextCursor::End);
        ui->textEdit->setTextCursor(cursor);
    }
}

void TextEditor::on_actionToggle_Theme_triggered()
{
    static bool isDarkTheme = false;

    if (isDarkTheme) {
        // Set white theme
        this->setStyleSheet(""); // Reset to default style
        ui->textEdit->setStyleSheet("background-color: white; color: black;");
    } else {
        // Set dark theme
        this->setStyleSheet("QMainWindow { background-color: #2E3440; }"
                            "QMenuBar { background-color: #3B4252; color: #ECEFF4; }"
                            "QMenuBar::item:selected { background-color: #4C566A; }"
                            "QMenu { background-color: #3B4252; color: #ECEFF4; }"
                            "QMenu::item:selected { background-color: #4C566A; }"
                            "QTextEdit { background-color: #3B4252; color: #ECEFF4; }"
                            "QToolBar { background-color: #3B4252; }");
        ui->textEdit->setStyleSheet("background-color: #3B4252; color: #ECEFF4;");
    }

    isDarkTheme = !isDarkTheme; // Toggle the theme state
}

void TextEditor::closeEvent(QCloseEvent *event)
{
    // Check if the document has been modified
    if (ui->textEdit->document()->isModified()) {

        QMessageBox::StandardButton ret = QMessageBox::warning(this, tr("Warning"),
                              tr("The document has been modified.\n"
                              "Do you want to save your changes?"),
               QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);

        if (ret == QMessageBox::Save) {
            // Save the file and accept the close event
            on_actionSave_triggered();
            event->accept();
        } else if (ret == QMessageBox::Cancel) {
            // Ignore the close event
            event->ignore();
        } else {
            // Discard changes and accept the close event
            event->accept();
        }
    } else {
        // Document is not modified, accept the close event
        event->accept();
    }
}
