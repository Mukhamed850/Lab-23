#include "widget.h"
#include <QPainterPath>
#include <QDebug>
#include <QMenu>
#include <QAction>
#include <QRandomGenerator>

Widget::Widget(QWidget *parent): QWidget(parent) {
    this->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, SIGNAL(customContextMenuRequested(const QPoint &)),
        this, SLOT(ShowContextMenu(const QPoint &)));
}

void Widget::paintEvent(QPaintEvent *) {
    drawRectangle();
    drawCircle();
}

void Widget::drawRectangle() {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    for(Shape &shape : rectangles_list) {
        painter.save();
        rotate(painter, shape.path, shape.angle);
        QPainterPathStroker stroker;
        stroker.setWidth(border_thikness);
        QPainterPath borderedPath = stroker.createStroke(shape.path);
        if(shape.path.contains(mapFromGlobal(QCursor::pos()))) {
            shape.transparency = 0.5;
            shape.border_color = shape.color;
            shape.border_color.setAlphaF(0.5);
        }
        else if(borderedPath.contains(mapFromGlobal(QCursor::pos()))) {
            shape.transparency = 1.0;
            shape.border_color = Qt::black;
        }
        else {
            shape.transparency = 1.0;
            shape.border_color = shape.color;
            shape.border_color.setAlphaF(1.0);
        }
        QColor color = shape.color;
        color.setAlphaF(shape.transparency);
        painter.setBrush(color);
        painter.setPen(QPen(shape.border_color, border_thikness));
        painter.drawPath(shape.path);
        painter.restore();
        update();
    }
}

void Widget::drawCircle() {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    for(Shape &shape : circles_list) {
        painter.save();
        rotate(painter, shape.path, shape.angle);
        QPainterPathStroker stroker;
        stroker.setWidth(border_thikness);
        QPainterPath borderedPath = stroker.createStroke(shape.path);
        if(shape.path.contains(mapFromGlobal(QCursor::pos()))) {
            shape.transparency = 0.5;
            shape.border_color = shape.color;
            shape.border_color.setAlphaF(0.5);
        }
        else if(borderedPath.contains(mapFromGlobal(QCursor::pos()))) {
            shape.transparency = 1.0;
            shape.border_color = Qt::black;
        }
        else {
            shape.transparency = 1.0;
            shape.border_color = shape.color;
            shape.border_color.setAlphaF(1.0);
        }
        QColor color = shape.color;
        color.setAlphaF(shape.transparency);
        painter.setBrush(color);
        painter.setPen(QPen(shape.border_color, border_thikness));
        painter.drawPath(shape.path);
        painter.restore();
        update();
    }
}

void Widget::addNewRectangle() {
    Shape newRectangle;
    newRectangle.path.addRect(10, 10, 90, 90);
    newRectangle.color = QColor(QRandomGenerator::global()->bounded(100, 256), QRandomGenerator::global()->bounded(100, 256), QRandomGenerator::global()->bounded(100, 256));
    rectangles_list.append(newRectangle);
    update();
}

void Widget::addNewCircle() {
    Shape newCircle;
    newCircle.path.addEllipse(QPoint(55, 55), 50, 50);
    newCircle.color = QColor(QRandomGenerator::global()->bounded(100, 256), QRandomGenerator::global()->bounded(100, 256), QRandomGenerator::global()->bounded(100, 256));
    circles_list.append(newCircle);
    update();
}

int Widget::inBorderRectangle(const QPoint& clickPoint) {
    for (int i = 0; i < rectangles_list.size(); ++i) {
        QPainterPathStroker stroker;
        stroker.setWidth(border_thikness);
        QPainterPath borderedPath = stroker.createStroke(rectangles_list[i].path);
        if (borderedPath.contains(clickPoint))
            return i;
    }
    return -1;
}

int Widget::inBorderCircle(const QPoint& clickPoint) {
    for (int i = 0; i < circles_list.size(); ++i) {
        QPainterPathStroker stroker;
        stroker.setWidth(border_thikness);
        QPainterPath borderedPath = stroker.createStroke(circles_list[i].path);
        if (borderedPath.contains(clickPoint))
            return i;
    }
    return -1;
}

