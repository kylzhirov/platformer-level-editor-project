#include "main_window.h"
#include "utilities.h"



MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), selectedTile(TileType::Air)
{
    setWindowTitle("Platformer Level Editor");
    setFocusPolicy(Qt::StrongFocus);

    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    centralWidget->setFocusPolicy(Qt::StrongFocus);

    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    level = new QTableWidget(10, 300);

    // QTable headers stretching
    level->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    level->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);


    level->verticalHeader()->setDefaultSectionSize(24);
    level->horizontalHeader()->setDefaultSectionSize(24);
    level->verticalHeader()->setMinimumSectionSize(24);
    level->horizontalHeader()->setMinimumSectionSize(24);
    level->verticalHeader()->hide();
    level->horizontalHeader()->hide();
    level->setEditTriggers(QAbstractItemView::NoEditTriggers);

    level->setSelectionMode(QAbstractItemView::NoSelection);
    level->setSelectionBehavior(QAbstractItemView::SelectItems);
    connect(level, &QTableWidget::cellClicked, this, &MainWindow::onTileClicked);


    // Setting buttons



    mainLayout->addWidget(level);
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    QPushButton *WallButton = createButton(QIcon("data/sprites/wall.png"), TileType::Wall, buttonLayout);
    QPushButton *exitButton = createButton(QIcon("data/sprites/exit.png"), TileType::Exit, buttonLayout);
    QPushButton *coinButton = createButton(QIcon("data/sprites/coin.png"), TileType::Coin, buttonLayout);
    QPushButton *enemyButton = createButton(QIcon("data/sprites/enemy.png"), TileType::Enemy, buttonLayout);
    QPushButton *playerButton = createButton(QIcon("data/sprites/player.png"), TileType::Player, buttonLayout);
    QPushButton *spikeButton = createButton(QIcon("data/sprites/spikes.png"), TileType::Spikes, buttonLayout);
    QPushButton *darkWallButton = createButton(QIcon("data/sprites/wall_dark.png"), TileType::DarkWall, buttonLayout);

    QPushButton *clearButton = createActionButton(QIcon("data/sprites/recycle-bin.png"), "Clear Level", buttonLayout);

    mainLayout->addLayout(buttonLayout);
    centralWidget->show();

    QToolBar *toolBar = addToolBar("File operations");
    QAction *exportAction = new QAction("Export", this);
    connect(exportAction, &QAction::triggered, this, &MainWindow::exportToFile);
    toolBar->addAction(exportAction);

    QAction *undoAction = new QAction("Undo", this);
    undoAction->setShortcut(QKeySequence("Ctrl+Z"));
    connect(undoAction, &QAction::triggered, this, &MainWindow::restorePreviousTile);
    toolBar->addAction(undoAction);
}

void MainWindow::onTileClicked(int row, int col)
{
    QTableWidgetItem* item = level->item(row, col);

    char previousTile = '-'; // Default to air

    if (item != nullptr && item->data(Qt::UserRole).isValid()) {
        previousTile = item->data(Qt::UserRole).toChar().toLatin1();
    }

    if (item == nullptr) {
        item = new QTableWidgetItem();
        level->setItem(row, col, item);
    }

    QIcon icon;
    char newTile = ' ';


    switch (selectedTile) {
        case TileType::Coin: icon = QIcon("data/sprites/coin.png"); break;
        case TileType::Enemy: icon = QIcon("data/sprites/enemy.png"); break;
        case TileType::Exit: icon = QIcon("data/sprites/exit.png"); break;
        case TileType::Player: icon = QIcon("data/sprites/player.png"); break;
        case TileType::Spikes: icon = QIcon("data/sprites/spikes.png"); break;
        case TileType::Wall: icon = QIcon("data/sprites/wall.png"); break;
        case TileType::DarkWall: icon = QIcon("data/sprites/wall_dark.png"); break;
    }

    if (previousTile != newTile) {
        TileAction action = {row, col, previousTile, newTile};
        actionHistory.push(action);
    }


    item->setIcon(icon);
    char data = ' ';
    switch (selectedTile) {
        case TileType::Coin:  data = '*'; break;
        case TileType::Enemy: data = '&'; break;
        case TileType::Exit: data = 'E'; break;
        case TileType::Player: data = '@'; break;
        case TileType::Spikes: data = '^'; break;
        case TileType::Wall: data = '#'; break;
        case TileType::DarkWall: data = '='; break;
    }
    item->setData(Qt::UserRole, data);
}


