//
// Created by fedor on 31.10.2024.
//

// You may need to build the project (run Qt uic code generator) to get "ui_mainwindow.h" resolved

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <algorithm>
#include <QLabel>



mainwindow::mainwindow(QWidget *parent) :
        QMainWindow(parent), ui(new Ui::mainwindow)
        {
    ui->setupUi(this);

    QLabel *radiusLabel = new QLabel("Радиус (blue):", this);
    QLabel *radiusLabel2 = new QLabel("Радиус 2 (green):", this);
    QLabel *temperatureLabel = new QLabel("Начальная температура частицы:", this);
    QLabel *maxTemperatureLabel = new QLabel("температура потока:", this);

    // Создаем поля ввода
    radiusInput = new QDoubleSpinBox(this);
    radiusInput->setRange(0.0001, 0.1);
    radiusInput->setValue(0.0001);
    radiusInput->setDecimals(4);
    radiusInput->setSingleStep(0.0001);

    radiusInput2 = new QDoubleSpinBox(this);
    radiusInput2->setRange(0.0001, 0.1);
    radiusInput2->setValue(0.1);
    radiusInput2->setDecimals(4);
    radiusInput2->setSingleStep(0.0001);

    temperatureInput = new QDoubleSpinBox(this);
    temperatureInput->setRange(0.0, 2000.0);
    temperatureInput->setValue(200);

    maxTemperatureInput = new QDoubleSpinBox(this);
    maxTemperatureInput->setRange(0.0, 2000.0);
    maxTemperatureInput->setValue(300);

    QPushButton *updateButton = new QPushButton("Update Graph", this);
    connect(updateButton, &QPushButton::clicked, this, &mainwindow::updateGraph);

    QHBoxLayout *radiusLayout = new QHBoxLayout();
    radiusLayout->addWidget(radiusLabel);
    radiusLayout->addWidget(radiusInput);

    QHBoxLayout *radiusLayout2 = new QHBoxLayout();
    radiusLayout2->addWidget(radiusLabel2);
    radiusLayout2->addWidget(radiusInput2);

    QHBoxLayout *maxTemperatureLayout = new QHBoxLayout();
    maxTemperatureLayout->addWidget(maxTemperatureLabel);
    maxTemperatureLayout->addWidget(maxTemperatureInput);


    QHBoxLayout *temperatureLayout = new QHBoxLayout();
    temperatureLayout->addWidget(temperatureLabel);
    temperatureLayout->addWidget(temperatureInput);


    QVBoxLayout *layout = new QVBoxLayout();
    layout->addLayout(radiusLayout);
    layout->addLayout(radiusLayout2);
    layout->addLayout(temperatureLayout);
    layout->addLayout(maxTemperatureLayout);
    layout->addWidget(updateButton);

    graphwidget = new class graphwidget(this);
    layout->addWidget(graphwidget);

    QWidget *centralWidget = new QWidget(this);
    centralWidget->setLayout(layout);
    setCentralWidget(centralWidget);


    updateGraph();
}

mainwindow::~mainwindow() {
    delete ui;
}

void mainwindow::updateGraph() {
    long double radius = radiusInput->value();
    long double radius2 = radiusInput2->value();
    long double current_temperature = temperatureInput->value();


    calculateTemperatures(radius, current_temperature, radius2);
}

void mainwindow::calculateTemperatures(long double radius, long double current_temperature, long double radius2) {
    long double temperature_gas = maxTemperatureInput->value();
    long double previous_temperature;

    long double step = 0.1;
    long double total_time = 0.0;
    size_t counter = 0;
    std::vector<long double> time_values;
    std::vector<long double> temperature_values;
    auto beg_temperature = current_temperature;



    while (std::abs(temperature_gas - current_temperature) > eps) {
        previous_temperature = current_temperature;

        current_temperature = runge_kutta_4th_order(previous_temperature, step, eps,
                                                    temperature_gas, radius,
                                                    density_air, density_material, Cp_Fe,
                                                    Cp_air, k, S, T0, v0, g);


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

        //total_time += step;
        counter++;
    }

    counter = 0;
    step = 0.1;

    current_temperature = beg_temperature;
    long double current_temperature2 = beg_temperature; // Начальная температура для второго радиуса
    std::vector<long double> temperature_values2; // Вектор для второго графика
    std::vector<long double> time_values2;
    long double previous_temperature2;

    while (std::abs(temperature_gas - current_temperature2) > eps) {
        previous_temperature2 = current_temperature2;

        current_temperature2 = runge_kutta_4th_order(previous_temperature2, step, eps,
                                                     temperature_gas, radius2,
                                                     density_air, density_material, Cp_Fe,
                                                     Cp_air, k, S, T0, v0, g);

        time_values2.push_back(counter);
        temperature_values2.push_back(current_temperature2);

        long double delta_temp = std::abs(current_temperature2 - previous_temperature2);

        if (delta_temp > 0.05) {
            step *= std::max((long double)0.5, 1.0 - delta_temp / 10.0);
            current_temperature2 = previous_temperature2;
        } else if (delta_temp < 0.001 && step < 1.0) {
            step *= 1.2;
        }

        // Установите минимальное значение шага
//        if (step < 0.01) {
//            step = 0.01;
//        }

        //total_time += step;
        counter++;
    }


    auto h = get_h(eps, k, radius, temperature_gas, density_air, S, T0, v0, Cp_air, g);

    if(beg_temperature < temperature_gas)
    {

        total_time = (density_material*Cp_Fe*radius*radius*2)/(3*h) * std::log(( temperature_gas - beg_temperature )/(0.01*temperature_gas));
    }
    else
    {
        total_time = -(density_material*Cp_Fe*radius*radius*2)/(3*h)* std::log((0.01*temperature_gas )/(beg_temperature - temperature_gas));
    }






    //std::cout << "total_time = " << total_time/1000 << " sec\n";
    std::cout << "total_time = " << total_time<< " sec\n";




    graphwidget->setData(time_values, temperature_values, time_values2, temperature_values2);
    graphwidget->setTargetTemperature(temperature_gas);
    graphwidget->update_total_time(total_time);
}
