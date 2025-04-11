#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QDateTimeAxis> // Add for date axis
#include <QLabel>
#include "../../include/DataFetcher.hpp"
#include "../../include/StrategyEngine.hpp"
#include "../../include/TradeSimulator.hpp"
#include "../../include/Logger.hpp"

QT_BEGIN_NAMESPACE
namespace Ui
{
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
    void updatePrices();
    void onBuyClicked();
    void onSellClicked();

private:
    Ui::MainWindow *ui;
    DataFetcher fetcher;
    StrategyEngine strategy;
    TradeSimulator trader;
    Logger logger;
    QtCharts::QChartView *chartView;
    QtCharts::QLineSeries *series;
    QTimer *timer;
    QLabel *priceLabel;
    QLabel *balanceLabel;
    QLabel *signalLabel;
    QLabel *highestPriceLabel;
    QLabel *lowestPriceLabel;
};
#endif