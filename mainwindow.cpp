#include "mainwindow.h"
#include <QPixmap>
#include <QGraphicsDropShadowEffect>
#include <QPen>
#include <QBrush>
#include <QFont>
#include <QScrollArea>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    setWindowTitle("Data Structures Assignment 2026");
    resize(1200, 850);

    openSet = new HashSetOpen(7);
    closedSet = new HashSetClosed(7);

    setStyleSheet(
        "QMainWindow { background-color: #ffffff; }"
        "QWidget { color: #202124; font-family: 'Segoe UI', Arial, sans-serif; background-color: #ffffff; }"

        /* SIDEBAR */
        "QWidget#Sidebar { background-color: #000000; }"
        "QLabel#SidebarTitle { color: #ffffff; font-size: 22px; font-weight: 800; letter-spacing: 1px; }"
        "QLabel#SidebarText { color: #9aa0a6; font-size: 13px; }"
        "QPushButton#MenuBtn { background-color: transparent; color: #9aa0a6; border: none; text-align: left; padding: 14px 20px; font-size: 15px; font-weight: 600; border-radius: 8px; }"
        "QPushButton#MenuBtn:hover { color: #ffffff; background-color: #2d2d2d; }"
        "QPushButton#MenuBtn[active='true'] { color: #1a1a1a; background-color: #ffffff; }"

        /* BUTTONS */
        "QPushButton#ActionBtn { background-color: #1a1a1a; color: #ffffff; border: none; border-radius: 6px; padding: 12px 24px; font-weight: bold; font-size: 14px; min-width: 100px; }"
        "QPushButton#ActionBtn:hover { background-color: #333333; }"
        "QPushButton#DangerBtn { background-color: #d93025; color: #ffffff; border: none; border-radius: 6px; padding: 12px 24px; font-weight: bold; font-size: 14px; min-width: 100px; }"
        "QPushButton#DangerBtn:hover { background-color: #b3261e; }"

        /* INPUTS */
        "QLineEdit, QComboBox { background-color: #ffffff; border: 1px solid #dadce0; border-radius: 6px; padding: 12px 16px; font-size: 14px; color: #202124; }"
        "QLineEdit:focus, QComboBox:focus { border: 2px solid #1a1a1a; }"
        "QComboBox::drop-down { border: none; width: 30px; }"

        /* TEXT BOXES */
        "QTextEdit { background-color: #ffffff; border: 1px solid #dadce0; border-radius: 8px; font-family: 'Consolas', monospace; font-size: 14px; color: #202124; padding: 15px; }"

        /* CARDS & TYPOGRAPHY */
        "QFrame#Card { background-color: #ffffff; border: 1px solid #dadce0; border-radius: 12px; }"
        "QLabel#PageTitle { font-size: 32px; font-weight: 800; color: #000000; margin-bottom: 5px; }"
        "QLabel#PageSubtitle { font-size: 15px; color: #5f6368; margin-bottom: 25px; }"
        "QLabel#ResultLabel { font-size: 52px; font-weight: 800; color: #000000; }"
        "QLabel#SubText { font-size: 12px; font-weight: 800; color: #5f6368; letter-spacing: 1px; margin-bottom: 5px; }"
        );

    QWidget* central = new QWidget;
    QHBoxLayout* mainLayout = new QHBoxLayout(central);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    mainLayout->addWidget(buildSidebar());

    stack = new QStackedWidget(this);
    stack->addWidget(buildOpenPage());
    stack->addWidget(buildClosedPage());
    stack->addWidget(buildDijkstraPage());
    stack->addWidget(buildFilePage());

    mainLayout->addWidget(stack);
    setCentralWidget(central);

    showDijkstraPage();
}

MainWindow::~MainWindow() {
    delete openSet;
    delete closedSet;
}

void MainWindow::setActiveButton(QPushButton* btn) {
    QList<QPushButton*> btns = {btnOpen, btnClosed, btnDijkstra, btnFile};
    for (auto b : btns) {
        b->setProperty("active", b == btn);
        b->style()->unpolish(b);
        b->style()->polish(b);
    }
}

