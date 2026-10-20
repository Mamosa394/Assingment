#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include <QTextEdit>
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

#include "hashsetopen.h"
#include "hashsetclosed.h"
#include "dijkstra.h"

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
    void addOpenItem();
    void removeOpenItem();
    void addClosedItem();
    void removeClosedItem();
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

    // Open Hashing Vars
    HashSetOpen* openSet;
    QLineEdit* openKeyInput;
    QLineEdit* openValueInput;
    QTextEdit* openOutput;

    // Closed Hashing Vars
    HashSetClosed* closedSet;
    QLineEdit* closedKeyInput;
    QLineEdit* closedValueInput;
    QTextEdit* closedOutput;

    // Dijkstra Vars
    QComboBox* startDropdown;
    QComboBox* targetDropdown;
    QLabel* resultDistanceLabel;
    QLabel* resultPathLabel;
    QGraphicsScene* mapScene;
    void drawMap(const QString& highlightStart, const QString& highlightTarget);

    // File Vars
    QLineEdit* filePathEdit;
    QTextEdit* fileOutput;
};
#endif