#include "mainwindow.h"
#include <QPixmap>
#include <QGraphicsDropShadowEffect>
#include <QPen>
#include <QBrush>
#include <QFont>
#include <QScrollArea>
#include <algorithm>
#include <vector>

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
        "QWidget#Sidebar QLabel { background: transparent; }"
        "QWidget#Sidebar QPushButton { background: transparent; }"
        "QLabel#SidebarTitle { color: #ffffff; background: transparent; font-size: 22px; font-weight: 800; letter-spacing: 1px; }"
        "QLabel#SidebarText  { color: #9aa0a6; background: transparent; font-size: 13px; }"
        "QPushButton#MenuBtn { background: transparent; color: #9aa0a6; border: none; text-align: left; padding: 14px 20px; font-size: 15px; font-weight: 600; border-radius: 8px; }"
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

    showOpenPage();
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

    openOutput = new QTextEdit;
    openOutput->setReadOnly(true);
    openOutput->setAcceptRichText(true);
    openOutput->setHtml(openSet->displayAsTable());

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
    openOutput->setHtml(openSet->displayAsTable());
    openKeyInput->clear(); openValueInput->clear();
}

void MainWindow::removeOpenItem() {
    bool ok;
    int key = openKeyInput->text().toInt(&ok);
    if (!ok) { QMessageBox::warning(this, "Error", "Key must be a valid integer."); return; }
    openSet->remove(key);
    openOutput->setHtml(openSet->displayAsTable());
    openKeyInput->clear();
}

// ==========================================
// CLOSED HASHING  (Linear Probing)
// ==========================================
//
// KEY   = a unique integer. It is hashed with  key % capacity  to pick
//         the STARTING slot. You can insert the same key again to
//         overwrite its value, but you cannot have two entries with the
//         same key.
//
// VALUE = the text payload stored alongside the key. Many keys may hold
//         the same value; values are what get()/contains() return.
//
// LINEAR PROBING — when a key's hash slot is already taken by a DIFFERENT
// key, we step forward one slot at a time (with wrap-around) until we
// find a free slot (empty or a reusable tombstone). Lookups follow the
// exact same probe sequence, and stop as soon as they hit a truly empty
// slot (which proves the key was never inserted) or find the key.
// ---------------------------------------------------------------

