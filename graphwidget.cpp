//
// Created by fedor on 31.10.2024.
//

// You may need to build the project (run Qt uic code generator) to get "ui_graphwidget.h" resolved

#include "graphwidget.h"
//#include "ui_graphwidget.h"


graphwidget::graphwidget(QWidget *parent) :
        QWidget(parent){}

//graphwidget::~graphwidget() {
//    delete ui;
//}

void graphwidget::setData(const std::vector<long double>& time, const std::vector<long double>& temperature) {
    timeValues = time;
    temperatureValues = temperature;
    update(); // Перерисовываем виджет
}

void graphwidget::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    // Устанавливаем масштабирование для графика
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(Qt::blue);

    if (timeValues.empty() || temperatureValues.empty()) {
        return; // Если данных нет, ничего не рисуем
    }

    // Нормализуем данные для отображения
    long double maxTime = *std::max_element(timeValues.begin(), timeValues.end());
    long double maxTemperature = *std::max_element(temperatureValues.begin(), temperatureValues.end());

    int width = this->width();
    int height = this->height();

    // Рисуем оси
    painter.drawLine(50, height - 50, width - 50, height - 50); // Ось X
    painter.drawLine(50, 50, 50, height - 50); // Ось Y

    // Рисуем график
    for (size_t i = 1; i < timeValues.size(); ++i) {
        int x1 = 50 + static_cast<int>((timeValues[i - 1] / maxTime) * (width - 100));
        int y1 = height - 50 - static_cast<int>((temperatureValues[i - 1] / maxTemperature) * (height - 100));

        int x2 = 50 + static_cast<int>((timeValues[i] / maxTime) * (width - 100));
        int y2 = height - 50 - static_cast<int>((temperatureValues[i] / maxTemperature) * (height - 100));

        painter.drawLine(x1, y1, x2, y2);
    }
}

