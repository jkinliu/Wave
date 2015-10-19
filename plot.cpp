#include "plot.h"

const char *colors[] =
{
    "LightSalmon",
    "Yellow",
    "Fuchsia",
    "SteelBlue",
    "PaleGreen",
    "PaleTurquoise",
    "Cornsilk",
    "HotPink",
    "Peru",
    "Maroon",
    "Red",
    "Green",
};

Plot::Plot(QWidget *parent)
{
    if (parent)
        plot = (QwtPlot *)parent;
    else
        plot = new QwtPlot();

    plot->setAutoReplot(false);

    set_canvas();
    set_grid();
    set_legenditem();

    //for (int i=0; i<CURVE_MAX; i++)
    {
        curve.is_attach = false;
        curve.data.clear();
        curve.curve  = NULL;
        curve.marker = NULL;
    }
}

Plot::~Plot()
{
    delete plot;
}

/**
 * @brief 画布
 */
void Plot::set_canvas()
{
    canvas = new QwtPlotCanvas();

    canvas->setFocusIndicator(QwtPlotCanvas::CanvasFocusIndicator);
    canvas->setFocusPolicy(Qt::StrongFocus);
    canvas->setPalette(Qt::black);
    canvas->setBorderRadius(10);
    plot->setAxisScale(QwtPlot::xBottom, 0, 10);
    plot->setAxisScale(QwtPlot::yLeft, -1, 1);
    plot->setCanvas(canvas);

    magnifier   = new QwtPlotMagnifier(canvas);   // 滚轮放大/缩小

    //鼠标左键选择区域放大，右键还原
    zoomer = new QwtPlotZoomer(canvas);
    zoomer->setRubberBandPen( QColor( Qt::white ) );
    zoomer->setTrackerPen( QColor( Qt::red ) );
    zoomer->setMousePattern(QwtEventPattern::MouseSelect2,Qt::RightButton, Qt::ControlModifier );
    zoomer->setMousePattern(QwtEventPattern::MouseSelect3,Qt::RightButton );
//    panner      = new QwtPlotPanner(canvas);      // 鼠标左键平移
}

/**
 * @brief 网格
 */
void Plot::set_grid()
{
    grid = new QwtPlotGrid();

    grid->enableXMin( true );
    grid->setMajorPen( Qt::gray, 0, Qt::DotLine );
    grid->setMinorPen( Qt::darkGray, 0, Qt::DotLine );
    grid->attach(plot);
}

/**
 * @brief 标记
 */
void Plot::set_marker(int id, double x, double y, QString str)
{
    if (id >= 0 && id < CURVE_MAX)
    {
        QwtPlotMarker *marker = curve.marker;

        marker->setValue( x, y );
        marker->setLabel(str);
        marker->setLinePen( QColor(colors[id]), 0.0, Qt::SolidLine );
        marker->setLineStyle( QwtPlotMarker::VLine );
        marker->setLabelAlignment(Qt::AlignRight | Qt::AlignTop);
        marker->attach( plot );

        replot();
    }
}

/**
 * @brief 设置图例
 */
void Plot::set_legenditem()
{
    legend_item = new QwtPlotLegendItem();

    legend_item->setRenderHint( QwtPlotItem::RenderAntialiased );

    QColor color( Qt::white );
    legend_item->setTextPen( color );
    legend_item->setBorderPen( color );

    QColor c( Qt::gray );
    c.setAlpha( 100 );
    legend_item->setBackgroundBrush( c );

    legend_item->setMaxColumns(1);
    legend_item->setAlignment(Qt::Alignment(Qt::AlignRight | Qt::AlignTop));
    legend_item->setBackgroundMode(QwtPlotLegendItem::BackgroundMode(0));

    legend_item->setBorderRadius( 8 );
    legend_item->setMargin( 4 );
    legend_item->setSpacing( 2 );
    legend_item->setItemMargin( 0 );

    legend_item->attach(plot);
}

