#include <QApplication>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QTextEdit>
#include <QFileDialog>
#include <QClipboard>
#include <QUrl>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>

class FileProcessorGUI : public QWidget {
    Q_OBJECT
public:
    FileProcessorGUI(QWidget *parent = nullptr) : QWidget(parent) {
        initUI();
    }

private slots:
    void browseFile() {
        QString filePath = QFileDialog::getOpenFileName(
            this, "选择文件", "", "所有文件 (*);;LaTeX源文件 (*.tex)");
        if (!filePath.isEmpty()) {
            fileInput->setText(filePath);
            if (fileOutput->text().isEmpty()) {
                fileOutput->setText(filePath.section('/', 0, -2));
            }
        }
    }

    void browseOutputFile() {
        QString dir = QFileDialog::getExistingDirectory(this, "选择输出目录");
        if (!dir.isEmpty()) {
            fileOutput->setText(dir);
        }
    }

    void processFile() {
        QString inputPath = fileInput->text();
        if (inputPath.isEmpty()) {
            outputArea->setText("请先选择文件！");
            return;
        }

        try {
            QFile inputFile(inputPath);
            if (!inputFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
                throw std::runtime_error("无法打开输入文件");
            }

            QTextStream in(&inputFile);
            in.setEncoding(QStringConverter::Utf8);
            QString content = in.readAll();
            if (content.contains("ctex") || content.contains("xeCJK")) {
                content.prepend("% !TEX program = xelatex\n");
            }
            inputFile.close();

            QString outputPath = fileOutput->text() + "/onlineLaTeX.bat";
            QFile outputFile(outputPath);
            if (!outputFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
                throw std::runtime_error("无法创建输出文件");
            }

            QTextStream out(&outputFile);
            out.setEncoding(QStringConverter::Utf8);
            out << "@echo off\n";

            QString url = "https://texlive.net/run?" + QUrl::toPercentEncoding(content);
            QString command = "start " + url.replace("%", "%%");
            out << command;
            outputFile.close();

            outputArea->setText(
                QString("处理完成，输出文件地址生成的onlineLaTeX.bat文件可以一键打开在线分享的网址，具体网址为\n\n%1")
                .arg(url));
        } catch (const std::exception& e) {
            outputArea->setText(QString("处理出错：%1").arg(e.what()));
        }
    }

    void copyOutput() {
        QString text = outputArea->toPlainText();
        if (!text.isEmpty() && text.contains('\n')) {
            QString url = text.section('\n', -1).replace("%%", "%");
            QApplication::clipboard()->setText(url);
            outputArea->setText("内容已复制到剪切板");
        } else {
            outputArea->setText("没有可复制的内容！");
        }
    }

private:
    void initUI() {
        // 创建组件
        QLabel *fileLabel = new QLabel("文件路径:      ");
        QLabel *outputLabel = new QLabel("输出.bat文件路径");
        fileInput = new QLineEdit();
        fileOutput = new QLineEdit();
        QPushButton *browseBtn = new QPushButton("浏览...");
        QPushButton *browseOutBtn = new QPushButton("浏览...");
        QPushButton *processBtn = new QPushButton("处理文件");
        QPushButton *copyBtn = new QPushButton("复制结果");
        outputArea = new QTextEdit();
        outputArea->setReadOnly(true);

        // 布局管理
        QHBoxLayout *fileLayout = new QHBoxLayout();
        fileLayout->addWidget(fileLabel);
        fileLayout->addWidget(fileInput);
        fileLayout->addWidget(browseBtn);

        QHBoxLayout *outputLayout = new QHBoxLayout();
        outputLayout->addWidget(outputLabel);
        outputLayout->addWidget(fileOutput);
        outputLayout->addWidget(browseOutBtn);

        QVBoxLayout *mainLayout = new QVBoxLayout();
        mainLayout->addLayout(fileLayout);
        mainLayout->addLayout(outputLayout);
        mainLayout->addWidget(processBtn);
        mainLayout->addWidget(copyBtn);
        mainLayout->addWidget(outputArea);

        setLayout(mainLayout);
        setWindowTitle("LaTeX文件转在线网站查看（仅支持单文件）");
        resize(600, 400);

        // 连接信号槽
        connect(browseBtn, &QPushButton::clicked, this, &FileProcessorGUI::browseFile);
        connect(browseOutBtn, &QPushButton::clicked, this, &FileProcessorGUI::browseOutputFile);
        connect(processBtn, &QPushButton::clicked, this, &FileProcessorGUI::processFile);
        connect(copyBtn, &QPushButton::clicked, this, &FileProcessorGUI::copyOutput);
    }

    QLineEdit *fileInput;
    QLineEdit *fileOutput;
    QTextEdit *outputArea;
};

#include "main.moc"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    FileProcessorGUI window;
    window.show();
    return app.exec();
}