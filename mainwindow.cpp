//
// Created by fedor on 31.10.2024.
//

// You may need to build the project (run Qt uic code generator) to get "ui_mainwindow.h" resolved

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <algorithm>



mainwindow::mainwindow(QWidget *parent) :
        QMainWindow(parent), ui(new Ui::mainwindow) {
    ui->setupUi(this);

    // Создаем поля ввода
    radiusInput = new QDoubleSpinBox(this);
    radiusInput->setRange(0.001, 1.0); // Устанавливаем диапазон
    radiusInput->setValue(0.01); // Начальное значение

    temperatureInput = new QDoubleSpinBox(this);
    temperatureInput->setRange(0.0, 1000.0); // Устанавливаем диапазон
    temperatureInput->setValue(200); // Начальное значение

    QPushButton *updateButton = new QPushButton("Update Graph", this);
    connect(updateButton, &QPushButton::clicked, this, &mainwindow::updateGraph);

    // Устанавливаем компоновку
    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(radiusInput);
    layout->addWidget(temperatureInput);
    layout->addWidget(updateButton);

    graphwidget = new class graphwidget(this);
    layout->addWidget(graphwidget);

    QWidget *centralWidget = new QWidget(this);
    centralWidget->setLayout(layout);
    setCentralWidget(centralWidget); // Устанавливаем компоновку в главное окно

    // Вызываем начальное обновление графика
    updateGraph();
}

mainwindow::~mainwindow() {
    delete ui;
}

void mainwindow::updateGraph() {
    long double radius = radiusInput->value();
    long double current_temperature = temperatureInput->value();

    // Вызов функции для расчета температур
    calculateTemperatures(radius, current_temperature);
}

void mainwindow::calculateTemperatures(long double radius, long double current_temperature) {
    long double temperature_gas = 3000.0; // Температура газа
    long double previous_temperature;

    long double step = 0.1;
    size_t counter = 0;
    std::vector<long double> time_values; // Временные значения
    std::vector<long double> temperature_values; // Температурные значения

    while (std::abs(temperature_gas - current_temperature) > eps) {
        previous_temperature = current_temperature;

        current_temperature = runge_kutta_4th_order(previous_temperature, step, eps,
                                                    temperature_gas, radius,
                                                    density_air, density_material, Cp_Fe,
                                                    Cp_air, k, S, T0, v0, g);

        // Сохраняем значения
        time_values.push_back(counter);
        temperature_values.push_back(current_temperature);

//        long double delta_temp = std::abs(current_temperature - previous_temperature);
//        if (delta_temp > 0.05) {
//            step /= 2;
//            current_temperature = previous_temperature;
//        }

        long double delta_temp = std::abs(current_temperature - previous_temperature);

        if (delta_temp > 0.05) {
            step *= std::max((long double)0.5, 1.0 - delta_temp / 10.0);
            current_temperature = previous_temperature;
        } else if (delta_temp < 0.001 && step < 1.0) {
            step *= 1.2;
        }


        counter++;
    }

    // Рисуем график на основе time_values и temperature_values
    graphwidget->setData(time_values, temperature_values);
}