/**
 * @brief 设置X轴和Y轴标题
 * @param x_title   X轴标题
 * @param y_title   Y轴标题
 */
void Plot::set_title(QString x_title, QString y_title)
{
    if (!x_title.isEmpty())
        plot->setAxisTitle(QwtPlot::xBottom, x_title);

    if (!y_title.isEmpty())
        plot->setAxisTitle(QwtPlot::yLeft, y_title);
}

/**
 * @brief 设置X轴量程
 * @param min
 * @param max
 */
void Plot::set_x_scale(double min, double max)
{
    if (max > min)
    {
        interval_x = QwtInterval(min, max);
        plot->setAxisScale(QwtPlot::xBottom, min, max);
    }

    replot();
}

/**
 * @brief 设置Y轴量程
 * @param min
 * @param max
 */
void Plot::set_y_scale(double min, double max)
{
    if (max > min)
    {
        interval_y = QwtInterval(min, max);
        plot->setAxisScale(QwtPlot::yLeft, min, max);
    }

    replot();
}

/**
 * @brief 插入曲线
 * @param title 曲线标题
 */
int Plot::insert_curve(QString title,int colorIndex)
{
//    int i;

//    for (i=0; i<CURVE_MAX; i++)
    {
        if (!curve.is_attach)
        {
            QwtPlotCurve *tmp = new QwtPlotCurve();

            tmp->setTitle(title);
            tmp->setPen(QColor(colors[colorIndex]), 2);
            tmp->attach(plot);

            curve.curve = tmp;
            curve.is_attach = true;
            curve.marker = new QwtPlotMarker();
        }

    }

    replot();
    return 0;
//    return i<CURVE_MAX ? i : -1;
}

/**
 * @brief 删除曲线
 * @param id    曲线ID
 */
void Plot::del_curve(int id)
{
    if (id >= 0 && id < CURVE_MAX)
    {
        if (curve.is_attach)
        {
            curve.curve->detach();

            curve.is_attach = false;
            curve.data.clear();

            delete curve.curve;
            delete curve.marker;

            curve.curve  = NULL;
            curve.marker = NULL;
        }
    }

    replot();
}

/**
 * @brief   显示或隐藏曲线
 */
bool Plot::show_curve(int id, bool show)
{
    bool ret = false;

    if (id >= 0 && id < CURVE_MAX)
    {
        if (show)
            curve.curve->attach(plot);
        else
            curve.curve->detach();

        replot();

        ret = true;
    }

    return ret;
}

/**
 * @brief   曲线数
 */
int Plot::get_curves()
{
    int i;
    int cnt = 0;

//    for (i=0; i<CURVE_MAX; i++)
    {
        if (curve.is_attach)
            cnt++;
    }

    return cnt;
}

/**
 * @brief 曲线加入点
 * @param id    曲线ID
 * @param p     点
 */
void Plot::add_point(int id, QPointF p)
{
    if (id >= 0 && id < CURVE_MAX)
    {
        curve.data += p;
        curve.curve->setSamples(curve.data);
    }

    replot();
}

QVector<QPointF> Plot::get_points(int id)
{
    QVector<QPointF> points;

    points.clear();
    if (id >= 0 && id < CURVE_MAX)
        points = curve.data;

    return points;
}

/**
 * @brief 清屏
 * @param id
 */
void Plot::clear()
{
//    if (id >= 0 && id < CURVE_MAX)
    {
        curve.data.clear();

        //curve[id].curve->setSamples(curve[id].data);
        if (curve.marker)
            curve.marker->setLineStyle(QwtPlotMarker::NoLine);
    }

    //replot();
}

/**
 * @brief   清plot
 */
void Plot::clear_all()
{
//    int i;
//    for (i=0; i<CURVE_MAX; i++)
    {
        clear();
        //curve[i].data.clear();
    }
}

void Plot::replot()
{
    plot->replot();
}
