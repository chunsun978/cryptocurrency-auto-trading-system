#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QChart>
#include <QMainWindow>
#include <QLabel>
#include <QTimer>

#include "../../include/DataFetcher.hpp"
#include "../../include/StrategyEngine.hpp"
#include "../../include/TradeSimulator.hpp"
#include "../../include/Logger.hpp"

QT_BEGIN_NAMESPACE
namespace Ui {
    class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
private slots:
    void on_buyButton_clicked();
    void on_sellButton_clicked();
    void updatePrices();

private:
    Ui::MainWindow *ui;
    QLineSeries *priceSeries;
    double balance;

    DataFetcher fetcher;
    StrategyEngine strategy;
    TradeSimulator trader;
    Logger logger;

    QChartView *chartView;
    QLineSeries *series;
    QTimer *timer;

    QLabel *priceLabel;
    QLabel *balanceLabel;
    QLabel *signalLabel;
    QLabel *highestPriceLabel;
    QLabel *lowestPriceLabel;
};
#endif