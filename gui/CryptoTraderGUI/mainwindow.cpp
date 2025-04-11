#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QtCharts/QLineSeries>
#include <QtCharts/QChart>
#include <QtCharts/QValueAxis>
#include <QPushButton>
#include <QTimer>
#include <QVBoxLayout>
#include <QLabel>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), strategy(5, 20), trader(1000.0), logger("trade_log.txt")
{
    ui->setupUi(this);

    // Setup chart
    auto series = new QtCharts::QLineSeries();
    auto chart = new QtCharts::QChart();
    chart->addSeries(series);

    // Setup axes
    QtCharts::QValueAxis *axisX = new QtCharts::QValueAxis();
    axisX->setTitleText("Days");
    axisX->setLabelFormat("%i");
    axisX->setTickCount(5);
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    QtCharts::QValueAxis *axisY = new QtCharts::QValueAxis();
    axisY->setTitleText("Price (USD)");
    axisY->setLabelFormat("%.0f");
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    chartView = new QtCharts::QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);

    // Setup layout
    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(centralWidget);
    layout->addWidget(chartView);

    // Add price and balance labels
    priceLabel = new QLabel("Latest Price: $0", this);
    balanceLabel = new QLabel("Balance: $1000", this);
    layout->addWidget(priceLabel);
    layout->addWidget(balanceLabel);

    QPushButton *buyButton = new QPushButton("Buy", this);
    QPushButton *sellButton = new QPushButton("Sell", this);
    layout->addWidget(buyButton);
    layout->addWidget(sellButton);
    setCentralWidget(centralWidget);

    // Connect signals
    connect(buyButton, &QPushButton::clicked, this, &MainWindow::onBuyClicked);
    connect(sellButton, &QPushButton::clicked, this, &MainWindow::onSellClicked);

    // Setup timer for price updates
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::updatePrices);
    timer->start(60000); // Update every minute

    updatePrices();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::updatePrices()
{
    auto prices = fetcher.getPriceHistory("bitcoin", 30);
    if (!prices.empty())
    {
        auto series = dynamic_cast<QtCharts::QLineSeries *>(chartView->chart()->series().first());
        series->clear();
        logger.log("Fetched " + std::to_string(prices.size()) + " prices");

        double minPrice = *std::min_element(prices.begin(), prices.end());
        double maxPrice = *std::max_element(prices.begin(), prices.end());

        for (size_t i = 0; i < prices.size(); ++i)
        {
            series->append(i, prices[i]);
        }

        auto axisX = dynamic_cast<QtCharts::QValueAxis *>(chartView->chart()->axes(Qt::Horizontal).first());
        axisX->setRange(0, prices.size() - 1);

        auto axisY = dynamic_cast<QtCharts::QValueAxis *>(chartView->chart()->axes(Qt::Vertical).first());
        axisY->setRange(minPrice * 0.95, maxPrice * 1.05);

        // Update labels
        priceLabel->setText(QString("Latest Price: $%1").arg(prices.back()));
        balanceLabel->setText(QString("Balance: $%1").arg(trader.getBalance()));

        logger.log("Price updated: " + priceLabel->text().toStdString());
    }
    else
    {
        logger.log("Failed to fetch prices");
    }
}

void MainWindow::onBuyClicked()
{
    auto prices = fetcher.getPriceHistory("bitcoin", 30);
    if (!prices.empty())
    {
        std::string signal = strategy.analyze(prices);
        trader.executeTrade("buy", prices.back());
        balanceLabel->setText(QString("Balance: $%1").arg(trader.getBalance()));
        logger.log("Buy clicked, Signal: " + signal + ", Price: " + std::to_string(prices.back()) +
                   ", Balance: " + std::to_string(trader.getBalance()));
    }
}

void MainWindow::onSellClicked()
{
    auto prices = fetcher.getPriceHistory("bitcoin", 30);
    if (!prices.empty())
    {
        std::string signal = strategy.analyze(prices);
        trader.executeTrade("sell", prices.back());
        balanceLabel->setText(QString("Balance: $%1").arg(trader.getBalance()));
        logger.log("Sell clicked, Signal: " + signal + ", Price: " + std::to_string(prices.back()) +
                   ", Balance: " + std::to_string(trader.getBalance()));
    }
}