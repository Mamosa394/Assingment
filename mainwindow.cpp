#include "mainwindow.h"
#include <QPixmap>
#include <QGraphicsDropShadowEffect>
#include <QPen>
#include <QBrush>
#include <QFont>
#include <QScrollArea>
#include <algorithm>
#include <vector>
#include <cmath>

static void populateHashFnCombo(QComboBox* combo) {
    combo->addItem("Division   h(k) = k mod m    ", static_cast<int>(HashFunction::Division));
    // combo->addItem("Multiplication  h(k) = floor(m * frac(k * 1/phi))   * recommended", static_cast<int>(HashFunction::Multiplicative));
    // combo->addItem("Mid-square  h(k) = middle digits of k^2", static_cast<int>(HashFunction::MidSquare));
    // combo->addItem("Universal   h(k) = ((a*k + b) mod p) mod m", static_cast<int>(HashFunction::Universal));
    // combo->addItem("Custom universal  (enter your own a, b, p)", static_cast<int>(HashFunction::CustomUniversal));
}

static HashFunction fnFromCombo(QComboBox* c) {
    return static_cast<HashFunction>(c->currentData().toInt());
}

static QWidget* buildHashConfigPanel(
    QComboBox*& fnCombo,
    QLineEdit*& modulusEdit,
    QLineEdit*& universalA,
    QLineEdit*& universalB,
    QLineEdit*& universalP,
    QLabel*& summaryLabel,
    int defaultSize)
{
    QWidget* panel = new QWidget;
    QVBoxLayout* v = new QVBoxLayout(panel);
    v->setContentsMargins(0, 0, 0, 0);
    v->setSpacing(8);

    QLabel* title = new QLabel("HASH CONFIG - choose how keys map to buckets");
    title->setObjectName("SubText");
    v->addWidget(title);

    QHBoxLayout* row = new QHBoxLayout;
    row->setSpacing(12);

    QLabel* fnLbl = new QLabel("Hash function:");
    fnLbl->setStyleSheet("font-family: Consolas, monospace; font-size: 13px;");
    fnCombo = new QComboBox;
    populateHashFnCombo(fnCombo);
    fnCombo->setMinimumWidth(430);

    QLabel* modLbl = new QLabel("Table size (M):");
    modLbl->setStyleSheet("font-family: Consolas, monospace; font-size: 13px;");
    modulusEdit = new QLineEdit(QString::number(defaultSize));
    modulusEdit->setFixedWidth(90);
    modulusEdit->setToolTip("The exact number of buckets / slots in the array.");
    modulusEdit->setStyleSheet("font-family: Consolas, monospace; font-size: 13px;");

    row->addWidget(fnLbl);
    row->addWidget(fnCombo);
    row->addSpacing(20);
    row->addWidget(modLbl);
    row->addWidget(modulusEdit);
    row->addStretch();
    v->addLayout(row);

    QWidget* params = new QWidget;
    QHBoxLayout* pRow = new QHBoxLayout(params);
    pRow->setContentsMargins(0, 0, 0, 0);
    pRow->setSpacing(12);

    auto mkField = [](const QString& lbl, const QString& def, QLineEdit*& out) {
        QLabel* l = new QLabel(lbl);
        l->setStyleSheet("font-family: Consolas, monospace; font-size: 13px;");
        out = new QLineEdit(def);
        out->setFixedWidth(90);
        out->setStyleSheet("font-family: Consolas, monospace; font-size: 13px;");
        return std::make_pair(l, out);
    };

    auto aPair = mkField("a =", "3", universalA);
    auto bPair = mkField("b =", "5", universalB);
    auto pPair = mkField("p =", "1000003", universalP);

    pRow->addWidget(aPair.first);  pRow->addWidget(universalA);
    pRow->addSpacing(12);
    pRow->addWidget(bPair.first);  pRow->addWidget(universalB);
    pRow->addSpacing(12);
    pRow->addWidget(pPair.first);  pRow->addWidget(universalP);
    pRow->addStretch();

    v->addWidget(params);
    params->setVisible(false);

    summaryLabel = new QLabel("");
    summaryLabel->setStyleSheet("font-family: Consolas, monospace; font-size: 12px; color:#1a1a1a;"
                                "background:#f1f3f4; border-radius:6px; padding:8px;");
    v->addWidget(summaryLabel);

    QObject::connect(fnCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
                     params, [params, fnCombo](int) {
                         auto fn = static_cast<HashFunction>(fnCombo->currentData().toInt());
                         params->setVisible(fn == HashFunction::Universal ||
                                            fn == HashFunction::CustomUniversal);
                     });

    return panel;
}

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    setWindowTitle("Group Assingment");
    resize(1200, 850);

    openSet = nullptr;
    closedSet = nullptr;

    setStyleSheet(
        "QMainWindow { background-color: #ffffff; }"
        "QWidget { color: #202124; font-family: 'Segoe UI', Arial, sans-serif; background-color: #ffffff; }"
        "QWidget#Sidebar { background-color: #000000; }"
        "QWidget#Sidebar QLabel { background: transparent; }"
        "QWidget#Sidebar QPushButton { background: transparent; }"
        "QLabel#SidebarTitle { color: #ffffff; background: transparent; font-size: 22px; font-weight: 800; letter-spacing: 1px; }"
        "QLabel#SidebarText  { color: #9aa0a6; background: transparent; font-size: 13px; }"
        "QPushButton#MenuBtn { background: transparent; color: #9aa0a6; border: none; text-align: left; padding: 14px 20px; font-size: 15px; font-weight: 600; border-radius: 8px; }"
        "QPushButton#MenuBtn:hover { color: #ffffff; background-color: #2d2d2d; }"
        "QPushButton#MenuBtn[active='true'] { color: #1a1a1a; background-color: #ffffff; }"
        "QPushButton#ActionBtn { background-color: #1a1a1a; color: #ffffff; border: none; border-radius: 6px; padding: 12px 24px; font-weight: bold; font-size: 14px; min-width: 100px; }"
        "QPushButton#ActionBtn:hover { background-color: #333333; }"
        "QPushButton#DangerBtn { background-color: #d93025; color: #ffffff; border: none; border-radius: 6px; padding: 12px 24px; font-weight: bold; font-size: 14px; min-width: 100px; }"
        "QPushButton#DangerBtn:hover { background-color: #b3261e; }"
        "QLineEdit, QComboBox { background-color: #ffffff; border: 1px solid #dadce0; border-radius: 6px; padding: 12px 16px; font-size: 14px; color: #202124; }"
        "QLineEdit:focus, QComboBox:focus { border: 2px solid #1a1a1a; }"
        "QComboBox::drop-down { border: none; width: 30px; }"
        "QTextEdit { background-color: #ffffff; border: 1px solid #dadce0; border-radius: 8px; font-family: 'Consolas', monospace; font-size: 14px; color: #202124; padding: 15px; }"
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
    QLabel* desc = new QLabel("Group Assingment");
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
    addMenuBtn("", &btnFile, &MainWindow::showFilePage);

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

// Open Hashing Page
QWidget* MainWindow::buildOpenPage() {
    QWidget* page = new QWidget;
    QVBoxLayout* layout = new QVBoxLayout(page);
    layout->setContentsMargins(50, 50, 50, 50);

    layout->addWidget(buildHeader("Open Hashing", "Separate Chaining - collisions handled by a linked list per bucket."));

    QWidget* cfg = buildHashConfigPanel(
        openHashFnCombo, openModulusEdit,
        openUniversalA, openUniversalB, openUniversalP,
        openHashSummary, 7); // Default table size 7

    QPushButton* createBtn = new QPushButton("Create / Rebuild Table");
    createBtn->setObjectName("ActionBtn");
    createBtn->setCursor(Qt::PointingHandCursor);
    connect(createBtn, &QPushButton::clicked, this, &MainWindow::createOpenTable);

    QWidget* controls = new QWidget;
    QHBoxLayout* cLayout = new QHBoxLayout(controls);
    cLayout->setContentsMargins(0, 0, 0, 0);
    cLayout->setSpacing(15);

    openKeyInput = new QLineEdit;
    openKeyInput->setPlaceholderText("Key (integer, unique ID)");
    openValueInput = new QLineEdit;
    openValueInput->setPlaceholderText("Value (string payload)");

    QPushButton* addBtn = new QPushButton("Insert"); addBtn->setObjectName("ActionBtn");
    QPushButton* remBtn = new QPushButton("Remove"); remBtn->setObjectName("DangerBtn");
    connect(addBtn, &QPushButton::clicked, this, &MainWindow::addOpenItem);
    connect(remBtn, &QPushButton::clicked, this, &MainWindow::removeOpenItem);

    openCalcPreview = new QLabel("h(key) = ...  ->  Table not created yet.");
    openCalcPreview->setStyleSheet("font-family: Consolas, monospace; font-size: 13px; color: #1a1a1a; background: #f1f3f4; border-radius: 6px; padding: 10px;");

    connect(openKeyInput, &QLineEdit::textChanged, this, [this](const QString& t) {
        if (!openSet) { openCalcPreview->setText("h(key) = ...  ->  Table not created yet."); return; }
        bool ok = false;
        int k = t.toInt(&ok);
        if (!ok) { openCalcPreview->setText("h(key) = ...  ->  enter a valid integer"); return; }
        int cap = openSet->getCapacity();
        int idx = openSet->getHashStrategy().indexFor(k, cap);
        openCalcPreview->setText(QString("h(%1) = %1 %% %2 -> bucket %3").arg(k).arg(cap).arg(idx));
    });

    cLayout->addWidget(openKeyInput, 1);
    cLayout->addWidget(openValueInput, 2);
    cLayout->addWidget(addBtn);
    cLayout->addWidget(remBtn);

    openOutput = new QTextEdit;
    openOutput->setReadOnly(true);
    openOutput->setAcceptRichText(true);
    openOutput->setHtml("<span style='color:#5f6368;'>Table not created. Click Create / Rebuild Table.</span>");
    openOutput->setMinimumHeight(220);

    QLabel* logLabel = new QLabel("PROCESS LOG - what happened after each operation");
    logLabel->setObjectName("SubText");

    openProcessLog = new QTextEdit;
    openProcessLog->setReadOnly(true);
    openProcessLog->setMinimumHeight(150);
    openProcessLog->setStyleSheet("font-family: Consolas, monospace; font-size: 13px;");
    openProcessLog->setPlainText("Waiting for table to be created...\n");

    QWidget* content = new QWidget;
    QVBoxLayout* contLayout = new QVBoxLayout(content);
    contLayout->setContentsMargins(0, 0, 0, 0);
    contLayout->addWidget(cfg);
    contLayout->addWidget(createBtn, 0, Qt::AlignLeft);
    contLayout->addSpacing(12);
    contLayout->addWidget(controls);
    contLayout->addSpacing(12);
    contLayout->addWidget(openCalcPreview);
    contLayout->addSpacing(20);
    contLayout->addWidget(openOutput);
    contLayout->addSpacing(20);
    contLayout->addWidget(logLabel);
    contLayout->addWidget(openProcessLog);

    layout->addWidget(createCard(content));

    QScrollArea* scrollArea = new QScrollArea;
    scrollArea->setWidgetResizable(true);
    scrollArea->setWidget(page);
    scrollArea->setStyleSheet("QScrollArea { border: none; background: transparent; }");
    return scrollArea;
}

void MainWindow::createOpenTable() {
    int m = std::max(1, openModulusEdit->text().toInt());

    if (openSet) { delete openSet; openSet = nullptr; }
    openSet = new HashSetOpen(m);

    HashStrategy s = openSet->getHashStrategy();
    s.fn = fnFromCombo(openHashFnCombo);

    bool okA = false, okB = false, okP = false;
    int a = openUniversalA->text().toInt(&okA);
    int b = openUniversalB->text().toInt(&okB);
    int p = openUniversalP->text().toInt(&okP);
    if (okA) s.a = a;
    if (okB) s.b = b;
    if (okP && p > 0) s.p = p;
    openSet->setHashStrategy(s);

    int newCap = openSet->getCapacity();
    openOutput->setHtml(openSet->displayAsTable());
    openHashSummary->setText(QString("n = 0   M = %1   alpha = 0.00").arg(newCap));

    openProcessLog->append("------------  TABLE CREATED  ------------");
    openProcessLog->append(QString("Created exactly M = %1 buckets.").arg(newCap));
    openProcessLog->append("");
    openProcessLog->moveCursor(QTextCursor::End);
}

void MainWindow::addOpenItem() {
    if (!openSet) { QMessageBox::warning(this, "Error", "Please create the table first."); return; }

    bool ok;
    int key = openKeyInput->text().toInt(&ok);
    if (!ok) { QMessageBox::warning(this, "Error", "Key must be a valid integer."); return; }
    std::string val = openValueInput->text().toStdString();

    openSet->insert(key, val);
    QString story = openSet->explainInsert(key, val);

    openHashSummary->setText(QString("n = %1   M = %2   alpha = %3")
                                 .arg(openSet->getCount())
                                 .arg(openSet->getCapacity())
                                 .arg(QString::number(openSet->loadFactor(), 'f', 2)));

    openOutput->setHtml(openSet->displayAsTable());
    openProcessLog->append("-------------  INSERT  -------------");
    openProcessLog->append(story);
    openProcessLog->append("");
    openProcessLog->moveCursor(QTextCursor::End);

    openKeyInput->clear();
    openValueInput->clear();
}

void MainWindow::removeOpenItem() {
    if (!openSet) { QMessageBox::warning(this, "Error", "Please create the table first."); return; }

    bool ok;
    int key = openKeyInput->text().toInt(&ok);
    if (!ok) { QMessageBox::warning(this, "Error", "Key must be a valid integer."); return; }

    int cap = openSet->getCapacity();
    int bucket = openSet->getHashStrategy().indexFor(key, cap);
    bool removed = openSet->remove(key);

    openHashSummary->setText(QString("n = %1   M = %2   alpha = %3")
                                 .arg(openSet->getCount())
                                 .arg(openSet->getCapacity())
                                 .arg(QString::number(openSet->loadFactor(), 'f', 2)));

    openOutput->setHtml(openSet->displayAsTable());

    openProcessLog->append("-------------  REMOVE  -------------");
    openProcessLog->append(QString("HASH:      h(%1) = %2 (capacity %3)\nSLOT:      %4\n")
                               .arg(key).arg(bucket).arg(cap)
                               .arg(removed ? "entry removed from bucket " + QString::number(bucket) + " (chain shrinks by 1)"
                                            : "key " + QString::number(key) + " not found in bucket " + QString::number(bucket)));
    openProcessLog->append("");
    openProcessLog->moveCursor(QTextCursor::End);

    openKeyInput->clear();
}

// Closed Hashing Page
QWidget* MainWindow::buildClosedPage() {
    QWidget* page = new QWidget;
    QVBoxLayout* layout = new QVBoxLayout(page);
    layout->setContentsMargins(50, 50, 50, 50);

    layout->addWidget(buildHeader("Closed Hashing", "Linear probing - collisions resolved by stepping forward one slot at a time."));

    QWidget* cfg = buildHashConfigPanel(
        closedHashFnCombo, closedModulusEdit,
        closedUniversalA, closedUniversalB, closedUniversalP,
        closedHashSummary, 7); // Default table size 7

    QPushButton* createBtn = new QPushButton("Create / Rebuild Table");
    createBtn->setObjectName("ActionBtn");
    createBtn->setCursor(Qt::PointingHandCursor);
    connect(createBtn, &QPushButton::clicked, this, &MainWindow::createClosedTable);

    QWidget* controls = new QWidget;
    QHBoxLayout* cLayout = new QHBoxLayout(controls);
    cLayout->setContentsMargins(0, 0, 0, 0);
    cLayout->setSpacing(15);

    closedKeyInput = new QLineEdit;
    closedKeyInput->setPlaceholderText("Key  =  integer ID");
    closedValueInput = new QLineEdit;
    closedValueInput->setPlaceholderText("Value  =  text payload");

    QPushButton* addBtn = new QPushButton("Insert"); addBtn->setObjectName("ActionBtn");
    QPushButton* remBtn = new QPushButton("Remove"); remBtn->setObjectName("DangerBtn");
    connect(addBtn, &QPushButton::clicked, this, &MainWindow::addClosedItem);
    connect(remBtn, &QPushButton::clicked, this, &MainWindow::removeClosedItem);

    cLayout->addWidget(closedKeyInput, 1);
    cLayout->addWidget(closedValueInput, 2);
    cLayout->addWidget(addBtn);
    cLayout->addWidget(remBtn);

    QWidget* controls2 = new QWidget;
    QHBoxLayout* c2Layout = new QHBoxLayout(controls2);
    c2Layout->setContentsMargins(0, 0, 0, 0);
    c2Layout->setSpacing(15);

    QPushButton* getBtn = new QPushButton("Get Value"); getBtn->setObjectName("ActionBtn");
    QPushButton* containsBtn = new QPushButton("Contains Key?"); containsBtn->setObjectName("ActionBtn");
    QPushButton* clearBtn = new QPushButton("Clear Table"); clearBtn->setObjectName("DangerBtn");
    connect(getBtn,      &QPushButton::clicked, this, &MainWindow::doClosedGet);
    connect(containsBtn, &QPushButton::clicked, this, &MainWindow::doClosedContains);
    connect(clearBtn,    &QPushButton::clicked, this, &MainWindow::clearClosedSet);

    c2Layout->addWidget(getBtn);
    c2Layout->addWidget(containsBtn);
    c2Layout->addStretch();
    c2Layout->addWidget(clearBtn);

    closedCalcPreview = new QLabel("h(key) = ...  ->  Table not created yet.");
    closedCalcPreview->setStyleSheet("font-family: Consolas, monospace; font-size: 13px; color: #1a1a1a; background: #f1f3f4; border-radius: 6px; padding: 10px;");

    connect(closedKeyInput, &QLineEdit::textChanged, this, [this](const QString& t) {
        if (!closedSet) { closedCalcPreview->setText("h(key) = ...  ->  Table not created yet."); return; }
        bool ok = false;
        int k = t.toInt(&ok);
        if (!ok) { closedCalcPreview->setText("h(key) = ...  ->  enter a valid integer"); return; }
        int cap = closedSet->getCapacity();
        int idx = closedSet->getHashStrategy().indexFor(k, cap);
        closedCalcPreview->setText(QString("h(%1) = %1 %% %2 -> HOME slot %3 (probe forward on collision)").arg(k).arg(cap).arg(idx));
    });

    closedStatusLabel = new QLabel("Ready. Load factor target: 0.5. Capacity grows by rehashing.");
    closedStatusLabel->setStyleSheet("font-family: Consolas, monospace; font-size: 13px; color: #1a1a1a; background: #f1f3f4; border-radius: 6px; padding: 10px;");

    closedOutput = new QTextEdit;
    closedOutput->setReadOnly(true);
    closedOutput->setAcceptRichText(true);
    closedOutput->setHtml("<span style='color:#5f6368;'>Table not created. Click Create / Rebuild Table.</span>");
    closedOutput->setMinimumHeight(220);

    QLabel* logLabel = new QLabel("PROCESS LOG - hash + probe walk");
    logLabel->setObjectName("SubText");

    closedProcessLog = new QTextEdit;
    closedProcessLog->setReadOnly(true);
    closedProcessLog->setMinimumHeight(170);
    closedProcessLog->setStyleSheet("font-family: Consolas, monospace; font-size: 13px;");
    closedProcessLog->setPlainText("Waiting for table to be created...\n");

    QWidget* content = new QWidget;
    QVBoxLayout* contLayout = new QVBoxLayout(content);
    contLayout->setContentsMargins(0, 0, 0, 0);
    contLayout->addWidget(cfg);
    contLayout->addWidget(createBtn, 0, Qt::AlignLeft);
    contLayout->addSpacing(12);
    contLayout->addWidget(controls);
    contLayout->addWidget(controls2);
    contLayout->addSpacing(12);
    contLayout->addWidget(closedCalcPreview);
    contLayout->addSpacing(12);
    contLayout->addWidget(closedStatusLabel);
    contLayout->addSpacing(20);
    contLayout->addWidget(closedOutput);
    contLayout->addSpacing(20);
    contLayout->addWidget(logLabel);
    contLayout->addWidget(closedProcessLog);

    layout->addWidget(createCard(content));

    QScrollArea* scrollArea = new QScrollArea;
    scrollArea->setWidgetResizable(true);
    scrollArea->setWidget(page);
    scrollArea->setStyleSheet("QScrollArea { border: none; background: transparent; }");
    return scrollArea;
}

void MainWindow::createClosedTable() {
    int m = std::max(1, closedModulusEdit->text().toInt());

    if (closedSet) { delete closedSet; closedSet = nullptr; }
    closedSet = new HashSetClosed(m);

    HashStrategy s = closedSet->getHashStrategy();
    s.fn = fnFromCombo(closedHashFnCombo);

    bool okA = false, okB = false, okP = false;
    int a = closedUniversalA->text().toInt(&okA);
    int b = closedUniversalB->text().toInt(&okB);
    int p = closedUniversalP->text().toInt(&okP);
    if (okA) s.a = a;
    if (okB) s.b = b;
    if (okP && p > 0) s.p = p;
    closedSet->setHashStrategy(s);

    int newCap = closedSet->getCapacity();
    closedOutput->setHtml(closedSet->displayAsTable());
    closedHashSummary->setText(QString("n = 0   M = %1   alpha = 0.00").arg(newCap));

    closedProcessLog->append("------------  TABLE CREATED  ------------");
    closedProcessLog->append(QString("Created exactly M = %1 slots.").arg(newCap));
    closedProcessLog->append("");
    closedProcessLog->moveCursor(QTextCursor::End);
}

void MainWindow::showClosedStatus(const QString& msg) { closedStatusLabel->setText(msg); }

void MainWindow::addClosedItem() {
    if (!closedSet) { QMessageBox::warning(this, "Error", "Please create the table first."); return; }

    bool ok;
    int key = closedKeyInput->text().toInt(&ok);
    if (!ok) { QMessageBox::warning(this, "Error", "Key must be a valid integer."); return; }
    std::string val = closedValueInput->text().toStdString();

    int beforeCap = closedSet->getCapacity();
    closedSet->insert(key, val);
    int afterCap  = closedSet->getCapacity();

    QString story = closedSet->explainInsert(key, val);

    if (afterCap != beforeCap) {
        closedModulusEdit->setText(QString::number(afterCap));
    }

    closedHashSummary->setText(QString("n = %1   M = %2   alpha = %3")
                                   .arg(closedSet->getCount())
                                   .arg(closedSet->getCapacity())
                                   .arg(QString::number(closedSet->loadFactor(), 'f', 2)));

    closedOutput->setHtml(closedSet->displayAsTable());

    closedProcessLog->append("-------------  INSERT  -------------");
    if (afterCap != beforeCap) {
        closedProcessLog->append(QString("REHASH:    load factor hit 0.5 -> capacity grew %1 -> %2\n").arg(beforeCap).arg(afterCap));
    }
    closedProcessLog->append(story);
    closedProcessLog->append("");
    closedProcessLog->moveCursor(QTextCursor::End);

    closedKeyInput->clear();
    closedValueInput->clear();
}

void MainWindow::removeClosedItem() {
    if (!closedSet) { QMessageBox::warning(this, "Error", "Please create the table first."); return; }

    bool ok;
    int key = closedKeyInput->text().toInt(&ok);
    if (!ok) { QMessageBox::warning(this, "Error", "Key must be a valid integer."); return; }

    QString story = closedSet->explainRemove(key);
    closedSet->remove(key);

    closedHashSummary->setText(QString("n = %1   M = %2   alpha = %3")
                                   .arg(closedSet->getCount())
                                   .arg(closedSet->getCapacity())
                                   .arg(QString::number(closedSet->loadFactor(), 'f', 2)));

    closedOutput->setHtml(closedSet->displayAsTable());
    closedProcessLog->append("-------------  REMOVE  -------------");
    closedProcessLog->append(story);
    closedProcessLog->append("");
    closedProcessLog->moveCursor(QTextCursor::End);
    closedKeyInput->clear();
}

void MainWindow::doClosedGet() {
    if (!closedSet) { QMessageBox::warning(this, "Error", "Please create the table first."); return; }

    bool ok;
    int key = closedKeyInput->text().toInt(&ok);
    if (!ok) { QMessageBox::warning(this, "Error", "Key must be a valid integer."); return; }
    std::string out;
    if (closedSet->get(key, out)) {
        showClosedStatus(QString("get(%1) -> \"%2\"").arg(key).arg(QString::fromStdString(out)));
    } else {
        showClosedStatus(QString("get(%1) -> (not found)").arg(key));
    }
}

void MainWindow::doClosedContains() {
    if (!closedSet) { QMessageBox::warning(this, "Error", "Please create the table first."); return; }

    bool ok;
    int key = closedKeyInput->text().toInt(&ok);
    if (!ok) { QMessageBox::warning(this, "Error", "Key must be a valid integer."); return; }
    bool present = closedSet->contains(key);
    showClosedStatus(QString("contains(%1) -> %2").arg(key).arg(present ? "true" : "false"));
}

void MainWindow::clearClosedSet() {
    if (!closedSet) { QMessageBox::warning(this, "Error", "Please create the table first."); return; }

    if (closedSet->isEmpty()) {
        showClosedStatus("Table is already empty.");
        return;
    }
    auto reply = QMessageBox::question(this, "Clear Table", "Remove all entries? This cannot be undone.", QMessageBox::Yes | QMessageBox::No);
    if (reply != QMessageBox::Yes) return;

    closedSet->clear();
    closedHashSummary->setText(QString("n = 0   M = %1   alpha = 0.00").arg(closedSet->getCapacity()));
    closedOutput->setHtml(closedSet->displayAsTable());
    closedProcessLog->append("-------------  CLEAR  -------------");
    closedProcessLog->append("All slots reset to empty. count = 0. Capacity unchanged.\n");
    closedProcessLog->moveCursor(QTextCursor::End);
    showClosedStatus("Table cleared. All slots are empty.");
}

// Dijkstra & File Loading Pages (unchanged)
QWidget* MainWindow::buildDijkstraPage() {
    QWidget* page = new QWidget;
    QVBoxLayout* layout = new QVBoxLayout(page);
    layout->setContentsMargins(50, 50, 50, 50);
    layout->setSpacing(25);

    layout->addWidget(buildHeader("Dijkstra Router", "Shortest path from Maseru (A) to any town using Dijkstra's algorithm."));

    QHBoxLayout* topLayout = new QHBoxLayout;
    topLayout->setSpacing(25);

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

    QWidget* resultWidget = new QWidget;
    QVBoxLayout* resLayout = new QVBoxLayout(resultWidget);
    resLayout->setContentsMargins(0, 0, 0, 0);

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
    resultPathLabel->setWordWrap(true);
    routeLayout->addWidget(routeLbl);
    routeLayout->addWidget(resultPathLabel);

    resLayout->addLayout(distRow);
    resLayout->addWidget(line);
    resLayout->addLayout(routeLayout);

    QFrame* line2 = new QFrame;
    line2->setFrameShape(QFrame::HLine);
    line2->setStyleSheet("color: #e4e7eb; margin: 15px 0;");

    QLabel* distLbl = new QLabel("DISTANCE TO EVERY TOWN (from selected start)");
    distLbl->setObjectName("SubText");

    resultDistancesTable = new QTextEdit;
    resultDistancesTable->setReadOnly(true);
    resultDistancesTable->setAcceptRichText(true);
    resultDistancesTable->setMinimumHeight(140);
    resultDistancesTable->setHtml("<span style='color:#5f6368;'>Click Calculate Route to populate.</span>");

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

    const QPointF pA(50,  50);
    const QPointF pB(250, 50);
    const QPointF pC(50,  200);
    const QPointF pD(250, 200);

    auto onPath = [&](const QString& a, const QString& b) {
        for (int i = 0; i + 1 < pathVertices.size(); ++i) {
            if ((pathVertices[i] == a && pathVertices[i+1] == b) ||
                (pathVertices[i] == b && pathVertices[i+1] == a)) return true;
        }
        return false;
    };

    auto drawEdge = [&](QPointF p1, QPointF p2, int weight, const QString& from, const QString& to) {
        bool hot = onPath(from, to);
        QPen pen(hot ? Qt::black : Qt::gray, hot ? 4 : 3, hot ? Qt::SolidLine : Qt::DashLine);
        mapScene->addLine(p1.x(), p1.y(), p2.x(), p2.y(), pen);

        QPointF mid  = (p1 + p2) / 2.0;
        QPointF dir  = p2 - p1;
        double len   = std::sqrt(dir.x() * dir.x() + dir.y() * dir.y());
        QPointF unit = (len > 0.0) ? QPointF(dir.x() / len, dir.y() / len) : QPointF(0.0, 0.0);

        QPointF perp(-unit.y(), unit.x());
        if (perp.y() > 0.0) perp = -perp;
        QPointF labelPos = mid + perp * 16.0;

        QGraphicsTextItem* t = mapScene->addText(QString::number(weight) + " km");
        t->setDefaultTextColor(hot ? Qt::black : Qt::darkGray);
        QFont f = t->font();
        f.setBold(hot);
        t->setFont(f);

        QRectF br = t->boundingRect();
        t->setPos(labelPos.x() - br.width() / 2.0, labelPos.y() - br.height() / 2.0);
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

        QRectF br = t->boundingRect();
        t->setPos(p.x() - br.width() / 2.0, p.y() - br.height() / 2.0);
    };

    drawNode(pA, "A", "A");
    drawNode(pB, "B", "B");
    drawNode(pC, "C", "C");
    drawNode(pD, "D", "D");
}

QString MainWindow::formatDistanceTable(const std::unordered_map<std::string, int>& distances, const std::string& startNode, const std::string& targetNode) const {
    auto pretty = [](const std::string& id) -> QString {
        if (id == "A") return "Maseru (A)";
        if (id == "B") return "Mohale (B)";
        if (id == "C") return "Leribe (C)";
        if (id == "D") return "TY (D)";
        return QString::fromStdString(id);
    };

    QString html;
    html += "<table cellspacing='0' cellpadding='6' border='1' style='border-collapse:collapse; font-family:Consolas,monospace; font-size:13px; width:100%;'>";
    html += "<tr style='background:#1a1a1a; color:#ffffff;'><th>Vertex</th><th>Distance from start</th><th>Status</th></tr>";

    const std::vector<std::string> order = {"A", "B", "C", "D"};
    for (const auto& id : order) {
        auto it = distances.find(id);
        if (it == distances.end()) continue;

        QString bg = "#ffffff";
        if (id == startNode)       bg = "#fff4e5";
        else if (id == targetNode) bg = "#e8f5e9";
        else if (it->second == INT_MAX) bg = "#fafafa";

        QString distTxt = (it->second == INT_MAX) ? QString("inf (unreachable)") : QString("%1 km").arg(it->second);
        QString status = (id == startNode) ? "start" : (id == targetNode) ? "target" : (it->second == INT_MAX) ? "unreachable" : "reachable";

        html += QString("<tr style='background:%1;'><td>%2</td><td align='center'>%3</td><td align='center'>%4</td></tr>")
                    .arg(bg, pretty(id), distTxt, status);
    }
    html += "</table>";
    return html;
}

void MainWindow::calculateRoute() {
    std::unordered_map<std::string, std::vector<Edge>> graph;

    auto addUndirected = [&](const std::string& u, const std::string& v, int w) {
        graph[u].push_back(Edge(v, w));
        graph[v].push_back(Edge(u, w));
        if (graph.find(u) == graph.end()) graph[u] = {};
        if (graph.find(v) == graph.end()) graph[v] = {};
    };

    addUndirected("A", "B", 3);
    addUndirected("A", "C", 1);
    addUndirected("B", "D", 2);
    addUndirected("C", "D", 4);

    auto textToId = [](const QString& t) -> std::string {
        if (t.contains("Maseru"))  return "A";
        if (t.contains("Mohale"))  return "B";
        if (t.contains("Leribe"))  return "C";
        return "D";
    };

    const std::string startNode  = textToId(startDropdown->currentText());
    const std::string targetNode = textToId(targetDropdown->currentText());

    if (startNode == targetNode) {
        resultDistanceLabel->setText("0 km");
        resultPathLabel->setText(startDropdown->currentText());
        drawMap(QStringList{ QString::fromStdString(startNode) });

        DijkstraResult self = runDijkstra(graph, startNode);
        if (self.valid) {
            resultDistancesTable->setHtml(formatDistanceTable(self.distances, startNode, targetNode));
        }
        return;
    }

    DijkstraResult res = runDijkstra(graph, startNode);

    if (!res.valid) {
        resultDistanceLabel->setText("Error");
        resultPathLabel->setText(res.errorMessage);
        resultDistancesTable->setHtml(QString("<span style='color:#d93025;'>%1</span>").arg(res.errorMessage.toHtmlEscaped()));
        drawMap(QStringList{});
        return;
    }

    auto it = res.distances.find(targetNode);
    if (it == res.distances.end() || it->second == INT_MAX) {
        resultDistanceLabel->setText("Unreachable");
        resultPathLabel->setText(QString("No route from %1 to %2.").arg(startDropdown->currentText(), targetDropdown->currentText()));
        resultDistancesTable->setHtml(formatDistanceTable(res.distances, startNode, targetNode));
        drawMap(QStringList{ QString::fromStdString(startNode) });
        return;
    }

    const int distance = it->second;
    resultDistanceLabel->setText(QString("%1 km").arg(distance));

    auto pathList = reconstructPath(res.previous, startNode, targetNode);
    if (pathList.empty()) {
        resultPathLabel->setText("Path reconstruction failed (unexpected).");
        resultDistancesTable->setHtml(formatDistanceTable(res.distances, startNode, targetNode));
        drawMap(QStringList{ QString::fromStdString(startNode) });
        return;
    }

    QStringList pathIds;
    for (const auto& v : pathList) pathIds << QString::fromStdString(v);
    resultPathLabel->setText(printPath(pathList));

    drawMap(pathIds);
    resultDistancesTable->setHtml(formatDistanceTable(res.distances, startNode, targetNode));
}

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
        graph[v].push_back(Edge(u, w));
        if (graph.find(u) == graph.end()) graph[u] = {};
        if (graph.find(v) == graph.end()) graph[v] = {};
    }
    if (graph.find(source) == graph.end()) graph[source] = {};

    DijkstraResult res = runDijkstra(graph, source);

    if (!res.valid) {
        fileOutput->setPlainText(QString("Source: %1\n\n%2").arg(QString::fromStdString(source), res.errorMessage));
        return;
    }

    std::vector<std::string> vertices;
    vertices.reserve(res.distances.size());
    for (const auto& p : res.distances) vertices.push_back(p.first);
    std::sort(vertices.begin(), vertices.end());

    QString log = QString("Source: %1\nEdges: %2\n\n").arg(QString::fromStdString(source)).arg(edges);

    for (const auto& v : vertices) {
        int d = res.distances.at(v);
        log += QString("%1: %2\n").arg(QString::fromStdString(v), d == INT_MAX ? "unreachable" : QString::number(d));
    }
    fileOutput->setPlainText(log);
}