TEMPLATE = lib
CONFIG += staticlib

TARGET = qwt
!android|!equals(QT_MAJOR_VERSION, 4) {
DESTDIR = lib
}

!equals(QT_MAJOR_VERSION, 4) {
    QT += widgets concurrent
    DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x040600
}

# SimpleQWT version 6.0
# targeted at projects that want only core qwt
# targets reduced computational and memory footprint compared to full-fat version

# forked from QWT "6.0.0-svn"
# fixes and production-ready antialiasing
# fixes for stable axes/canvas while panning/zooming
# some fixes pulled from 6.1
# Qt5 fixes, legacy cleanup, simplification and streamlining
# some fixes for plot renderer for wysiwyg
# removed obscure uncommon parts

HEADERS += \
    qwt_abstract_scale_draw.h \
    qwt_color_map.h \
    qwt_interval.h \
    qwt_dyngrid_layout.h \
    qwt_math.h \
    qwt_painter.h \
    qwt_scale_div.h \
    qwt_scale_draw.h \
    qwt_scale_engine.h \
    qwt_scale_map.h \
    qwt_spline.h \
    qwt_symbol.h \
    qwt_text_engine.h \
    qwt_text_label.h \
    qwt_text.h

SOURCES += \
    qwt_abstract_scale_draw.cpp \
    qwt_color_map.cpp \
    qwt_interval.cpp \
    qwt_dyngrid_layout.cpp \
    qwt_math.cpp \
    qwt_painter.cpp \
    qwt_scale_div.cpp \
    qwt_scale_draw.cpp \
    qwt_scale_map.cpp \
    qwt_spline.cpp \
    qwt_text_engine.cpp \
    qwt_text_label.cpp \
    qwt_text.cpp \
    qwt_scale_engine.cpp \
    qwt_symbol.cpp

HEADERS += \
    qwt_legend.h \
    qwt_legend_item.h \
    qwt_legend_itemmanager.h \
    qwt_plot.h \
    qwt_plot_renderer.h \
    qwt_plot_curve.h \
    qwt_plot_dict.h \
    qwt_plot_directpainter.h \
    qwt_plot_grid.h \
    qwt_plot_item.h \
    qwt_plot_layout.h \
    qwt_plot_marker.h \
    qwt_plot_rasteritem.h \
    qwt_plot_spectrogram.h \
    qwt_plot_seriesitem.h \
    qwt_plot_canvas.h \
    qwt_raster_data.h \
    qwt_series_data.h \
    qwt_scale_widget.h

SOURCES += \
    qwt_legend.cpp \
    qwt_legend_item.cpp \
    qwt_plot.cpp \
    qwt_plot_renderer.cpp \
    qwt_plot_axis.cpp \
    qwt_plot_curve.cpp \
    qwt_plot_dict.cpp \
    qwt_plot_directpainter.cpp \
    qwt_plot_grid.cpp \
    qwt_plot_item.cpp \
    qwt_plot_spectrogram.cpp \
    qwt_plot_seriesitem.cpp \
    qwt_plot_marker.cpp \
    qwt_plot_layout.cpp \
    qwt_plot_canvas.cpp \
    qwt_plot_rasteritem.cpp \
    qwt_raster_data.cpp \
    qwt_series_data.cpp \
    qwt_scale_widget.cpp

HEADERS += \
    qwt_abstract_slider.h \
    qwt_abstract_scale.h \
    qwt_double_range.h \
    qwt_slider.h \
    qwt_thermo.h

SOURCES += \
    qwt_abstract_slider.cpp \
    qwt_abstract_scale.cpp \
    qwt_double_range.cpp \
    qwt_slider.cpp \
    qwt_thermo.cpp
