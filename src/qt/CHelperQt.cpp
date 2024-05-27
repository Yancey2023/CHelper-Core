//
// Created by Yancey on 2024-05-25.
//

#include "CHelperQt.h"
#include "ui_chelper.h"
#include <QDebug>
#include <QFile>
#include <QListWidget>
#include <QMessageBox>
#include <QStringListModel>
#include <QClipboard>

CHelperApp::CHelperApp(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::CHelperApp) {
    ui->setupUi(this);
#ifdef _WIN32
    QFile file(":/assets/beta-experiment-1.21.0.23.cpack");
    if (file.open(QIODevice::ReadOnly) && file.isReadable()) {
        std::istringstream iss(file.readAll().toStdString());
        core = CHelper::Core::create([&iss]() {
            CHelper::BinaryReader binaryReader(true, iss);
            return CHelper::CPack::createByBinary(binaryReader);
        });
    }
#else
    core = CHelper::Core::createByBinary(R"(/home/yancey/CLionProjects/CHelper-Core/run/beta-experiment-1.21.0.23.cpack)");
#endif
    if (HEDLEY_UNLIKELY(core == nullptr)) {
        throw std::runtime_error("fail to load cpack");
    }
    ui->listView->setModel(new QStringListModel(this));
    ui->listView->setMovement(QListView::Static);
    ui->listView->setEditTriggers(QListView::NoEditTriggers);
    ui->listView->setVerticalScrollMode(QListView::ScrollPerPixel);
    ui->listView->setSpacing(2);
    onTextChanged(nullptr);
    ui->lineEdit->setFocus();
    connect(ui->listView, SIGNAL(clicked(QModelIndex)), this, SLOT(onSuggestionClick(QModelIndex)));
    connect(ui->lineEdit, SIGNAL(textChanged(QString)), this, SLOT(onTextChanged(QString)));
    connect(ui->copyButton, &QPushButton::clicked, this, &CHelperApp::copy);
}

CHelperApp::~CHelperApp() {
    delete ui;
}

void CHelperApp::onTextChanged(const QString &string) {
    if (HEDLEY_UNLIKELY(core == nullptr)) {
        return;
    }
    core->onTextChanged(string.toStdString(), string.length());
    if (HEDLEY_UNLIKELY(string == nullptr)) {
        ui->structureLabel->setText("欢迎使用CHelper");
        ui->descriptionLabel->setText("作者：Yancey");
        ui->errorReasonLabel->setText(nullptr);
    } else {
        ui->structureLabel->setText(QString::fromStdString(core->getStructure()));
        ui->descriptionLabel->setText(QString::fromStdString(core->getDescription()));
        std::vector<std::shared_ptr<CHelper::ErrorReason>> errorReasons = core->getErrorReasons();
        if (HEDLEY_UNLIKELY(errorReasons.empty())) {
            ui->errorReasonLabel->setText(nullptr);
        } else if (HEDLEY_UNLIKELY(errorReasons.size() == 1)) {
            ui->errorReasonLabel->setText(QString::fromStdString(errorReasons[0]->errorReason));
        } else {
            QString result = "可能的错误原因：";
            int i = 0;
            for (const auto &item: errorReasons) {
                result.append("\n").append(QString().setNum(++i)).append(". ").append(QString::fromStdString(item->errorReason));
            }
            ui->errorReasonLabel->setText(result);
        }
    }
    std::vector<CHelper::Suggestion> *suggestions = core->getSuggestions();
    QStringList list;
    for (const CHelper::Suggestion &suggestion: *suggestions) {
        list.append(QString::fromStdString(
                suggestion.content->description.has_value()
                        ? suggestion.content->name + " - " + suggestion.content->description.value()
                        : suggestion.content->name));
    }
    ((QStringListModel *) ui->listView->model())->setStringList(list);
    ui->listView->scrollToTop();
}

void CHelperApp::onSuggestionClick(const QModelIndex &index) {
    if (HEDLEY_UNLIKELY(core == nullptr)) {
        return;
    }
    std::optional<std::string> result = core->onSuggestionClick(index.row());
    if (HEDLEY_LIKELY(result.has_value())) {
        ui->lineEdit->setText(QString::fromStdString(result.value()));
        ui->lineEdit->setFocus();
    } else {
        qDebug() << "suggestion index is out of range: " << index.row();
    }
}

void CHelperApp::copy() {
    QClipboard *clip = QApplication::clipboard();
    clip->setText(ui->lineEdit->text());
}


void CHelperApp::about() {
    QMessageBox::about(this, tr("About"),
                       tr("Welcome to use <b>CHelper</b>. It is made by Yancey."));
}

int main(int argc, char *argv[]) {
    QApplication application(argc, argv);
    CHelperApp app;
    app.show();
    return QApplication::exec();
}