QWidget* MainWindow::buildSidebar() {
    QWidget* sidebar = new QWidget;
    sidebar->setObjectName("Sidebar");
    sidebar->setFixedWidth(260);
    QVBoxLayout* layout = new QVBoxLayout(sidebar);
    layout->setContentsMargins(20, 40, 20, 40);

    QLabel* title = new QLabel("Data Structures");
    title->setObjectName("SidebarTitle");
    QLabel* desc = new QLabel("Assignment 2026");
    desc->setObjectName("SidebarText");

    layout->addWidget(title);
    layout->addWidget(desc);
    layout->addSpacing(40);

    auto addMenuBtn = [&](QString text, QPushButton** btn, auto slot) {
        *btn = new QPushButton(text);
        (*btn)->setObjectName("MenuBtn");
        (*btn)->setCursor(Qt::PointingHandCursor);
        connect(*btn, &QPushButton::clicked, this, slot);
        layout->addWidget(*btn);
        layout->addSpacing(8);
    };

    addMenuBtn("Open Hashing", &btnOpen, &MainWindow::showOpenPage);
    addMenuBtn("Closed Hashing", &btnClosed, &MainWindow::showClosedPage);
    addMenuBtn("Dijkstra Router", &btnDijkstra, &MainWindow::showDijkstraPage);
    addMenuBtn("Load Custom Graph", &btnFile, &MainWindow::showFilePage);

    layout->addStretch();
    return sidebar;
}

QWidget* MainWindow::buildHeader(const QString& title, const QString& subtitle) {
    QWidget* header = new QWidget;
    QVBoxLayout* layout = new QVBoxLayout(header);
    layout->setContentsMargins(0, 0, 0, 0);
    QLabel* titleLbl = new QLabel(title);
    titleLbl->setObjectName("PageTitle");
    QLabel* subLbl = new QLabel(subtitle);
    subLbl->setObjectName("PageSubtitle");
    layout->addWidget(titleLbl);
    layout->addWidget(subLbl);
    return header;
}

QFrame* MainWindow::createCard(QWidget* content) {
    QFrame* card = new QFrame;
    card->setObjectName("Card");
    QGraphicsDropShadowEffect* shadow = new QGraphicsDropShadowEffect();
    shadow->setBlurRadius(20);
    shadow->setXOffset(0);
    shadow->setYOffset(4);
    shadow->setColor(QColor(0, 0, 0, 15));
    card->setGraphicsEffect(shadow);
    QVBoxLayout* layout = new QVBoxLayout(card);
    layout->setContentsMargins(35, 35, 35, 35);
    layout->addWidget(content);
    return card;
}

void MainWindow::showOpenPage() { stack->setCurrentIndex(0); setActiveButton(btnOpen); }
void MainWindow::showClosedPage() { stack->setCurrentIndex(1); setActiveButton(btnClosed); }
void MainWindow::showDijkstraPage() { stack->setCurrentIndex(2); setActiveButton(btnDijkstra); }
void MainWindow::showFilePage() { stack->setCurrentIndex(3); setActiveButton(btnFile); }

// ==========================================
// OPEN HASHING
// ==========================================

QWidget* MainWindow::buildOpenPage() {
    QWidget* page = new QWidget;
    QVBoxLayout* layout = new QVBoxLayout(page);
    layout->setContentsMargins(50, 50, 50, 50);

    layout->addWidget(buildHeader("Open Hashing", "Add and remove keys in real-time using Separate Chaining."));

    QWidget* controls = new QWidget;
    QHBoxLayout* cLayout = new QHBoxLayout(controls);
    cLayout->setContentsMargins(0,0,0,0);
    cLayout->setSpacing(15);

    openKeyInput = new QLineEdit;
    openKeyInput->setPlaceholderText("Key (integer, unique ID)");
    openKeyInput->setToolTip("Key = unique integer used for hashing & lookup.\n"
                             "Inserting an existing key updates its value.");

    openValueInput = new QLineEdit;
    openValueInput->setPlaceholderText("Value (string payload)");
    openValueInput->setToolTip("Value = string stored with the key.\n"
                               "Returned by get() / contains() lookups.");

    QPushButton* addBtn = new QPushButton("Insert"); addBtn->setObjectName("ActionBtn");
    QPushButton* remBtn = new QPushButton("Remove"); remBtn->setObjectName("DangerBtn");

    connect(addBtn, &QPushButton::clicked, this, &MainWindow::addOpenItem);
    connect(remBtn, &QPushButton::clicked, this, &MainWindow::removeOpenItem);

    cLayout->addWidget(openKeyInput, 1);
    cLayout->addWidget(openValueInput, 2);
    cLayout->addWidget(addBtn);
    cLayout->addWidget(remBtn);

    openOutput = new QTextEdit; openOutput->setReadOnly(true);
    openOutput->setPlainText(openSet->display());

    QWidget* content = new QWidget;
    QVBoxLayout* contLayout = new QVBoxLayout(content);
    contLayout->setContentsMargins(0,0,0,0);
    contLayout->addWidget(controls);
    contLayout->addSpacing(25);
    contLayout->addWidget(openOutput);

    layout->addWidget(createCard(content));

    QScrollArea* scrollArea = new QScrollArea;
    scrollArea->setWidgetResizable(true);
    scrollArea->setWidget(page);
    scrollArea->setStyleSheet("QScrollArea { border: none; background: transparent; }");
    return scrollArea;
}