QWidget* MainWindow::buildClosedPage() {
    QWidget* page = new QWidget;
    QVBoxLayout* layout = new QVBoxLayout(page);
    layout->setContentsMargins(50, 50, 50, 50);

    layout->addWidget(buildHeader(
        "Closed Hashing",
        "Linear probing with automatic rehashing at 0.5 load factor."));

    // ---------- Controls row 1: Key + Value + Insert + Remove ----------
    QWidget* controls = new QWidget;
    QHBoxLayout* cLayout = new QHBoxLayout(controls);
    cLayout->setContentsMargins(0, 0, 0, 0);
    cLayout->setSpacing(15);

    closedKeyInput = new QLineEdit;
    closedKeyInput->setPlaceholderText("Key  =  integer ID (e.g. 42)");
    closedKeyInput->setToolTip(
        "KEY: a unique integer.\n"
        "It is hashed with  key % capacity  to pick the starting slot.\n"
        "You cannot change a key once inserted; re-inserting a key just\n"
        "overwrites its VALUE.");

    closedValueInput = new QLineEdit;
    closedValueInput->setPlaceholderText("Value  =  text payload (e.g. \"Alice\")");
    closedValueInput->setToolTip(
        "VALUE: any text stored alongside the key.\n"
        "Lookups (get / contains) use the KEY; the VALUE is what you\n"
        "retrieve. Values may repeat; keys may not.");

    QPushButton* addBtn = new QPushButton("Insert");
    addBtn->setObjectName("ActionBtn");
    addBtn->setToolTip("Adds the (key, value) pair, or updates the value if the key already exists.");

    QPushButton* remBtn = new QPushButton("Remove");
    remBtn->setObjectName("DangerBtn");
    remBtn->setToolTip("Removes the entry for this key. The slot becomes a tombstone\n"
                       "so that probing for other keys still works.");

    connect(addBtn, &QPushButton::clicked, this, &MainWindow::addClosedItem);
    connect(remBtn, &QPushButton::clicked, this, &MainWindow::removeClosedItem);

    cLayout->addWidget(closedKeyInput, 1);
    cLayout->addWidget(closedValueInput, 2);
    cLayout->addWidget(addBtn);
    cLayout->addWidget(remBtn);

    // ---------- Controls row 2: Get / Contains / Clear ----------
    QWidget* controls2 = new QWidget;
    QHBoxLayout* c2Layout = new QHBoxLayout(controls2);
    c2Layout->setContentsMargins(0, 0, 0, 0);
    c2Layout->setSpacing(15);

    QPushButton* getBtn = new QPushButton("Get Value");
    getBtn->setObjectName("ActionBtn");
    getBtn->setToolTip("Look up the VALUE for the key in the Key field.");
    connect(getBtn, &QPushButton::clicked, this, &MainWindow::doClosedGet);

    QPushButton* containsBtn = new QPushButton("Contains Key?");
    containsBtn->setObjectName("ActionBtn");
    containsBtn->setToolTip("Returns true if the key exists, false otherwise.");
    connect(containsBtn, &QPushButton::clicked, this, &MainWindow::doClosedContains);

    QPushButton* clearBtn = new QPushButton("Clear Table");
    clearBtn->setObjectName("DangerBtn");
    clearBtn->setToolTip("Removes every entry and resets the table.");
    connect(clearBtn, &QPushButton::clicked, this, &MainWindow::clearClosedSet);

    c2Layout->addWidget(getBtn);
    c2Layout->addWidget(containsBtn);
    c2Layout->addStretch();
    c2Layout->addWidget(clearBtn);

    // ---------- Status line ----------
    closedStatusLabel = new QLabel("Ready. Load factor target: 0.5. Capacity grows by rehashing.");
    closedStatusLabel->setStyleSheet(
        "font-family: Consolas, monospace; font-size: 13px; color: #1a1a1a;"
        "background: #f1f3f4; border-radius: 6px; padding: 10px;");

    // ---------- The table itself ----------
    closedOutput = new QTextEdit;
    closedOutput->setReadOnly(true);
    closedOutput->setAcceptRichText(true);
    closedOutput->setHtml(closedSet->displayAsTable());

    QWidget* content = new QWidget;
    QVBoxLayout* contLayout = new QVBoxLayout(content);
    contLayout->setContentsMargins(0, 0, 0, 0);
    contLayout->addWidget(controls);
    contLayout->addWidget(controls2);
    contLayout->addSpacing(15);
    contLayout->addWidget(closedStatusLabel);
    contLayout->addSpacing(15);
    contLayout->addWidget(closedOutput);

    layout->addWidget(createCard(content));

    QScrollArea* scrollArea = new QScrollArea;
    scrollArea->setWidgetResizable(true);
    scrollArea->setWidget(page);
    scrollArea->setStyleSheet("QScrollArea { border: none; background: transparent; }");
    return scrollArea;
}

void MainWindow::showClosedStatus(const QString& msg) {
    closedStatusLabel->setText(msg);
}

