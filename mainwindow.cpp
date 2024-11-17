//
// Created by fedor on 31.10.2024.
//

// You may need to build the project (run Qt uic code generator) to get "ui_mainwindow.h" resolved

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <algorithm>
#include <QLabel>
#include <QComboBox>



mainwindow::mainwindow(QWidget *parent) :
        QMainWindow(parent), ui(new Ui::mainwindow)
        {
            std::ofstream file2(filename, std::ios::trunc );
            if(!file2.is_open())
                throw std::runtime_error("Can't open file! 1");
            file2.close();

    ui->setupUi(this);

    QComboBox *materialComboBox = new QComboBox(this);
    materialComboBox->addItem("Железо");
    materialComboBox->addItem("Алюминий");
    materialComboBox->addItem("Медь");


    QMap<QString, QPair<long double, long double>> materialProperties;
    materialProperties["Железо"] = qMakePair(7874.0, 460.0);
    materialProperties["Алюминий"] = qMakePair(2700.0, 900.0);
    materialProperties["Медь"] = qMakePair(8960.0, 385.0);


    connect(materialComboBox, &QComboBox::currentTextChanged, this, [=](const QString &material) {
        auto properties = materialProperties[material];
        name_material = materialComboBox->currentText().toStdString();
        density_material = properties.first;
        Cp_Fe = properties.second;
        updateGraph();
    });


    density_material = materialProperties["Железо"].first;
    Cp_Fe = materialProperties["Железо"].second;

    auto str = materialComboBox->currentText();
    name_material = str.toStdString();

    QLabel *radiusLabel = new QLabel("Радиус (blue) (м):", this);

    QLabel *temperatureLabel = new QLabel("Начальная температура частицы (K):", this);
    QLabel *maxTemperatureLabel = new QLabel("Температура потока (K):", this);


    radiusInput = new QDoubleSpinBox(this);
    radiusInput->setRange(0.0001, 0.1);
    radiusInput->setValue(0.0001);
    radiusInput->setDecimals(4);
    radiusInput->setSingleStep(0.0001);



    temperatureInput = new QDoubleSpinBox(this);
    temperatureInput->setRange(298.15, 2000.0);
    temperatureInput->setValue(298.15);

    maxTemperatureInput = new QDoubleSpinBox(this);
    maxTemperatureInput->setRange(298.15, 2000.0);
    maxTemperatureInput->setValue(2000.0);

    QPushButton *updateButton = new QPushButton("Update Graph", this);
    connect(updateButton, &QPushButton::clicked, this, &mainwindow::updateGraph);

    QHBoxLayout *radiusLayout = new QHBoxLayout();
    radiusLayout->addWidget(radiusLabel);
    radiusLayout->addWidget(radiusInput);



    QHBoxLayout *maxTemperatureLayout = new QHBoxLayout();
    maxTemperatureLayout->addWidget(maxTemperatureLabel);
    maxTemperatureLayout->addWidget(maxTemperatureInput);


    QHBoxLayout *temperatureLayout = new QHBoxLayout();
    temperatureLayout->addWidget(temperatureLabel);
    temperatureLayout->addWidget(temperatureInput);

    QHBoxLayout *materialLayout = new QHBoxLayout();
    materialLayout->addWidget(new QLabel("Выберите материал:", this)); // Подпись для выбора материала
    materialLayout->addWidget(materialComboBox);  // Выпадающий список для выбора материала

    QVBoxLayout *layout = new QVBoxLayout();
    layout->addLayout(materialLayout);
    layout->addLayout(radiusLayout);
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

mainwindow::~mainwindow()
{
    delete ui;
}

void mainwindow::updateGraph()
{
    long double radius = radiusInput->value();
    long double current_temperature = temperatureInput->value();


    calculateTemperatures(radius, current_temperature);
}

void mainwindow::calculateTemperatures(long double radius, long double current_temperature)
{
    long double temperature_gas = maxTemperatureInput->value();
    long double previous_temperature;

    long double step = 0.1;
    long double total_time = 0.0;
    size_t counter = 0;
    std::vector<long double> time_values;
    std::vector<long double> temperature_values;
    std::vector<long double> velocity_values;
    std::vector<long double> distance_values;
    std::vector<long double> counter_values;
    auto beg_temperature = current_temperature;

    long double velocity = 0.0; // Начальная скорость частицы
    long double distance = 0.0; // Начальное расстояние




    while (std::abs(temperature_gas - current_temperature) > eps) {
        previous_temperature = current_temperature;

        current_temperature = runge_kutta_4th_order(previous_temperature, step, eps,
                                                    temperature_gas, radius,
                                                    density_air, density_material, Cp_Fe,
                                                    Cp_air, k, S, T0, v0, g);

        runge_kutta_4_distance(velocity, distance, step*0.0001, g);


        time_values.push_back(counter);
        temperature_values.push_back(current_temperature);
        velocity_values.push_back(velocity);
        distance_values.push_back(distance);


        counter_values.push_back(counter);
        time_values.push_back(counter);
        temperature_values.push_back(current_temperature);


        long double delta_temp = std::abs(current_temperature - previous_temperature);

        if (delta_temp > 0.5)
        {
            step *= std::max((long double)0.5, 1.0 - delta_temp / 10.0);
            current_temperature = previous_temperature;
        }
        else if (delta_temp < 0.001 && step < 1.0)
        {
            step *= 1.2;
        }

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


    std::cout << "total_time = " << total_time<< " sec\n";
    //std::cout << counter << std::endl;

    graphwidget->setData(time_values, temperature_values, radiusInput->value());
    graphwidget->setTargetTemperature(temperature_gas);
    graphwidget->update_total_time(total_time);

    size_t intersectionIndex = -1;
    for (size_t i = 1; i < temperature_values.size(); ++i)
    {
        if (std::abs(temperature_values[i - 1] - temperature_gas) < 0.001)
        {
            intersectionIndex = i;
            break;
        }
    }

    if (intersectionIndex != -1)
    {
//        std::ofstream file2(filename, std::ios::trunc );
//        if(!file2.is_open())
//            throw std::runtime_error("Can't open file! 1");
//        file2.close();
        std::ofstream file(filename, std::ios::app  );
        if(!file.is_open())
            throw std::runtime_error("Can't open file! 2");

        file <<"#"<< beg_temperature << " -> " << temperature_gas << " for radius = " << std::setw(6) << radius << " for material: " << name_material <<  std::endl;

        file << "Counter"
                  << "," <<  "Time"
                  << "," << "Velocity"
                  << "," <<  "Distance"
                  << "," << "Temperature"
                  << std::endl;


        for(size_t i = 0; i < counter_values.size(); i += 923)
        {

            file << counter_values[i]
                      << "," << std::fixed << std::setprecision(6)
                      << (time_values[i] * total_time * 1.5) / (time_values[intersectionIndex])
                      << "," << velocity_values[i]
                      << ","
                      << g * (time_values[i] * total_time * 1.5) / (time_values[intersectionIndex])
                         * (time_values[i] * total_time * 1.5) / (time_values[intersectionIndex]) / 2
                      << ","<< temperature_values[i]
                      << std::endl;
        }


        file << counter_values[counter_values.size() - 1]
                  << ","  << total_time
                  << "," << velocity_values[velocity_values.size() - 1]
                  << "," << g * total_time * total_time / 2
                  << ","  << temperature_values[intersectionIndex]
                  << std::endl;

        file << "\n\n\n" << std::endl;

        file.close();
    }
    else
    {
        throw std::runtime_error("No intersection found");
    }
}
