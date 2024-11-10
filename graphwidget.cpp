//
// Created by fedor on 31.10.2024.
//

// You may need to build the project (run Qt uic code generator) to get "ui_graphwidget.h" resolved

#include "graphwidget.h"
//#include "ui_graphwidget.h"
#include <QString>
#include <algorithm>
#include <iostream>


graphwidget::graphwidget(QWidget *parent) :
        QWidget(parent), timer(new QTimer(this))
        {
connect(timer, &QTimer::timeout, this, &graphwidget::updateGraphStep);
}


void graphwidget::setData(const std::vector<long double>& time, const std::vector<long double>& temperature, long double rad) {
    timeValues = time;
    temperatureValues = temperature;

    radius = rad;

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
    copy_total_time = new_total_time;
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

    painter.drawText(width - 100, height - 10, "Time (s)");
    painter.drawText(10, 30, "Temperature (K)");

    long double tempStep = std::ceil(maxTemperature / 1.0) * 0.1;

    for (long double temp = 0; temp <= maxTemperature; temp += tempStep) {
        int y = height - 50 - static_cast<int>((temp / maxTemperature) * (height - 100));
        painter.drawLine(45, y, 50, y);
        painter.drawText(10, y + 5, QString::number(temp, 'f', 1));
    }

    painter.setPen(Qt::darkBlue);
    int xStart = 5 + static_cast<int>((timeValues[0] / maxTime) * (width - 100));
    int yStart = height - 45 - static_cast<int>((temperatureValues[0] / maxTemperature) * (height - 100));
    painter.drawText(xStart, yStart - 10, QString::number(temperatureValues[0], 'f', 2));


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


    int intersectionIndex = -1;
    for (size_t i = 1; i < temperatureValues.size(); ++i) {
        if (std::abs(temperatureValues[i - 1] - targetTemperature) < 0.001) {
            intersectionIndex = i;
            break;
        }
    }

    if (intersectionIndex != -1) {

        long double intersectionTime = timeValues[intersectionIndex];
        int xIntersection = 50 + static_cast<int>((intersectionTime / maxTime) * (width - 100));


        long double tmp_total = copy_total_time;

        total_time = intersectionTime;


        long double stepSize = (maxTime) / 8.0;
        int halfTicks = 6;

        int counter = 0;


        painter.setPen(Qt::black);
        for (int k = -halfTicks; k <= halfTicks-2; ++k)
        {
            if(counter > 10) break;
            long double tickTime = total_time + k * stepSize;


            if (tickTime >= 0 && tickTime <= maxTime)
            {
                int xTick = 50 + static_cast<int>((tickTime / maxTime) * (width - 100));
                auto tmp = tickTime*tmp_total/total_time;

                painter.drawText(xTick, height - 30, QString::number(tmp, 'f', 1));
                painter.drawLine(xTick, height - 50, xTick, height - 45);
            }
            else if (tickTime > maxTime || tickTime < 0)
            {
                QString totalTimeText = "Total Time: " + QString::number(tmp_total, 'f', 2) + " s";
                painter.drawText(xIntersection - totalTimeText.length() * 30, height, totalTimeText);
                return;
            }
            counter++;
        }

    }
    else
    {
        std::cout << "Intersection not found.\n";
        return;
    }

}


