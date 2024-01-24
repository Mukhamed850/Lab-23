#include <QWidget>
#include <QPainter>
#include <QMouseEvent>

struct Shape {
    QPainterPath path;
    QColor       color;
    QColor       border_color = color;
    qreal        angle        = 0;
    qreal        transparency = 1.0;
};

class Widget: public QWidget {
    Q_OBJECT
    public:
        Widget(QWidget* p = nullptr);
        void paintEvent(QPaintEvent *p = nullptr);

    protected:
        void mousePressEvent(QMouseEvent *event)   override;
        void mouseMoveEvent(QMouseEvent *event)    override;
        void mouseReleaseEvent(QMouseEvent *event) override;

    private:
        void drawRectangle();
        void drawCircle();
        void rotate(QPainter& painter, QPainterPath& path, const qreal angle);
        int inBorderRectangle(const QPoint& p);
        int inBorderCircle(const    QPoint& p);

        QPoint lastPoint;
        QPoint offset_;
        int shape_type;
        int current_rectangle_index = 0;
        int current_circle_index    = 0;
        int border_thikness         = 2;

        QList<Shape> rectangles_list = {};
        QList<Shape> circles_list    = {};

    public slots:
        void ShowContextMenu(const QPoint &pos);
        void changeAngle();
        void scale();
        void addNewRectangle();
        void addNewCircle();
};