void MainWindow::addClosedItem() {
    bool ok;
    int key = closedKeyInput->text().toInt(&ok);
    if (!ok) {
        QMessageBox::warning(this, "Error", "Key must be a valid integer.");
        return;
    }
    std::string val = closedValueInput->text().toStdString();

    int beforeCap   = closedSet->getCapacity();
    int beforeCount = closedSet->getCount();

    closedSet->insert(key, val);

    int afterCap   = closedSet->getCapacity();
    int afterCount = closedSet->getCount();

    QString msg;
    if (afterCap != beforeCap) {
        msg = QString("Inserted (%1 -> \"%2\"). Load factor threshold reached — REHASHED from %3 to %4 slots.")
                  .arg(key).arg(QString::fromStdString(val))
                  .arg(beforeCap).arg(afterCap);
    } else if (afterCount == beforeCount) {
        msg = QString("Key %1 already existed — VALUE updated to \"%2\".")
                  .arg(key).arg(QString::fromStdString(val));
    } else {
        msg = QString("Inserted key %1 with value \"%2\". "
                      "Check the 'Probed?' column — a blue row means linear probing was used to resolve a collision.")
                  .arg(key).arg(QString::fromStdString(val));
    }

    closedOutput->setHtml(closedSet->displayAsTable());
    showClosedStatus(msg);
    closedKeyInput->clear();
    closedValueInput->clear();
}

void MainWindow::removeClosedItem() {
    bool ok;
    int key = closedKeyInput->text().toInt(&ok);
    if (!ok) {
        QMessageBox::warning(this, "Error", "Key must be a valid integer.");
        return;
    }
    bool removed = closedSet->remove(key);
    closedOutput->setHtml(closedSet->displayAsTable());
    showClosedStatus(removed
                         ? QString("Removed key %1. Slot is now a TOMBSTONE — lookups skip over it while probing.")
                               .arg(key)
                         : QString("Key %1 was not in the table.").arg(key));
    closedKeyInput->clear();
}

void MainWindow::doClosedGet() {
    bool ok;
    int key = closedKeyInput->text().toInt(&ok);
    if (!ok) {
        QMessageBox::warning(this, "Error", "Key must be a valid integer.");
        return;
    }
    std::string out;
    if (closedSet->get(key, out)) {
        showClosedStatus(QString("get(%1) → \"%2\"").arg(key).arg(QString::fromStdString(out)));
    } else {
        showClosedStatus(QString("get(%1) → (not found)").arg(key));
    }
}

void MainWindow::doClosedContains() {
    bool ok;
    int key = closedKeyInput->text().toInt(&ok);
    if (!ok) {
        QMessageBox::warning(this, "Error", "Key must be a valid integer.");
        return;
    }
    bool present = closedSet->contains(key);
    showClosedStatus(QString("contains(%1) → %2")
                         .arg(key)
                         .arg(present ? "true" : "false"));
}

void MainWindow::clearClosedSet() {
    if (closedSet->isEmpty()) {
        showClosedStatus("Table is already empty.");
        return;
    }
    auto reply = QMessageBox::question(this, "Clear Table",
                                       "Remove all entries? This cannot be undone.",
                                       QMessageBox::Yes | QMessageBox::No);
    if (reply != QMessageBox::Yes) return;

    closedSet->clear();
    closedOutput->setHtml(closedSet->displayAsTable());
    showClosedStatus("Table cleared. All slots are empty.");
}

// ==========================================
// DIJKSTRA (VISUAL MAP)
// ==========================================