void Widget::ShowContextMenu(const QPoint &pos) {
    if(inBorderRectangle(pos) != -1)
        shape_type = 1;
    else if(inBorderCircle(pos) != -1)
        shape_type = 2;
    else
        shape_type = 0;

    if(shape_type) {
        QMenu contextMenu(tr("Контекстное меню"), this);

        QAction action1("Повернуть на 15° по часовой стрелке", this);
        connect(&action1, SIGNAL(triggered()), this, SLOT(changeAngle()));

        QAction action2("Увеличить длину и ширину на 20%", this);
        connect(&action2, SIGNAL(triggered()), this, SLOT(scale()));

        contextMenu.addAction(&action1);
        contextMenu.addAction(&action2);
        contextMenu.exec(mapToGlobal(pos));
    }
    else {
        QMenu contextMenu(tr("Контекстное меню"), this);

        QAction action3("Добавить прямоугольник", this);
        connect(&action3, SIGNAL(triggered()), this, SLOT(addNewRectangle()));

        QAction action4("Добавить круг", this);
        connect(&action4, SIGNAL(triggered()), this, SLOT(addNewCircle()));

        contextMenu.addAction(&action3);
        contextMenu.addAction(&action4);
        contextMenu.exec(mapToGlobal(pos));
    }
}

void Widget::mousePressEvent(QMouseEvent *event) {
    QPoint clickPoint = event->pos();
    if(inBorderRectangle(clickPoint) != -1) {
        shape_type = 1;
        current_rectangle_index = inBorderRectangle(clickPoint);
    }
    else if(inBorderCircle(clickPoint) != -1) {
        shape_type = 2;
        current_circle_index = inBorderCircle(clickPoint);
    }
    else
        shape_type = 0;

    if(shape_type && event->button() == Qt::LeftButton) {
        lastPoint = clickPoint;
        offset_ = QPoint(0, 0);
    }
}

void Widget::mouseMoveEvent(QMouseEvent *event) {
    if (shape_type) {
        int delta_x = event->pos().x() - lastPoint.x();
        int delta_y = event->pos().y() - lastPoint.y();
        lastPoint = event->pos();
        offset_ += QPoint(delta_x, delta_y);
        switch (shape_type) {
        case 1:
            rectangles_list[current_rectangle_index].path.translate(offset_);
            break;
        case 2:
            circles_list[current_circle_index].path.translate(offset_);
            break;
        default:
            break;
        }
        offset_ = QPoint(0, 0);
        update();
    }
}

void Widget::mouseReleaseEvent(QMouseEvent *event) { if (event->button() == Qt::LeftButton && shape_type) shape_type = 0; }

void Widget::changeAngle() {
    switch (shape_type) {
    case 1:
        rectangles_list[current_rectangle_index].angle += 15;
        break;
    case 2:
        circles_list[current_circle_index].angle += 15;
        break;
    default:
        break;
    }
}

void Widget::rotate(QPainter &painter, QPainterPath& path, const qreal angle) {
    painter.translate(path.boundingRect().center());
    painter.rotate(angle);
    painter.translate(-path.boundingRect().center());
}

void Widget::scale() {
    QTransform transform;
    switch (shape_type) {
    case 1:
        transform.translate(rectangles_list[current_rectangle_index].path.boundingRect().center().x(),
                            rectangles_list[current_rectangle_index].path.boundingRect().center().y());
        transform.scale(1.2, 1.2);
        transform.translate(-rectangles_list[current_rectangle_index].path.boundingRect().center().x(),
                            -rectangles_list[current_rectangle_index].path.boundingRect().center().y());
        rectangles_list[current_rectangle_index].path = transform.map(rectangles_list[current_rectangle_index].path);
        rectangles_list[current_rectangle_index].path = rectangles_list[current_rectangle_index].path.simplified();
        break;
    case 2:
        transform.translate(circles_list[current_circle_index].path.boundingRect().center().x(),
                            circles_list[current_circle_index].path.boundingRect().center().y());
        transform.scale(1.2, 1.2);
        transform.translate(-circles_list[current_circle_index].path.boundingRect().center().x(),
                            -circles_list[current_circle_index].path.boundingRect().center().y());
        circles_list[current_circle_index].path = transform.map(circles_list[current_circle_index].path);
        circles_list[current_circle_index].path = circles_list[current_circle_index].path.simplified();
        break;
    default:
        break;
    }
    update();
}