void MainWindow::addOpenItem() {
    bool ok;
    int key = openKeyInput->text().toInt(&ok);
    if (!ok) { QMessageBox::warning(this, "Error", "Key must be a valid integer."); return; }
    std::string val = openValueInput->text().toStdString();
    openSet->insert(key, val);
    openOutput->setPlainText(openSet->display());
    openKeyInput->clear(); openValueInput->clear();
}

void MainWindow::removeOpenItem() {
    bool ok;
    int key = openKeyInput->text().toInt(&ok);
    if (!ok) { QMessageBox::warning(this, "Error", "Key must be a valid integer."); return; }
    openSet->remove(key);
    openOutput->setPlainText(openSet->display());
    openKeyInput->clear();
}

// ==========================================
// CLOSED HASHING
// ==========================================

QWidget* MainWindow::buildClosedPage() {
    QWidget* page = new QWidget;
    QVBoxLayout* layout = new QVBoxLayout(page);
    layout->setContentsMargins(50, 50, 50, 50);

    layout->addWidget(buildHeader("Closed Hashing", "Linear Probing with automatic rehashing at 0.5 load factor."));

    QWidget* controls = new QWidget;
    QHBoxLayout* cLayout = new QHBoxLayout(controls);
    cLayout->setContentsMargins(0,0,0,0);
    cLayout->setSpacing(15);

    closedKeyInput = new QLineEdit; closedKeyInput->setPlaceholderText("Key (Integer)");
    closedValueInput = new QLineEdit; closedValueInput->setPlaceholderText("Value (String)");

    QPushButton* addBtn = new QPushButton("Insert"); addBtn->setObjectName("ActionBtn");
    QPushButton* remBtn = new QPushButton("Remove"); remBtn->setObjectName("DangerBtn");

    connect(addBtn, &QPushButton::clicked, this, &MainWindow::addClosedItem);
    connect(remBtn, &QPushButton::clicked, this, &MainWindow::removeClosedItem);

    cLayout->addWidget(closedKeyInput, 1);
    cLayout->addWidget(closedValueInput, 2);
    cLayout->addWidget(addBtn);
    cLayout->addWidget(remBtn);

    closedOutput = new QTextEdit; closedOutput->setReadOnly(true);
    closedOutput->setPlainText(closedSet->display());

    QWidget* content = new QWidget;
    QVBoxLayout* contLayout = new QVBoxLayout(content);
    contLayout->setContentsMargins(0,0,0,0);
    contLayout->addWidget(controls);
    contLayout->addSpacing(25);
    contLayout->addWidget(closedOutput);

    layout->addWidget(createCard(content));

    QScrollArea* scrollArea = new QScrollArea;
    scrollArea->setWidgetResizable(true);
    scrollArea->setWidget(page);
    scrollArea->setStyleSheet("QScrollArea { border: none; background: transparent; }");
    return scrollArea;
}

void MainWindow::addClosedItem() {
    bool ok;
    int key = closedKeyInput->text().toInt(&ok);
    if (!ok) { QMessageBox::warning(this, "Error", "Key must be a valid integer."); return; }
    std::string val = closedValueInput->text().toStdString();
    closedSet->insert(key, val);
    closedOutput->setPlainText(closedSet->display());
    closedKeyInput->clear(); closedValueInput->clear();
}