QWidget* MainWindow::buildDijkstraPage() {
    QWidget* page = new QWidget;
    QVBoxLayout* layout = new QVBoxLayout(page);
    layout->setContentsMargins(50, 50, 50, 50);
    layout->setSpacing(25);

    layout->addWidget(buildHeader(
        "Dijkstra Router",
        "Shortest path from Maseru (A) to any town using Dijkstra's algorithm. "
        "Negative edges are rejected; unreachable vertices are shown as ∞."));

    QHBoxLayout* topLayout = new QHBoxLayout;
    topLayout->setSpacing(25);

    // ----- Left card: the map -----
    QWidget* mapWidget = new QWidget;
    QVBoxLayout* mapLayout = new QVBoxLayout(mapWidget);
    mapLayout->setContentsMargins(0, 0, 0, 0);

    QLabel* mapTitle = new QLabel("THE MAP");
    mapTitle->setObjectName("SubText");

    QGraphicsView* mapView = new QGraphicsView;
    mapView->setRenderHint(QPainter::Antialiasing);
    mapView->setStyleSheet("background-color: #ffffff; border: none; border-radius: 8px;");
    mapView->setFixedHeight(250);
    mapScene = new QGraphicsScene(this);
    mapView->setScene(mapScene);

    drawMap(QStringList{});
    mapLayout->addWidget(mapTitle);
    mapLayout->addWidget(mapView);

    // ----- Right card: calculator -----
    QWidget* calcWidget = new QWidget;
    QVBoxLayout* calcLayout = new QVBoxLayout(calcWidget);
    calcLayout->setContentsMargins(0, 0, 0, 0);

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

    // ----- Bottom card: results -----
    QWidget* resultWidget = new QWidget;
    QVBoxLayout* resLayout = new QVBoxLayout(resultWidget);
    resLayout->setContentsMargins(0, 0, 0, 0);

    resultDistanceLabel = new QLabel("-- km");
    resultDistanceLabel->setObjectName("ResultLabel");

    QHBoxLayout* distRow = new QHBoxLayout;
    QLabel* mapIcon = new QLabel();
    mapIcon->setPixmap(QPixmap(":/icons/map.png").scaled(40, 40,
                                                         Qt::KeepAspectRatio,
                                                         Qt::SmoothTransformation));
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
    resultPathLabel->setWordWrap(true);
    routeLayout->addWidget(routeLbl);
    routeLayout->addWidget(resultPathLabel);

    resLayout->addLayout(distRow);
    resLayout->addWidget(line);
    resLayout->addLayout(routeLayout);

    // ---- Distance-to-every-town table ----
    QFrame* line2 = new QFrame;
    line2->setFrameShape(QFrame::HLine);
    line2->setStyleSheet("color: #e4e7eb; margin: 15px 0;");

    QLabel* distLbl = new QLabel("DISTANCE TO EVERY TOWN (from selected start)");
    distLbl->setObjectName("SubText");

    resultDistancesTable = new QTextEdit;
    resultDistancesTable->setReadOnly(true);
    resultDistancesTable->setAcceptRichText(true);
    resultDistancesTable->setMinimumHeight(140);
    resultDistancesTable->setHtml(
        "<span style='color:#5f6368;'>Click “Calculate Route” to populate.</span>");

    resLayout->addWidget(line2);
    resLayout->addWidget(distLbl);
    resLayout->addWidget(resultDistancesTable);

    layout->addWidget(createCard(resultWidget));

    QScrollArea* scrollArea = new QScrollArea;
    scrollArea->setWidgetResizable(true);
    scrollArea->setWidget(page);
    scrollArea->setStyleSheet("QScrollArea { border: none; background: transparent; }");
    return scrollArea;
}

void MainWindow::drawMap(const QStringList& pathVertices) {
    mapScene->clear();

    const QPointF pA(50,  50);    // Maseru
    const QPointF pB(250, 50);    // Mohale
    const QPointF pC(50,  200);   // Leribe
    const QPointF pD(250, 200);   // TY

    auto onPath = [&](const QString& a, const QString& b) {
        for (int i = 0; i + 1 < pathVertices.size(); ++i) {
            if ((pathVertices[i] == a && pathVertices[i+1] == b) ||
                (pathVertices[i] == b && pathVertices[i+1] == a)) return true;
        }
        return false;
    };

    auto drawEdge = [&](QPointF p1, QPointF p2, int weight,
                        const QString& from, const QString& to) {
        bool hot = onPath(from, to);
        QPen pen(hot ? Qt::black : Qt::gray,
                 hot ? 4 : 3,
                 hot ? Qt::SolidLine : Qt::DashLine);
        mapScene->addLine(p1.x(), p1.y(), p2.x(), p2.y(), pen);

        QGraphicsTextItem* t = mapScene->addText(QString::number(weight) + " km");
        t->setDefaultTextColor(hot ? Qt::black : Qt::darkGray);
        QFont f = t->font();
        f.setBold(hot);
        t->setFont(f);
        t->setPos((p1.x() + p2.x())/2 - 15, (p1.y() + p2.y())/2 - 10);
    };

    drawEdge(pA, pB, 3, "A", "B");
    drawEdge(pA, pC, 1, "A", "C");
    drawEdge(pB, pD, 2, "B", "D");
    drawEdge(pC, pD, 4, "C", "D");

    auto drawNode = [&](QPointF p, const QString& label, const QString& id) {
        bool hot = pathVertices.contains(id);
        QBrush brush(hot ? Qt::black : Qt::white);
        QPen   pen  (Qt::black, 2);
        mapScene->addEllipse(p.x() - 20, p.y() - 20, 40, 40, pen, brush);

        QGraphicsTextItem* t = mapScene->addText(label);
        t->setDefaultTextColor(hot ? Qt::white : Qt::black);
        QFont f = t->font();
        f.setBold(true);
        f.setPointSize(10);
        t->setFont(f);
        t->setPos(p.x() - 10, p.y() - 10);
    };

    drawNode(pA, "A", "A");
    drawNode(pB, "B", "B");
    drawNode(pC, "C", "C");
    drawNode(pD, "D", "D");
}

