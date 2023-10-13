#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidget>
#include <QTreeWidgetItem>
#include <QGraphicsItem>


#include "../../../../kernel/simulator/Simulator.h"
#include "../../../../kernel/simulator/TraceManager.h"
#include "ModelGraphicsScene.h"

QT_BEGIN_NAMESPACE
		namespace Ui {
	class MainWindow;
}

QT_END_NAMESPACE

class MainWindow : public QMainWindow {
	Q_OBJECT

public:
	MainWindow(QWidget *parent = nullptr);
	~MainWindow();

public: // to notify changes
	bool graphicalModelHasChanged() const;
	void setGraphicalModelHasChanged(bool graphicalModelHasChanged);

private slots:
    // actions
	void on_actionEditUndo_triggered();
	void on_actionEditRedo_triggered();
	void on_actionEditFind_triggered();
	void on_actionEditReplace_triggered();
	void on_actionEditCut_triggered();
	void on_actionEditCopy_triggered();
	void on_actionEditPaste_triggered();
	void on_actionEditDelete_triggered();
	void on_actionEditGroup_triggered();
	void on_actionEditUngroup_triggered();

	void on_actionShowRule_triggered();
	void on_actionShowGuides_triggered();
	void on_actionViewConfigure_triggered();

    void on_actionZoom_In_triggered();
    void on_actionZoom_Out_triggered();
    void on_actionZoom_All_triggered();

	void on_actionDrawLine_triggered();
	void on_actionDrawRectangle_triggered();
	void on_actionDrawEllipse_triggered();
	void on_actionDrawText_triggered();
	void on_actionDrawPoligon_triggered();

	void on_actionAnimateSimulatedTime_triggered();
	void on_actionAnimateVariable_triggered();
	void on_actionAnimateExpression_triggered();
	void on_actionAnimateResource_triggered();
	void on_actionAnimateQueue_triggered();
	void on_actionAnimateStation_triggered();
	void on_actionAnimateCounter_triggered();
	void on_actionAnimateEntity_triggered();
	void on_actionAnimateEvent_triggered();
	void on_actionAnimateAttribute_triggered();
	void on_actionAnimateStatistics_triggered();
	void on_actionAnimatePlot_triggered();

	void on_actionSimulationStop_triggered();
	void on_actionSimulationStart_triggered();
	void on_actionSimulationStep_triggered();
	void on_actionSimulationPause_triggered();
	void on_actionSimulationResume_triggered();
	void on_actionSimulationConfigure_triggered();

	void on_actionAboutAbout_triggered();
	void on_actionAboutLicence_triggered();
	void on_actionAboutGetInvolved_triggered();

	void on_actionAlignMiddle_triggered();
	void on_actionAlignTop_triggered();
	void on_actionAlignRight_triggered();
	void on_actionAlignCenter_triggered();
	void on_actionAlignLeft_triggered();

	void on_actionToolsParserGrammarChecker_triggered();
	void on_actionToolsExperimentation_triggered();
	void on_actionToolsOptimizator_triggered();
	void on_actionToolsDataAnalyzer_triggered();

	void on_actionSimulatorPluginManager_triggered();
	void on_actionSimulatorExit_triggered();
	void on_actionSimulatorPreferences_triggered();

	void on_actionModelNew_triggered();
	void on_actionModelOpen_triggered();
	void on_actionModelSave_triggered();
	void on_actionModelClose_triggered();
	void on_actionModelInformation_triggered();
	void on_actionModelCheck_triggered();

	void on_actionConnect_triggered();
	void on_actionComponent_Breakpoint_triggered();


    // widget events
	//void on_textCodeEdit_Model_textChanged();
	void on_tabWidget_Model_tabBarClicked(int index);
	void on_tabWidget_Debug_currentChanged(int index);
	void on_tabWidgetCentral_currentChanged(int index);
	void on_tabWidgetCentral_tabBarClicked(int index);
	void on_tabWidgetModel_currentChanged(int index);
	void on_tabWidgetSimulation_currentChanged(int index);
	void on_tabWidgetReports_currentChanged(int index);
	void on_tabWidgetModelLanguages_currentChanged(int index);

	void on_treeWidget_Plugins_itemDoubleClicked(QTreeWidgetItem *item, int column);
	void on_treeWidgetComponents_itemSelectionChanged();
	void on_treeWidget_Plugins_itemClicked(QTreeWidgetItem *item, int column);

	void on_horizontalSlider_Zoom_valueChanged(int value);
	void on_horizontalSlider_ZoomGraphical_valueChanged(int value);

	void on_checkBox_ShowElements_stateChanged(int arg1);
	void on_checkBox_ShowInternals_stateChanged(int arg1);
	void on_checkBox_ShowRecursive_stateChanged(int arg1);
	void on_checkBox_ShowLevels_stateChanged(int arg1);

	void on_pushButton_Breakpoint_Insert_clicked();
	void on_pushButton_Breakpoint_Remove_clicked();
	void on_pushButton_Export_clicked();