void MainWindow::removeClosedItem() {
    bool ok;
    int key = closedKeyInput->text().toInt(&ok);
    if (!ok) { QMessageBox::warning(this, "Error", "Key must be a valid integer."); return; }
    closedSet->remove(key);
    closedOutput->setPlainText(closedSet->display());
    closedKeyInput->clear();
}

// ==========================================
// DIJKSTRA (VISUAL MAP)
// ==========================================

QWidget* MainWindow::buildDijkstraPage() {
    QWidget* page = new QWidget;
    QVBoxLayout* layout = new QVBoxLayout(page);
    layout->setContentsMargins(50, 50, 50, 50);
    layout->setSpacing(25);

    layout->addWidget(buildHeader("Dijkstra Router", "Calculate the shortest distance between Maseru and TY using the visual graph."));

    QHBoxLayout* topLayout = new QHBoxLayout;
    topLayout->setSpacing(25);

    // Left Card: Visual Map
    QWidget* mapWidget = new QWidget;
    QVBoxLayout* mapLayout = new QVBoxLayout(mapWidget);
    mapLayout->setContentsMargins(0,0,0,0);

    QLabel* mapTitle = new QLabel("THE MAP");
    mapTitle->setObjectName("SubText");

    QGraphicsView* mapView = new QGraphicsView;
    mapView->setRenderHint(QPainter::Antialiasing);
    mapView->setStyleSheet("background-color: #ffffff; border: none; border-radius: 8px;");
    mapView->setFixedHeight(250);
    mapScene = new QGraphicsScene(this);
    mapView->setScene(mapScene);

    drawMap("", ""); // Initial draw
    mapLayout->addWidget(mapTitle);
    mapLayout->addWidget(mapView);

    // Right Card: Calculator
    QWidget* calcWidget = new QWidget;
    QVBoxLayout* calcLayout = new QVBoxLayout(calcWidget);
    calcLayout->setContentsMargins(0,0,0,0);

    QLabel* startLabel = new QLabel("STARTING POINT");
    startLabel->setObjectName("SubText");
    startDropdown = new QComboBox;
    startDropdown->addItems({"Maseru (A)", "Mohale (B)", "Leribe (C)", "TY (D)"});

    QLabel* destLabel = new QLabel("DESTINATION");
    destLabel->setObjectName("SubText");
    targetDropdown = new QComboBox;
    targetDropdown->addItems({"TY (D)", "Mohale (B)", "Leribe (C)", "Maseru (A)"});

    QPushButton* calcBtn = new QPushButton("Calculate Route");
    calcBtn->setObjectName("ActionBtn");
    calcBtn->setCursor(Qt::PointingHandCursor);
    calcBtn->setFixedHeight(52);
    connect(calcBtn, &QPushButton::clicked, this, &MainWindow::calculateRoute);

    calcLayout->addWidget(startLabel);
    calcLayout->addWidget(startDropdown);
    calcLayout->addSpacing(15);
    calcLayout->addWidget(destLabel);
    calcLayout->addWidget(targetDropdown);
    calcLayout->addSpacing(25);
    calcLayout->addWidget(calcBtn);
    calcLayout->addStretch();

    topLayout->addWidget(createCard(mapWidget), 1);
    topLayout->addWidget(createCard(calcWidget), 1);
    layout->addLayout(topLayout);

    // Bottom Card: Result
    QWidget* resultWidget = new QWidget;
    QVBoxLayout* resLayout = new QVBoxLayout(resultWidget);
    resLayout->setContentsMargins(0,0,0,0);

    resultDistanceLabel = new QLabel("-- km");
    resultDistanceLabel->setObjectName("ResultLabel");

    QHBoxLayout* distRow = new QHBoxLayout;
    QLabel* mapIcon = new QLabel();
    mapIcon->setPixmap(QPixmap(":/icons/map.png").scaled(40, 40, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    distRow->addWidget(mapIcon);
    distRow->addSpacing(15);
    distRow->addWidget(resultDistanceLabel);
    distRow->addStretch();

    QFrame* line = new QFrame;
    line->setFrameShape(QFrame::HLine);
    line->setStyleSheet("color: #e4e7eb; margin: 15px 0;");

    QVBoxLayout* routeLayout = new QVBoxLayout;
    QLabel* routeLbl = new QLabel("SHORTEST PATH");
    routeLbl->setObjectName("SubText");
    resultPathLabel = new QLabel("N/A");
    resultPathLabel->setStyleSheet("font-weight: bold; font-size: 20px; margin-top: 5px;");
    routeLayout->addWidget(routeLbl);
    routeLayout->addWidget(resultPathLabel);

    resLayout->addLayout(distRow);
    resLayout->addWidget(line);
    resLayout->addLayout(routeLayout);

    layout->addWidget(createCard(resultWidget));

    QScrollArea* scrollArea = new QScrollArea;
    scrollArea->setWidgetResizable(true);
    scrollArea->setWidget(page);
    scrollArea->setStyleSheet("QScrollArea { border: none; background: transparent; }");
    return scrollArea;
}

void MainWindow::drawMap(const QString& highlightStart, const QString& highlightTarget) {
    mapScene->clear();

    // Coordinates for cities (A=TopLeft, B=TopRight, C=BottomLeft, D=BottomRight)
    QPointF pA(50, 50);   // Maseru
    QPointF pB(250, 50);  // Mohale
    QPointF pC(50, 200);  // Leribe
    QPointF pD(250, 200); // TY

    // Draw Lines (Roads)
    auto drawEdge = [&](QPointF p1, QPointF p2, int weight, QString from, QString to) {
        QPen pen(Qt::gray, 3, Qt::DashLine);
        if ((highlightStart == from && highlightTarget == to) || (highlightStart == to && highlightTarget == from)) {
            pen.setColor(Qt::black);
            pen.setStyle(Qt::SolidLine);
            pen.setWidth(4);
        }
        mapScene->addLine(p1.x(), p1.y(), p2.x(), p2.y(), pen);

        // Draw distance text
        QGraphicsTextItem* text = mapScene->addText(QString::number(weight) + " km");
        text->setDefaultTextColor(Qt::darkGray);
        text->setPos((p1.x() + p2.x())/2 - 15, (p1.y() + p2.y())/2 - 10);
    };

    drawEdge(pA, pB, 3, "A", "B");
    drawEdge(pA, pC, 1, "A", "C");
    drawEdge(pB, pD, 2, "B", "D");
    drawEdge(pC, pD, 4, "C", "D");

    // Draw Nodes (Cities)
    auto drawNode = [&](QPointF p, QString label, QString id) {
        QBrush brush(Qt::white);
        QPen pen(Qt::black, 2);
        if (id == highlightStart || id == highlightTarget) {
            brush.setColor(Qt::black);
            pen.setColor(Qt::black);
        }
        mapScene->addEllipse(p.x() - 20, p.y() - 20, 40, 40, pen, brush);

        QGraphicsTextItem* text = mapScene->addText(label);
        text->setDefaultTextColor(id == highlightStart || id == highlightTarget ? Qt::white : Qt::black);
        QFont font = text->font();
        font.setBold(true);
        font.setPointSize(10);
        text->setFont(font);
        text->setPos(p.x() - 10, p.y() - 10);
    };

    drawNode(pA, "A", "A");
    drawNode(pB, "B", "B");
    drawNode(pC, "C", "C");
    drawNode(pD, "D", "D");
}

void MainWindow::calculateRoute() {
    std::unordered_map<std::string, std::vector<Edge>> graph;
    graph["A"] = { Edge("B", 3), Edge("C", 1) };
    graph["B"] = { Edge("D", 2) };
    graph["C"] = { Edge("D", 4) };
    graph["D"] = {};

    QString startTxt = startDropdown->currentText();
    QString targetTxt = targetDropdown->currentText();

    std::string startNode = startTxt.contains("Maseru") ? "A" : startTxt.contains("Mohale") ? "B" : startTxt.contains("Leribe") ? "C" : "D";
    std::string targetNode = targetTxt.contains("Maseru") ? "A" : targetTxt.contains("Mohale") ? "B" : targetTxt.contains("Leribe") ? "C" : "D";

    if (startNode == targetNode) {
        resultDistanceLabel->setText("0 km");
        resultPathLabel->setText(startTxt);
        drawMap(QString::fromStdString(startNode), QString::fromStdString(targetNode));
        return;
    }

    DijkstraResult res = runDijkstra(graph, startNode);

    if (!res.valid) {
        resultDistanceLabel->setText("Error");
        resultPathLabel->setText(res.errorMessage);
        return;
    }

    int d = res.distances.at(targetNode);
    if (d == INT_MAX) {
        resultDistanceLabel->setText("Unreachable");
        resultPathLabel->setText("No valid route.");
        drawMap("", "");
        return;
    }

    resultDistanceLabel->setText(QString("%1 km").arg(d));

    auto pathList = reconstructPath(res.previous, startNode, targetNode);
    QString pathStr = printPath(pathList);
    resultPathLabel->setText(pathStr);

    // Highlight the path on the map
    drawMap(QString::fromStdString(startNode), QString::fromStdString(targetNode));
}

// ==========================================
// FILE LOADING
// ==========================================

QWidget* MainWindow::buildFilePage() {
    QWidget* page = new QWidget;
    QVBoxLayout* layout = new QVBoxLayout(page);
    layout->setContentsMargins(50, 50, 50, 50);

    layout->addWidget(buildHeader("Load Custom Graph", "Upload a .txt file to compute routes on a custom graph."));

    QWidget* content = new QWidget;
    QVBoxLayout* cLayout = new QVBoxLayout(content);
    cLayout->setContentsMargins(0,0,0,0);

    QHBoxLayout* fileRow = new QHBoxLayout;
    filePathEdit = new QLineEdit; filePathEdit->setPlaceholderText("Select graph.txt...");
    QPushButton* browseBtn = new QPushButton("Browse");
    browseBtn->setObjectName("ActionBtn");
    browseBtn->setCursor(Qt::PointingHandCursor);
    connect(browseBtn, &QPushButton::clicked, this, &MainWindow::browseFile);
    fileRow->addWidget(filePathEdit); fileRow->addWidget(browseBtn);

    QPushButton* runBtn = new QPushButton("Compute Shortest Paths");
    runBtn->setObjectName("ActionBtn");
    runBtn->setCursor(Qt::PointingHandCursor);
    runBtn->setFixedHeight(45);
    connect(runBtn, &QPushButton::clicked, this, &MainWindow::runDijkstraFile);

    fileOutput = new QTextEdit; fileOutput->setReadOnly(true);

    cLayout->addLayout(fileRow);
    cLayout->addSpacing(15);
    cLayout->addWidget(runBtn);
    cLayout->addSpacing(20);
    cLayout->addWidget(fileOutput);

    layout->addWidget(createCard(content));

    QScrollArea* scrollArea = new QScrollArea;
    scrollArea->setWidgetResizable(true);
    scrollArea->setWidget(page);
    scrollArea->setStyleSheet("QScrollArea { border: none; background: transparent; }");
    return scrollArea;
}

void MainWindow::browseFile() {
    QString file = QFileDialog::getOpenFileName(this, "Select Graph File", "", "Text Files (*.txt)");
    if (!file.isEmpty()) filePathEdit->setText(file);
}

void MainWindow::runDijkstraFile() {
    QFile file(filePathEdit->text());
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        fileOutput->setPlainText("Could not open file.");
        return;
    }
    QTextStream in(&file);
    std::unordered_map<std::string, std::vector<Edge>> graph;
    std::string source = in.readLine().trimmed().toStdString();
    int edges = in.readLine().toInt();

    for (int i = 0; i < edges; i++) {
        QString line = in.readLine();
        QStringList parts = line.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);
        if (parts.size() == 3) {
            graph[parts[0].toStdString()].push_back(Edge(parts[1].toStdString(), parts[2].toInt()));
            if (graph.find(parts[1].toStdString()) == graph.end()) graph[parts[1].toStdString()] = {};
        }
    }
    if (graph.find(source) == graph.end()) graph[source] = {};

    DijkstraResult res = runDijkstra(graph, source);
    QString log = QString("Source: %1\n\n").arg(QString::fromStdString(source));

    if (!res.valid) { fileOutput->setPlainText(log + res.errorMessage); return; }

    for (const auto& p : res.distances) {
        log += QString("%1: %2\n").arg(QString::fromStdString(p.first))
        .arg(p.second == INT_MAX ? "unreachable" : QString::number(p.second));
    }
    fileOutput->setPlainText(log);
}