//
// Created by fedor on 31.10.2024.
//

// You may need to build the project (run Qt uic code generator) to get "ui_graphwidget.h" resolved

#include "graphwidget.h"
//#include "ui_graphwidget.h"
#include <QString>
#include <algorithm>


graphwidget::graphwidget(QWidget *parent) :
        QWidget(parent), timer(new QTimer(this)) {
connect(timer, &QTimer::timeout, this, &graphwidget::updateGraphStep);
}


void graphwidget::setData(const std::vector<long double>& time, const std::vector<long double>& temperature) {
    timeValues = time;
    temperatureValues = temperature;

    pointsToShow = 0;
    timer->start(std::max(1, (int)std::abs(total_time/timeValues.size())) );
}

void graphwidget::updateGraphStep() {
    int pointsToAdd = std::min(1000, static_cast<int>(timeValues.size()) - pointsToShow);
    if (pointsToShow < timeValues.size()) {
        pointsToShow += pointsToAdd;
        update();
    } else {
        timer->stop();
    }
}

void graphwidget::update_total_time(long double new_total_time)
{
    total_time = new_total_time;
}

void graphwidget::setTargetTemperature(long double targetTemperature) {
    this->targetTemperature = targetTemperature;
    update();
}

void graphwidget::paintEvent(QPaintEvent *event) {
    QPainter painter(this);

    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(Qt::black);

    if (timeValues.empty() || temperatureValues.empty()) {
        return;
    }


    long double maxTime = *std::max_element(timeValues.begin(), timeValues.end());
    long double maxTemperature = *std::max_element(temperatureValues.begin(), temperatureValues.end());

    int width = this->width();
    int height = this->height();


    painter.drawLine(50, height - 50, width - 50, height - 50); // Ось X
    painter.drawLine(50, 50, 50, height - 50); // Ось Y


    painter.drawText(width - 100, height - 20, "Time (s)");
    painter.drawText(10, 30, "Temperature (K)");

    painter.setPen(Qt::darkBlue);


    for (size_t i = 1; i < pointsToShow && i < timeValues.size(); ++i) {
        int x1 = 50 + static_cast<int>((timeValues[i - 1] / maxTime) * (width - 100));
        int y1 = height - 50 - static_cast<int>((temperatureValues[i - 1] / maxTemperature) * (height - 100));

        int x2 = 50 + static_cast<int>((timeValues[i] / maxTime) * (width - 100));
        int y2 = height - 50 - static_cast<int>((temperatureValues[i] / maxTemperature) * (height - 100));

        painter.drawLine(x1, y1, x2, y2);
    }

    painter.setPen(Qt::red);
    int yTarget = height - 50 - static_cast<int>((targetTemperature / maxTemperature) * (height - 100));
    painter.drawLine(50, yTarget, width - 50, yTarget);


    painter.drawText(5, yTarget, QString::number(targetTemperature, 'f', 2));

    painter.setPen(Qt::darkBlue);
    int xStart = 5 + static_cast<int>((timeValues[0] / maxTime) * (width - 100));
    int yStart = height - 45 - static_cast<int>((temperatureValues[0] / maxTemperature) * (height - 100));
    painter.drawText(xStart, yStart - 10, QString::number(temperatureValues[0], 'f', 2));

    painter.setPen(Qt::black);
    long double total = total_time/1000;
    std::string str = "Total Time: " + std::to_string(total) + " s";
    QString totalTimeText = QString(str.c_str());
    painter.drawText(50, height - 30, totalTimeText);
}

