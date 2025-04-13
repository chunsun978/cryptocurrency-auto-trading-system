#include <QChartView>
#include <QChart>
#include <QLineSeries>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), balance(1000.0) {
    ui->setupUi(this);

    // Set up the chart
    priceSeries = new QLineSeries();
    // Example data (replace with your actual price data)
    // Based on the screenshot, you have 31 daily prices from 2025-03-14 to 2025-04-12
    QList<double> prices = {
        82765, 82650, 82500, 82700, 83000, 82800, 82750, 82900, 83100, 83200,
        83050, 82950, 82800, 82600, 82500, 82400, 82300, 82200, 82000, 81800,
        81600, 81500, 81300, 81200, 81100, 81000, 81200, 81500, 82000, 82500,
        85233.2 // Latest price from screenshot
    };
    for (int i = 0; i < prices.size(); ++i) {
        priceSeries->append(i, prices[i]);
    }

    QChart *chart = new QChart();
    chart->addSeries(priceSeries);
    chart->createDefaultAxes();
    ui->chartWidget->setChart(chart);
    ui->chartWidget->setRenderHint(QPainter::Antialiasing);

    // Update labels with initial values
    ui->priceLabel->setText(QString("Latest Price: $%1").arg(prices.last()));
    ui->balanceLabel->setText(QString("Balance: $%1").arg(balance));
    ui->signalLabel->setText("Signal: sell"); // From screenshot

    ui->highestPriceLabel->setText("Highest Price: $87351.70 on 2025-03-24 19:08");
    ui->lowestPriceLabel->setText("Lowest Price: $77695.17 on 2025-04-07 19:04");
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::on_buyButton_clicked() {
    QMessageBox::information(this, "Buy", "Buy button clicked (functionality to be implemented)");
}

void MainWindow::on_sellButton_clicked() {
    QMessageBox::information(this, "Sell", "Sell button clicked (functionality to be implemented)");
}
