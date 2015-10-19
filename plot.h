#ifndef PLOT_H
#define PLOT_H

#include <QString>
#include <QPointF>
#include <qwt_plot.h>
#include <qwt_plot_legenditem.h>
#include <qwt_plot_canvas.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_grid.h>
#include <qwt_plot_marker.h>
#include <qwt_interval.h>
#include <qwt_plot_legenditem.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_magnifier.h>
#include <qwt_plot_panner.h>
#include <qwt_plot_zoomer.h>

#define CURVE_MAX       15

class Plot : public QwtPlot
{
public:
    Plot(QWidget *parent = NULL);
    ~Plot();

    void set_title(QString x_title, QString y_title);
    void set_x_scale(double min, double max);
    void set_y_scale(double min, double max);
    int  insert_curve(QString title,int colorIndex);
    void del_curve(int id);
    bool show_curve(int id, bool show);
    int  get_curves();
    void add_point(int id, QPointF p);
    QVector<QPointF> get_points(int id);
    void set_marker(int id, double x, double y, QString str);
    void clear();
    void clear_all();
    void replot();

private:
    void set_canvas();
    void set_grid();
    void set_marker();
    void set_legenditem();

private:
    QwtPlot *plot;
    QwtPlotCanvas *canvas;
    QwtPlotGrid *grid;
    QwtPlotLegendItem *legend_item;

    // 曲线
    struct
    {
        bool is_attach;
        QVector<QPointF> data;
        QwtPlotCurve *curve;
        QwtPlotMarker *marker;
    } curve;

    QwtPlotMagnifier *magnifier;
    QwtPlotPanner *panner;
    QwtPlotZoomer *zoomer;

    QwtInterval interval_x;
    QwtInterval interval_y;
};

#endif // PLOT_H