QString MainWindow::formatDistanceTable(const std::unordered_map<std::string, int>& distances,
                                        const std::string& startNode,
                                        const std::string& targetNode) const {
    auto pretty = [](const std::string& id) -> QString {
        if (id == "A") return "Maseru (A)";
        if (id == "B") return "Mohale (B)";
        if (id == "C") return "Leribe (C)";
        if (id == "D") return "TY (D)";
        return QString::fromStdString(id);
    };

    QString html;
    html += "<table cellspacing='0' cellpadding='6' border='1' "
            "style='border-collapse:collapse; font-family:Consolas,monospace; "
            "font-size:13px; width:100%;'>"
            "<tr style='background:#1a1a1a; color:#ffffff;'>"
            "<th>Vertex</th><th>Distance from start</th><th>Status</th>"
            "</tr>";

    const std::vector<std::string> order = {"A", "B", "C", "D"};
    for (const auto& id : order) {
        auto it = distances.find(id);
        if (it == distances.end()) continue;

        QString bg = "#ffffff";
        if (id == startNode)       bg = "#fff4e5";
        else if (id == targetNode) bg = "#e8f5e9";
        else if (it->second == INT_MAX) bg = "#fafafa";

        QString distTxt = (it->second == INT_MAX)
                              ? QString("∞ (unreachable)")
                              : QString("%1 km").arg(it->second);

        QString status = (id == startNode)  ? "start"
                         : (id == targetNode) ? "target"
                         : (it->second == INT_MAX) ? "unreachable"
                                                   : "reachable";

        html += QString("<tr style='background:%1;'>"
                        "<td>%2</td>"
                        "<td align='center'>%3</td>"
                        "<td align='center'>%4</td>"
                        "</tr>")
                    .arg(bg, pretty(id), distTxt, status);
    }

    html += "</table>";
    return html;
}

