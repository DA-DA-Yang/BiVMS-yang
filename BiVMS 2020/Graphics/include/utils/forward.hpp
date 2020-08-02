/*********************************************************************
 This file is part of VDM, an app. for visual deformation measurement.
		   Copyright(C) 2019-now, SHU, all rights reserved.
*********************************************************************/
#pragma once
#include <type_traits>

class MainWnd;
class NewDialog;
class AboutDialog;
class QAction;
class QMenu;
class QTimer;
class QString;
class QWidget;
class QStandardItem;
class QGraphicsPixmapItem;
class QGraphicsLineItem;
class StartWidget;
class Deflection2dMenu;
class Deflection2dToolbar;
class Deflection2dTab;
class ScalableGraphicsView;
class CameraViewCenter;
class EditableGraphicsScene;
class PlainChartWidget;
class storage;
class MyThread;
class QTcpServer;
class QTcpSocket;
class acqisitionset;
class cameraviewcenter3d;
class syncwidget;
class QProgressBar;
class Pointselect;
class Calibrate_with_inclinometer;
class twocameramatch;
class QFile;
class AssitCalib;

namespace detail { class _Project_base; }
using Project = detail::_Project_base;

enum class ProjectTag {
	DEF2D = 0, //2d deflection measurement
	DEF3D = 1, //3d deflection measurement
	GEN2D = 2, //general 2d measurement
	GEN3D = 3, //general 3d measurement
	IMACQ = 4, //imge acquization
	UNDEF = 99, //undefined project tag
};

template<ProjectTag _Tag> class DyUiWrapper {};

using Deflection2D = DyUiWrapper<ProjectTag::DEF2D>;
using Deflection3D = DyUiWrapper<ProjectTag::DEF3D>;

using ScalGraphicsView = ScalableGraphicsView;

template<typename Str>
inline constexpr auto is_stdstring_v = std::is_same_v<Str, std::string>;

template<typename Str>
inline constexpr auto is_qstring_v = std::is_same_v<Str, QString>;

template<typename Str>
inline decltype(auto) std_str(const Str& str) noexcept {
	if constexpr (is_qstring_v<Str>) {
		return str.toStdString();
	}
	else return str;
}
template<typename Str>
inline auto q_str(const Str& str) noexcept {
	if constexpr (is_stdstring_v<Str>) {
		return QString::fromStdString(str);
	}
	else return str;
}