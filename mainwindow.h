#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include <QTextEdit>
#include <QTextCursor>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFileDialog>
#include <QMessageBox>
#include <QFile>
#include <QRegularExpression>
#include <QStringList>
#include <QFrame>
#include <QStyle>
#include <QComboBox>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsEllipseItem>
#include <QGraphicsTextItem>
#include <QGraphicsLineItem>
#include <unordered_map>
#include <string>

#include "hashsetopen.h"
#include "hashsetclosed.h"
#include "dijkstra.h"
#include "hashstrategy.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void showOpenPage();
    void showClosedPage();
    void showDijkstraPage();
    void showFilePage();
    void setActiveButton(QPushButton* btn);

private slots:
    void createOpenTable();
    void addOpenItem();
    void removeOpenItem();

    void createClosedTable();
    void addClosedItem();
    void removeClosedItem();
    void doClosedGet();
    void doClosedContains();
    void clearClosedSet();
    void showClosedStatus(const QString& msg);

    void calculateRoute();
    void browseFile();
    void runDijkstraFile();

private:
    QStackedWidget *stack;
    QWidget* buildSidebar();
    QWidget* buildHeader(const QString& title, const QString& subtitle);
    QFrame* createCard(QWidget* content);

    QWidget* buildOpenPage();
    QWidget* buildClosedPage();
    QWidget* buildDijkstraPage();
    QWidget* buildFilePage();

    QPushButton* btnOpen;
    QPushButton* btnClosed;
    QPushButton* btnDijkstra;
    QPushButton* btnFile;

    // Open Hashing
    HashSetOpen* openSet;
    QLineEdit* openKeyInput;
    QLineEdit* openValueInput;
    QTextEdit* openOutput;
    QLabel*    openCalcPreview;
    QTextEdit* openProcessLog;

    // Open Hash Config
    QComboBox* openHashFnCombo;
    QLineEdit* openModulusEdit;
    QLineEdit* openUniversalA;
    QLineEdit* openUniversalB;
    QLineEdit* openUniversalP;
    QLabel*    openHashSummary;

    // Closed Hashing
    HashSetClosed* closedSet;
    QLineEdit* closedKeyInput;
    QLineEdit* closedValueInput;
    QTextEdit* closedOutput;
    QLabel*    closedStatusLabel;
    QLabel*    closedCalcPreview;
    QTextEdit* closedProcessLog;

    // Closed Hash Config
    QComboBox* closedHashFnCombo;
    QLineEdit* closedModulusEdit;
    QLineEdit* closedUniversalA;
    QLineEdit* closedUniversalB;
    QLineEdit* closedUniversalP;
    QLabel*    closedHashSummary;

    // Dijkstra
    QComboBox* startDropdown;
    QComboBox* targetDropdown;
    QLabel* resultDistanceLabel;
    QLabel* resultPathLabel;
    QTextEdit* resultDistancesTable;
    QGraphicsScene* mapScene;

    void drawMap(const QStringList& pathVertices);
    QString formatDistanceTable(const std::unordered_map<std::string, int>& distances,
                                const std::string& startNode,
                                const std::string& targetNode) const;

    // File
    QLineEdit* filePathEdit;
    QTextEdit* fileOutput;
};
#endif