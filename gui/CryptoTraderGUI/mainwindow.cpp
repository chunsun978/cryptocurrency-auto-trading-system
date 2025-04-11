#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QtCharts/QLineSeries>
#include <QtCharts/QChart>
#include <QtCharts/QValueAxis>
#include <QtCharts/QDateTimeAxis>
#include <QPushButton>
#include <QTimer>
#include <QVBoxLayout>
#include <QLabel>
#include <QToolTip>
#include <QDateTime>
#include <QMouseEvent>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), strategy(5, 20), trader(1000.0), logger("trade_log.txt")
{
    ui->setupUi(this);

    // Setup chart
    series = new QtCharts::QLineSeries();
    auto chart = new QtCharts::QChart();
    chart->addSeries(series);

    // Use QDateTimeAxis for X-axis
    QtCharts::QDateTimeAxis *axisX = new QtCharts::QDateTimeAxis();
    axisX->setTitleText("Date");
    axisX->setFormat("yyyy-MM-dd");
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

    priceLabel = new QLabel("Latest Price: $0", this);
    balanceLabel = new QLabel("Balance: $1000", this);
    signalLabel = new QLabel("Signal: Hold", this);
    highestPriceLabel = new QLabel("Highest Price: N/A", this);
    lowestPriceLabel = new QLabel("Lowest Price: N/A", this);

    layout->addWidget(priceLabel);
    layout->addWidget(balanceLabel);
    layout->addWidget(signalLabel);
    layout->addWidget(highestPriceLabel);
    layout->addWidget(lowestPriceLabel);

    QPushButton *buyButton = new QPushButton("Buy", this);
    QPushButton *sellButton = new QPushButton("Sell", this);
    layout->addWidget(buyButton);
    layout->addWidget(sellButton);
    setCentralWidget(centralWidget);

    connect(buyButton, &QPushButton::clicked, this, &MainWindow::onBuyClicked);
    connect(sellButton, &QPushButton::clicked, this, &MainWindow::onSellClicked);

    // Enable hover functionality for the chart
    chartView->setMouseTracking(true);
    connect(series, &QtCharts::QLineSeries::hovered, this, [this](const QPointF &point, bool state)
            {
        if (state) {
            // Find the closest point in the series
            qreal closestDistance = std::numeric_limits<qreal>::max();
            int closestIndex = -1;
            const auto points = series->points();
            for (int i = 0; i < points.size(); ++i) {
                qreal distance = qAbs(points[i].x() - point.x());
                if (distance < closestDistance) {
                    closestDistance = distance;
                    closestIndex = i;
                }
            }
            if (closestIndex >= 0) {
                double price = points[closestIndex].y();
                QDateTime dateTime = QDateTime::fromMSecsSinceEpoch(points[closestIndex].x());
                QString tooltip = QString("Price: $%1\nDate: %2")
                                      .arg(price, 0, 'f', 2)
                                      .arg(dateTime.toString("yyyy-MM-dd HH:mm"));
                QToolTip::showText(QCursor::pos(), tooltip);
            }
        } else {
            QToolTip::hideText();
        } });

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
    auto priceData = fetcher.getPriceHistory("bitcoin", 30);
    if (!priceData.empty())
    {
        series->clear();
        logger.log("Fetched " + std::to_string(priceData.size()) + " prices");

        double minPrice = priceData[0].first;
        double maxPrice = priceData[0].first;
        int minIndex = 0, maxIndex = 0;

        // Plot prices and find min/max
        for (size_t i = 0; i < priceData.size(); ++i)
        {
            // Use timestamp as X-axis (convert to QDateTime for hover)
            series->append(priceData[i].second, priceData[i].first);

            // Find min and max prices
            if (priceData[i].first < minPrice)
            {
                minPrice = priceData[i].first;
                minIndex = i;
            }
            if (priceData[i].first > maxPrice)
            {
                maxPrice = priceData[i].first;
                maxIndex = i;
            }
        }

        auto axisX = dynamic_cast<QtCharts::QDateTimeAxis *>(chartView->chart()->axes(Qt::Horizontal).first());
        axisX->setRange(QDateTime::fromMSecsSinceEpoch(priceData[0].second),
                        QDateTime::fromMSecsSinceEpoch(priceData.back().second));

        auto axisY = dynamic_cast<QtCharts::QValueAxis *>(chartView->chart()->axes(Qt::Vertical).first());
        axisY->setRange(minPrice * 0.95, maxPrice * 1.05);

        // Update labels
        priceLabel->setText(QString("Latest Price: $%1").arg(priceData.back().first));
        balanceLabel->setText(QString("Balance: $%1").arg(trader.getBalance()));

        // Update signal with color
        std::string signal = strategy.analyze(priceData);
        signalLabel->setText(QString("Signal: %1").arg(QString::fromStdString(signal)));
        if (signal == "buy")
        {
            signalLabel->setStyleSheet("QLabel { color: green; }");
        }
        else if (signal == "sell")
        {
            signalLabel->setStyleSheet("QLabel { color: red; }");
        }
        else
        {
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
    }
    else
    {
        logger.log("Failed to fetch prices");
    }
}

void MainWindow::onBuyClicked()
{
    auto priceData = fetcher.getPriceHistory("bitcoin", 30);
    if (!priceData.empty())
    {
        std::string signal = strategy.analyze(priceData);
        trader.executeTrade("buy", priceData.back().first);
        balanceLabel->setText(QString("Balance: $%1").arg(trader.getBalance()));
        signalLabel->setText(QString("Signal: %1").arg(QString::fromStdString(signal)));
        if (signal == "buy")
        {
            signalLabel->setStyleSheet("QLabel { color: green; }");
        }
        else if (signal == "sell")
        {
            signalLabel->setStyleSheet("QLabel { color: red; }");
        }
        else
        {
            signalLabel->setStyleSheet("QLabel { color: yellow; }");
        }
        logger.log("Buy clicked, Signal: " + signal + ", Price: " + std::to_string(priceData.back().first) +
                   ", Balance: " + std::to_string(trader.getBalance()));
    }
}

void MainWindow::onSellClicked()
{
    auto priceData = fetcher.getPriceHistory("bitcoin", 30);
    if (!priceData.empty())
    {
        std::string signal = strategy.analyze(priceData);
        trader.executeTrade("sell", priceData.back().first);
        balanceLabel->setText(QString("Balance: $%1").arg(trader.getBalance()));
        signalLabel->setText(QString("Signal: %1").arg(QString::fromStdString(signal)));
        if (signal == "buy")
        {
            signalLabel->setStyleSheet("QLabel { color: green; }");
        }
        else if (signal == "sell")
        {
            signalLabel->setStyleSheet("QLabel { color: red; }");
        }
        else
        {
            signalLabel->setStyleSheet("QLabel { color: yellow; }");
        }
        logger.log("Sell clicked, Signal: " + signal + ", Price: " + std::to_string(priceData.back().first) +
                   ", Balance: " + std::to_string(trader.getBalance()));
    }
}