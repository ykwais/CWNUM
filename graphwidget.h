//
// Created by fedor on 31.10.2024.
//

#ifndef CWNUM_GRAPHWIDGET_H
#define CWNUM_GRAPHWIDGET_H

#include <QWidget>
#include <QPainter>
#include <vector>


//QT_BEGIN_NAMESPACE
//namespace Ui { class graphwidget; }
//QT_END_NAMESPACE

class graphwidget : public QWidget {
Q_OBJECT

public:
    explicit graphwidget(QWidget *parent = nullptr);

    void setData(const std::vector<long double>& time, const std::vector<long double>& temperature);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    std::vector<long double> timeValues;
    std::vector<long double> temperatureValues;

//private:
//    Ui::graphwidget *ui;
};


#endif //CWNUM_GRAPHWIDGET_H