void MainWindow::calculateRoute() {
    // ---- Build the assignment's weighted directed graph ----
    std::unordered_map<std::string, std::vector<Edge>> graph;
    graph["A"] = { Edge("B", 3), Edge("C", 1) };
    graph["B"] = { Edge("D", 2) };
    graph["C"] = { Edge("D", 4) };
    graph["D"] = {};

    // Map the human-readable combo text to node IDs.
    auto textToId = [](const QString& t) -> std::string {
        if (t.contains("Maseru"))  return "A";
        if (t.contains("Mohale"))  return "B";
        if (t.contains("Leribe"))  return "C";
        return "D";                                  // TY
    };

    const std::string startNode  = textToId(startDropdown->currentText());
    const std::string targetNode = textToId(targetDropdown->currentText());

    // ---- Source == destination shortcut ----
    if (startNode == targetNode) {
        resultDistanceLabel->setText("0 km");
        resultPathLabel->setText(startDropdown->currentText());
        drawMap(QStringList{ QString::fromStdString(startNode) });

        DijkstraResult self = runDijkstra(graph, startNode);
        if (self.valid) {
            resultDistancesTable->setHtml(
                formatDistanceTable(self.distances, startNode, targetNode));
        }
        return;
    }

    // ---- Run Dijkstra ----
    DijkstraResult res = runDijkstra(graph, startNode);

    // Negative edge or missing source
    if (!res.valid) {
        resultDistanceLabel->setText("Error");
        resultPathLabel->setText(res.errorMessage);
        resultDistancesTable->setHtml(
            QString("<span style='color:#d93025;'>%1</span>")
                .arg(res.errorMessage.toHtmlEscaped()));
        drawMap(QStringList{});
        return;
    }

    // ---- Target distance (safe lookup — never throws) ----
    auto it = res.distances.find(targetNode);
    if (it == res.distances.end() || it->second == INT_MAX) {
        resultDistanceLabel->setText("Unreachable");
        resultPathLabel->setText(
            QString("No route from %1 to %2.")
                .arg(startDropdown->currentText(), targetDropdown->currentText()));
        resultDistancesTable->setHtml(
            formatDistanceTable(res.distances, startNode, targetNode));
        drawMap(QStringList{ QString::fromStdString(startNode) });
        return;
    }

    const int distance = it->second;
    resultDistanceLabel->setText(QString("%1 km").arg(distance));

    // ---- Path ----
    auto pathList = reconstructPath(res.previous, startNode, targetNode);
    if (pathList.empty()) {
        resultPathLabel->setText("Path reconstruction failed (unexpected).");
        resultDistancesTable->setHtml(
            formatDistanceTable(res.distances, startNode, targetNode));
        drawMap(QStringList{ QString::fromStdString(startNode) });
        return;
    }

    QStringList pathIds;
    for (const auto& v : pathList) pathIds << QString::fromStdString(v);
    resultPathLabel->setText(printPath(pathList));

    // ---- Highlight the full path on the map ----
    drawMap(pathIds);

    // ---- Full distance table ----
    resultDistancesTable->setHtml(
        formatDistanceTable(res.distances, startNode, targetNode));
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

    // File format:
    //   <source>              (first line)
    //   <edge count>          (second line)
    //   <u> <v> <weight>      (one per line)
    std::string source = in.readLine().trimmed().toStdString();
    int edges = in.readLine().toInt();

    for (int i = 0; i < edges && !in.atEnd(); ++i) {
        QString line = in.readLine();
        QStringList parts = line.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);
        if (parts.size() != 3) continue;
        bool okW = false;
        int w = parts[2].toInt(&okW);
        if (!okW) continue;

        const std::string u = parts[0].toStdString();
        const std::string v = parts[1].toStdString();
        graph[u].push_back(Edge(v, w));
        if (graph.find(v) == graph.end()) graph[v] = {};
    }
    if (graph.find(source) == graph.end()) graph[source] = {};

    DijkstraResult res = runDijkstra(graph, source);

    if (!res.valid) {
        fileOutput->setPlainText(
            QString("Source: %1\n\n%2").arg(QString::fromStdString(source), res.errorMessage));
        return;
    }

    // Stable alphabetical output
    std::vector<std::string> vertices;
    vertices.reserve(res.distances.size());
    for (const auto& p : res.distances) vertices.push_back(p.first);
    std::sort(vertices.begin(), vertices.end());

    QString log = QString("Source: %1\nEdges: %2\n\n")
                      .arg(QString::fromStdString(source))
                      .arg(edges);

    for (const auto& v : vertices) {
        int d = res.distances.at(v);
        log += QString("%1: %2\n")
                   .arg(QString::fromStdString(v),
                        d == INT_MAX ? "unreachable" : QString::number(d));
    }
    fileOutput->setPlainText(log);
}