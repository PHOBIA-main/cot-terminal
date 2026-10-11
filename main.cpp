#include <QApplication>
#include <QMainWindow>
#include <QVBoxLayout>
#include <QTextEdit>
#include <QLineEdit>
#include <QProcess>
#include <QScrollBar>
#include <QTextCursor>
#include <QRegularExpression>

class TerminalWindow : public QMainWindow {
    Q_OBJECT

public:
    TerminalWindow(QWidget *parent = nullptr) : QMainWindow(parent) {
        setWindowTitle("MyTerminal");
        resize(950, 750);

        QWidget *centralWidget = new QWidget(this);
        QVBoxLayout *layout = new QVBoxLayout(centralWidget);
        layout->setContentsMargins(10, 10, 10, 10);
        layout->setSpacing(8);

        outputArea = new QTextEdit(this);
        outputArea->setReadOnly(true);
        outputArea->setAcceptRichText(true);
        outputArea->setStyleSheet(
            "QTextEdit { "
            "   background-color: #0b0f19; "
            "   color: #ef4444; "
            "   font-family: monospace; "
            "   font-size: 10pt; "
            "   border: 2px solid #0284c7; "
            "   border-radius: 10px; "
            "   padding: 8px; "
            "} "
            "QScrollBar:vertical { "
            "   background: #0b0f19; "
            "   width: 8px; "
            "   margin: 0px; "
            "   border-radius: 4px; "
            "} "
            "QScrollBar::handle:vertical { "
            "   background: #0284c7; "
            "   border-radius: 4px; "
            "   min-height: 20px; "
            "} "
            "QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { "
            "   height: 0px; "
            "}"
        );

        inputLine = new QLineEdit(this);
        inputLine->setStyleSheet(
            "QLineEdit { "
            "   background-color: #0f172a; "
            "   color: #38bdf8; "
            "   font-family: monospace; "
            "   font-size: 12pt; "
            "   padding: 8px; "
            "   border: 2px solid #0284c7; "
            "   border-radius: 8px; "
            "} "
            "QLineEdit:focus { "
            "   border: 2px solid #38bdf8; "
            "}"
        );

        layout->addWidget(outputArea);
        layout->addWidget(inputLine);
        setCentralWidget(centralWidget);

        showStartupScreen();

        process = new QProcess(this);
        QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
        env.insert("TERM", "xterm-256color");
        process->setProcessEnvironment(env);

        connect(inputLine, &QLineEdit::returnPressed, this, &TerminalWindow::runCommand);

        connect(process, &QProcess::readyReadStandardOutput, this, [this]() {
            QByteArray data = process->readAllStandardOutput();
            appendFormattedOutput(QString::fromUtf8(data));
        });

        connect(process, &QProcess::readyReadStandardError, this, [this]() {
            QByteArray data = process->readAllStandardError();
            appendFormattedOutput(QString::fromUtf8(data));
        });
    }

private slots:
    void runCommand() {
        QString cmd = inputLine->text().trimmed();
        if (cmd.isEmpty()) return;

        if (cmd == "clear") {
            showStartupScreen();
            inputLine->clear();
            return;
        }

        if (cmd == "help") {
            appendCommandPrompt("help");
            appendFormattedOutput(
                "Доступные встроенные команды:\n"
                "  • help  - показать эту справку\n"
                "  • clear - очистить экран и вернуть стартовый баннер\n"
                "  • sudo <команда> - выполнить команду с правами администратора (через pkexec)\n\n"
                "Также вы можете выполнять любые стандартные утилиты CachyOS (например, ls, uname -a, pacman и др.).\n"
            );
            inputLine->clear();
            return;
        }

        if (process->state() == QProcess::Running) {
            appendCommandPrompt(cmd);
            appendFormattedOutput("Ошибка: Предыдущий процесс еще выполняется! Подождите завершения.\n");
            inputLine->clear();
            return;
        }

        appendCommandPrompt(cmd);
        inputLine->clear();

        if (cmd.startsWith("sudo ")) {
            cmd.replace(0, 5, "pkexec ");
        } else if (cmd == "sudo") {
            cmd = "pkexec";
        }

        process->start("sh", QStringList() << "-c" << cmd);
    }

private:
    void showStartupScreen() {
        QString startupHtml =
        "<html><body style=\"color:#ef4444; font-family:monospace; background-color:#0b0f19;\">"
        "<table style=\"font-family:monospace; border-collapse:collapse;\">"
        "<tr>"
        "<td style=\"vertical-align:top; padding-right:15px; color:#ffffff;\"><pre>"
        "                                                        \n"
        "    .@                                                      \n"
        "     .@+                                                    \n"
        "       %@:                                                  \n"
        "        +@@                                                 \n"
        "         :@@*                                               \n"
        "          .@@@.                                            \n"
        "            %@*                                    \n"
        "             *@@@.               @@*@#                     \n"
        "              =@@@%             #@: .@@:    %@@@::@       \n"
        "               .@@@@.           %@    *@@@@@@.   #@         \n"
        "                .@@@@@@@==..   .@@  :@@@@@@@@=  @@          \n"
        "                  @@@@@@@@@@@@@@@@@@@@@@@@@@@=.@#           \n"
        "                   .@@@@@@@@@@@@@@@@@@@@@@@@@@@-            \n"
        "                     -@@@@@@@@@@@@@@@@@@@@@@@@.             \n"
        "                      .%@@@#:@@@@@@.*@@@@@@@=               \n"
        "                        .@@@@.:@@@@@=  -@#@                 \n"
        "                         :@@@. .@@@@*  %                   \n"
        "                            %@@=  .@@@@. .@#                \n"
        "                              +@#   :@@@- .@#               \n"
        "                                 @    #@@%  =@@%            \n"
        "                                         @@. .%@@*          \n"
        "                                                    \n"
        "</pre></td>"
        "<td style=\"vertical-align:top; color:#38bdf8;\"><pre>"
        "<b>phobia@my-os</b>\n"
        "------------------------\n"
        "<b>OS:</b> CachyOS (KDE Plasma)\n"
        "<b>Host:</b> ASUS TUF Gaming\n"
        "<b>GPU:</b> NVIDIA RTX 4060\n"
        "</pre></td>"
        "</tr>"
        "</table>"
        "<br><span style=\"color:#38bdf8;\">Введите системную команду (например, help, ls, sudo pacman -Syu, clear):</span><br><br>"
        "</body></html>";
        outputArea->setHtml(startupHtml);
    }

    void appendCommandPrompt(QString cmd) {
        QTextCursor cursor = outputArea->textCursor();
        cursor.movePosition(QTextCursor::End);
        outputArea->setTextCursor(cursor);
        outputArea->insertHtml("<br><span style=\"color:#38bdf8; font-weight:bold;\">> " + cmd + "</span><br>");
        outputArea->verticalScrollBar()->setValue(outputArea->verticalScrollBar()->maximum());
    }

    void appendFormattedOutput(QString text) {
        text.replace(QRegularExpression("\033\\[[0-9;]*[a-zA-Z]"), "");
        text.replace(QRegularExpression("\033\\(B"), "");

        text.replace("\n", "<br>");
        text.replace(" ", "&nbsp;");

        QTextCursor cursor = outputArea->textCursor();
        cursor.movePosition(QTextCursor::End);
        outputArea->setTextCursor(cursor);
        outputArea->insertHtml("<span style=\"color:#ef4444;\">" + text + "</span>");
        outputArea->verticalScrollBar()->setValue(outputArea->verticalScrollBar()->maximum());
    }

    QTextEdit *outputArea;
    QLineEdit *inputLine;
    QProcess *process;
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    TerminalWindow window;
    window.show();
    return app.exec();
}

#include "main.moc"