void MainWindow::restorePreviousTile()
{
    if (actionHistory.isEmpty()) return; // cancel if empty

    TileAction action = actionHistory.pop();

    // Get the pos
    QTableWidgetItem* item = level->item(action.row, action.col);


    QIcon icon;
    switch (action.previousTile) {
        case '*': icon = QIcon("data/sprites/coin.png"); break;
        case '&': icon = QIcon("data/sprites/enemy.png"); break;
        case 'E': icon = QIcon("data/sprites/exit.png"); break;
        case '@': icon = QIcon("data/sprites/player.png"); break;
        case '^': icon = QIcon("data/sprites/spikes.png"); break;
        case '#': icon = QIcon("data/sprites/wall.png"); break;
        case '=': icon = QIcon("data/sprites/wall_dark.png"); break;
        case '-': icon = QIcon(); break;
    }

    item->setIcon(icon);
    item->setData(Qt::UserRole, action.previousTile);
}


void MainWindow::keyPressEvent(QKeyEvent *event)
{
    // TODO

    QMainWindow::keyPressEvent(event);
}

void MainWindow::selectTile(char tile)
{
    switch(tile) {
        case '*': selectedTile = TileType::Coin;     break;
        case '&': selectedTile = TileType::Enemy;    break;
        case 'E': selectedTile = TileType::Exit;     break;
        case '@': selectedTile = TileType::Player;   break;
        case '^': selectedTile = TileType::Spikes;   break;
        case '#': selectedTile = TileType::Wall;     break;
        case '=': selectedTile = TileType::DarkWall; break;
        case '-': selectedTile = TileType::Air;      break;
    }
}



QPushButton* MainWindow::createButton(const QIcon &icon, TileType tileType, QHBoxLayout* layout)
{
    QPushButton *button = new QPushButton();

    button->setFixedSize(32, 32);
    button->setIcon(icon);
    connect(button, &QPushButton::clicked, this, [this,  tileType](){selectTile(static_cast<char>(tileType));});
    layout->addWidget(button);
    return button;
}

QPushButton* MainWindow::createActionButton(const QIcon &icon, const QString &tooltip, QHBoxLayout* layout)
{
    QPushButton *button = new QPushButton();
    button->setFixedSize(32, 32);
    button->setIcon(icon);
    button->setToolTip(tooltip);
    connect(button, &QPushButton::clicked, this, &MainWindow::clearLevel);
    layout->addWidget(button);
    return button;
}

void MainWindow::clearLevel()
{
    QMessageBox::StandardButton confirm = QMessageBox::question(
    this,
    "Clear Level",
    "Are you sure? This action cannot be undone.",
    QMessageBox::Yes | QMessageBox::No
    );

    if (confirm == QMessageBox::No) {
        return; // User cancelled the operation
    }

    int rows = level->rowCount();
    int cols = level->columnCount();

    // Clear the undo history
    actionHistory.clear();


    for (int row = 0; row < rows; row++) {
        for (int col = 0; col < cols; col++) {
            QTableWidgetItem* item = level->item(row, col);
            if (item) {
                item->setIcon(QIcon()); // empty Icon
                item->setData(Qt::UserRole, '-');
            }
        }
    }

    statusBar()->showMessage("Level cleared");
}

void MainWindow::resizeLevel()
{
    QDialog resizeDialog(this);
    // TODO
    if (resizeDialog.exec() == QDialog::Accepted) {
        // TODO
    }
}

void MainWindow::exportToFile()
{
    QString filePath = QFileDialog::getSaveFileName(this);

    // Cancel if the filepath is not given
    if (filePath.isEmpty()) {
        return;
    }

    if (!filePath.endsWith(".rll")) filePath += ".rll";
    
    QFile file(filePath);
    if (!file.open(QIODevice::ReadWrite)) {
        QMessageBox::critical(this, "Error", "Could not open the file");
        return;
    }
    
    QTextStream out(&file);

    int rows = level->rowCount();
    int cols = level->columnCount();
    
    // buffer to hold the level data
    std::vector<char> data(rows * cols, '-');
    // Def value is '-' (Air)
    
    // Fill the buffer
    for (int row = 0; row < rows; row++) {
        for (int col = 0; col < cols; col++) {
            QTableWidgetItem* item = level->item(row, col);
            if (item) {
                char tile = item->data(Qt::UserRole).toChar().toLatin1();
                data[row * cols + col] = tile; // 2D (x and y) to 1D index
            }
        }
    }

    QString output;
    encrypt(rows, cols, data.data(), output);

    out << output;
    
    file.close();
    
    QMessageBox::information(this, "Success!", "Level export is done");
}