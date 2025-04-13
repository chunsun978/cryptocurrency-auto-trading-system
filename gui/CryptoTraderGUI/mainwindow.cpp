#include <QChartView>
#include <QChart>
#include <QLineSeries>
#include <QDateTime>
#include <QtCharts/QDateTimeAxis>
#include <QtCharts/QValueAxis>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow),
      balance(1000.0),
      fetcher(),
      strategy(5, 20),              // Initialize StrategyEngine with shortPeriod=5, longPeriod=20
      trader(1000.0),               // Initialize TradeSimulator with initialBalance=1000.0
      logger("trading.log")         // Initialize Logger with filename="trading.log"
{
    ui->setupUi(this);

    // Set up the chart
    priceSeries = new QLineSeries();
    series = priceSeries; // Alias for updatePrices
    chartView = ui->chartWidget; // Use the chartWidget from the UI

    // Initialize chart
    QChart *chart = new QChart();
    chart->addSeries(priceSeries);

    // Set up axes
    QDateTimeAxis *axisX = new QDateTimeAxis();
    axisX->setFormat("yyyy-MM-dd");
    axisX->setTitleText("Date");
    chart->addAxis(axisX, Qt::AlignBottom);
    priceSeries->attachAxis(axisX);

    QValueAxis *axisY = new QValueAxis();
    axisY->setTitleText("Price ($)");
    chart->addAxis(axisY, Qt::AlignLeft);
    priceSeries->attachAxis(axisY);

    chartView->setChart(chart);
    chartView->setRenderHint(QPainter::Antialiasing);

    // Initialize labels
    priceLabel = ui->priceLabel;
    balanceLabel = ui->balanceLabel;
    signalLabel = ui->signalLabel;
    highestPriceLabel = ui->highestPriceLabel;
    lowestPriceLabel = ui->lowestPriceLabel;

    // Update labels with initial values (from your original constructor)
    priceLabel->setText(QString("Latest Price: $%1").arg(85233.2)); // From your screenshot
    balanceLabel->setText(QString("Balance: $%1").arg(balance));
    signalLabel->setText("Signal: sell");
    highestPriceLabel->setText("Highest Price: $87351.70 on 2025-03-24 19:08");
    lowestPriceLabel->setText("Lowest Price: $77695.17 on 2025-04-07 19:04");

    // Set up timer for periodic updates
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::updatePrices);
    timer->start(60000); // Update every 60 seconds

    // Initial price update
    updatePrices();
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::updatePrices() {
    auto priceData = fetcher.getPriceHistory("bitcoin", 30);
    if (!priceData.empty()) {
        series->clear();
        logger.log("Fetched " + std::to_string(priceData.size()) + " prices");

        double minPrice = priceData[0].first;
        double maxPrice = priceData[0].first;
        int minIndex = 0, maxIndex = 0;

        // Plot prices and find min/max
        for (size_t i = 0; i < priceData.size(); ++i) {
            // Use timestamp as X-axis (convert to QDateTime for hover)
            series->append(priceData[i].second, priceData[i].first);

            // Find min and max prices
            if (priceData[i].first < minPrice) {
                minPrice = priceData[i].first;
                minIndex = i;
            }
            if (priceData[i].first > maxPrice) {
                maxPrice = priceData[i].first;
                maxIndex = i;
            }
        }

        auto axisX = dynamic_cast<QDateTimeAxis *>(chartView->chart()->axes(Qt::Horizontal).first());
        auto axisY = dynamic_cast<QValueAxis *>(chartView->chart()->axes(Qt::Vertical).first());
        axisX->setRange(QDateTime::fromMSecsSinceEpoch(priceData[0].second),
                        QDateTime::fromMSecsSinceEpoch(priceData.back().second));
        axisY->setRange(minPrice * 0.95, maxPrice * 1.05);

        // Update labels
        priceLabel->setText(QString("Latest Price: $%1").arg(priceData.back().first));
        balanceLabel->setText(QString("Balance: $%1").arg(trader.getBalance()));

        // Update signal with color
        std::string signal = strategy.analyze(priceData);
        signalLabel->setText(QString("Signal: %1").arg(QString::fromStdString(signal)));
        if (signal == "buy") {
            signalLabel->setStyleSheet("QLabel { color: green; }");
        } else if (signal == "sell") {
            signalLabel->setStyleSheet("QLabel { color: red; }");
        } else {
            signalLabel->setStyleSheet("QLabel { color: yellow; }");
        }

        // Update highest and lowest price labels with dates
        QDateTime highestDateTime = QDateTime::fromMSecsSinceEpoch(priceData[maxIndex].second);
        QDateTime lowestDateTime = QDateTime::fromMSecsSinceEpoch(priceData[minIndex].second);
        highestPriceLabel->setText(QString("Highest Price: $%1 on %2")
                                       .arg(maxPrice, 0, 'f', 2)
                                       .arg(highestDateTime.toString("yyyy-MM-dd HH:mm")));
        lowestPriceLabel->setText(QString("Lowest Price: $%1 on %2")
                                      .arg(minPrice, 0, 'f', 2)
                                      .arg(lowestDateTime.toString("yyyy-MM-dd HH:mm")));

        logger.log("Price updated: " + priceLabel->text().toStdString());
        logger.log("Signal updated: " + signal);
    } else {
        logger.log("Failed to fetch prices");
    }
}

void MainWindow::on_buyButton_clicked() {
    QMessageBox::information(this, "Buy", "Buy button clicked (functionality to be implemented)");
}

void MainWindow::on_sellButton_clicked() {
    QMessageBox::information(this, "Sell", "Sell button clicked (functionality to be implemented)");
}