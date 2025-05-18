#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QtWidgets>

class MainWindow : public QMainWindow
{
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow() = default;

protected:
    void keyPressEvent(QKeyEvent *event) override;

private:
    void selectTile(char tile);
    void onTileClicked(int row, int col);
    void restorePreviousTile();

    void clearLevel();
    void resizeLevel();

    void exportToFile();
    void importFromFile();
    void setTileAt(int row, int col, char tileChar);


    struct TileAction
    {
        int row;
        int col;
        char previousTile;
        char newTile;
    };

    enum class TileType
    {
        Coin     = '*',
        Enemy    = '&',
        Exit     = 'E',
        Player   = '@',
        Spikes   = '^',
        Wall     = '#',
        DarkWall = '=',
        Air      = '-'
    };

    TileType selectedTile;
    QStack<TileAction> actionHistory;


    QPushButton* createButton(const QIcon &icon, TileType tileType, QHBoxLayout* layout);
    QPushButton* createActionButton(const QIcon &icon, const QString &tooltip, QHBoxLayout* layout);
    QTableWidget* level = nullptr;
};

#endif // MAIN_WINDOW_H
