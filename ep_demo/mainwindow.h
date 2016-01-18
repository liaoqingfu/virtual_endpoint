#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_start_encode_pbt_clicked();
    void on_stop_encode_pbt_clicked();
    void on_start_decode_pbt_clicked();
    void on_stop_decode_pbt_clicked();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
