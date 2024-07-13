//
// Created by Yancey on 2024-05-25.
//

#ifndef CHELPER_CHELPERQT_H
#define CHELPER_CHELPERQT_H

#include "../chelper/Core.h"
#include <QMainWindow>
#include <QModelIndex>
#include <QStyledItemDelegate>

QT_BEGIN_NAMESPACE
namespace Ui {
    class CHelperApp;
}
QT_END_NAMESPACE

class CHelperApp : public QMainWindow {
    Q_OBJECT

public:
    explicit CHelperApp(QWidget *parent = nullptr);

    ~CHelperApp() override;


private slots:
    void onTextChanged(const QString &string);

    void onSuggestionClick(const QModelIndex &index);

    void copy();

    void about();

private:
    Ui::CHelperApp *ui;
    CHelper::Core *core;
};

int main(int argc, char *argv[]);

#endif//CHELPER_CHELPERQT_H