	void on_graphicsView_rubberBandChanged(const QRect &viewportRect, const QPointF &fromScenePoint, const QPointF &toScenePoint);
	void on_TextCodeEditor_textChanged();



private: // VIEW

private: // trace handlers
	void _simulatorTraceHandler(TraceEvent e);
	void _simulatorTraceErrorHandler(TraceErrorEvent e);
	void _simulatorTraceSimulationHandler(TraceSimulationEvent e);
	void _simulatorTraceReportsHandler(TraceEvent e);
private: // simulator event handlers
	void _onModelCheckSuccessHandler(ModelEvent* re);
	void _onReplicationStartHandler(SimulationEvent* re);
	void _onSimulationStartHandler(SimulationEvent* re);
	void _onSimulationPausedHandler(SimulationEvent* re);
	void _onSimulationResumeHandler(SimulationEvent* re);
	void _onSimulationEndHandler(SimulationEvent* re);
	void _onProcessEventHandler(SimulationEvent* re);
	void _onEntityCreateHandler(SimulationEvent* re);
	void _onEntityRemoveHandler(SimulationEvent* re);
private: // model Graphics View handlers
    void _onSceneMouseEvent(QGraphicsSceneMouseEvent* mouseEvent);
    void _onSceneWheelInEvent();
    void _onSceneWheelOutEvent();
    void _onSceneGraphicalModelEvent(GraphicalModelEvent* event);
private: // QGraphicsScene Slots
	void sceneChanged(const QList<QRectF> &region);
	void sceneFocusItemChanged(QGraphicsItem *newFocusItem, QGraphicsItem *oldFocusItem, Qt::FocusReason reason);
	//void sceneRectChanged(const QRectF &rect);
	void sceneSelectionChanged();
private: // Similar to QGraphicsScene Slots
	void sceneGraphicalModelChanged();
private: // simulator related
	void _setOnEventHandlers();
	void _insertPluginUI(Plugin* plugin);
	void _insertFakePlugins();
	bool _setSimulationModelBasedOnText();
	bool _createModelImage();
	std::string _adjustDotName(std::string name);
	void _insertTextInDot(std::string text, unsigned int compLevel, unsigned int compRank, std::map<unsigned int, std::map<unsigned int, std::list<std::string>*>*>* dotmap, bool isNode = false);
	void _recursiveCreateModelGraphicPicture(ModelDataDefinition* componentOrData, std::list<ModelDataDefinition*>* visited, std::map<unsigned int, std::map<unsigned int, std::list<std::string>*>*>* dotmap);
	void _actualizeModelCppCode();
	std::string _addCppCodeLine(std::string line, unsigned int indent = 0);
private: // view
	void _initModelGraphicsView();
	void _initUiForNewModel(Model* m);
	void _actualizeActions();
	void _actualizeTabPanes();
	void _actualizeModelSimLanguage();
	void _actualizeModelTextHasChanged(bool hasChanged);
	void _actualizeSimulationEvents(SimulationEvent * re);
	void _actualizeDebugVariables(bool force);
	void _actualizeDebugEntities(bool force);
	void _actualizeDebugBreakpoints(bool force);
	void _actualizeModelComponents(bool force);
	void _actualizeModelDataDefinitions(bool force);
	void _actualizeGraphicalModel(SimulationEvent * re);
	void _insertCommandInConsole(std::string text);
	void _clearModelEditors();
	void _gentle_zoom(double factor);
	void _showMessageNotImplemented();
	void _recursivalyGenerateGraphicalModelFromModel(ModelComponent* component, List<ModelComponent*>* visited, std::map<ModelComponent*,GraphicalModelComponent*>* map, int *x, int *y, int *ymax, int sequenceInline);
	void _generateGraphicalModelFromModel();
	//bool _checkStartSimulation();
private: // graphical model persistence
	bool _saveGraphicalModel(std::string filename);
	Model* _loadGraphicalModel(std::string filename);
private:
	QColor myrgba(uint64_t color); // TODO: Should NOT be here, but in UtilGUI.h, but then it generates multiple definitions error
	static std::string dotColor(uint64_t color); // TODO: Should NOT be here, but in UtilGUI.h, but then it generates multiple definitions error
private: // interface and model main elements to join
	Ui::MainWindow *ui;
	Simulator* simulator;
private: // misc useful
	bool _textModelHasChanged = false;
	bool _graphicalModelHasChanged = false;
	bool _modelWasOpened = false;
	QString _modelfilename;
	std::map<std::string /*category*/,QColor>* _pluginCategoryColor = new std::map<std::string,QColor>();
    int _zoomValue; // todo should be set for each open graphical model, such as view rect, etc

    struct COPIED_OCCUPIED {
        Plugin *plugin;
        ModelComponent *component;
        QPointF position;
        QColor color;
        bool cut;
    } _copied;


private:

	const struct TABINDEXES_STRUC {
		const int TabCentralModelIndex = 0;
		const int TabCentralSimulationIndex = 1;
		const int TabCentralReportsIndex = 2;
		//
		const int TabModelSimLangIndex = 0;
		const int TabModelCppCodeIndex = 1;
		const int TabModelDiagramIndex = 2;
		const int TabModelComponentsIndex = 3;
		//
		const int TabModelDataDefinitionsIndex = 4;
		const int TabSimulationBreakpointsIndex = 0;
		const int TabSimulationVariablesIndex = 1;
		const int TabSimulationEntitiesIndex = 2;
		const int TabSimulationTracesIndex = 3;
		const int TabSimulationEventsIndex = 4;
		//
		const int TabReportReportIndex = 0;
		const int TabReportResultIndex = 1;
		const int TabReportPlotIndex = 2;
	} CONST;
	//CodeEditor* textCodeEdit_Model;
};
#endif // MAINWINDOW_H
