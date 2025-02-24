#ifndef TEXTEDITOR_H
#define TEXTEDITOR_H

#include <QMainWindow>
#include <QTextEdit>

QT_BEGIN_NAMESPACE
namespace Ui { class TextEditor; }
QT_END_NAMESPACE

class TextEditor : public QMainWindow
{
    Q_OBJECT

public:
    explicit TextEditor(QWidget *parent = nullptr);
    ~TextEditor();

protected:
    void closeEvent(QCloseEvent *event) override;


private slots:
    void removeExtraSpaces(QString& text);

    void on_actionNew_triggered();
    void on_actionOpen_triggered();
    void on_actionSave_triggered();
    void on_actionSave_As_triggered();
    void on_actionCut_triggered();
    void on_actionCopy_triggered();
    void on_actionPaste_triggered();
    void on_actionUndo_triggered();
    void on_actionRedo_triggered();
    void on_actionAbout_triggered();
    void on_actionZoom_in_triggered();
    void on_actionZoom_out_triggered();
    void on_textEdit_textChanged();
    void on_actionToggle_Theme_triggered();




private:
    void saveFile(const QString &fileName);
    void highlightErrors();
    void capitalizeFirstLetterOfSentences();
    void capitalizeFirstLetter();

    Ui::TextEditor *ui;
    QString file_path; // Path of the current file

};

#endif // TEXTEDITOR_H
