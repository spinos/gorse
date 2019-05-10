
#include <QtWidgets>

#include "MainWindow.h"
#include "widgets/AssetPalette.h"
#include "VachellScene.h"
#include "widgets/AttribEditor.h"
#include "widgets/RenderWidget.h"
#include <qt_graph/SceneGraph.h>
#include <qt_graph/GlyphOps.h>

MainWindow::MainWindow()
{
    createActions();
    //createStatusBar();
    QDockWidget *assetDock = new QDockWidget(tr("Assets"), this);
    assetDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    m_palette = new AssetPalette(assetDock);
    assetDock->setWidget(m_palette);
    addDockWidget(Qt::LeftDockWidgetArea, assetDock);
    viewMenu->addAction(assetDock->toggleViewAction());

    m_scene = new VachellScene(this);
    m_scene->setAssetCollection(m_palette->assetCollector());
    m_scene->initializeGraphics();
    m_renderView = new RenderWidget(m_scene, this);
    setCentralWidget(m_renderView);

    alo::GlyphOps::loadAttributePreset(":/mimes/attribpreset.json");

    QDockWidget *attrDock = new QDockWidget(tr("Attributes"), this);
    attrDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    m_editor = new AttribEditor(m_scene, attrDock);
    attrDock->setWidget(m_editor);
    addDockWidget(Qt::RightDockWidgetArea, attrDock);
    viewMenu->addAction(attrDock->toggleViewAction());

    QDockWidget *grDock = new QDockWidget(tr("Graph Editor"), this);
    grDock->setAllowedAreas(Qt::AllDockWidgetAreas);
    m_graphView = new alo::SceneGraph(m_scene, grDock);
    grDock->setWidget(m_graphView);
    addDockWidget(Qt::BottomDockWidgetArea, grDock);
    //grDock->setFloating(true);
    viewMenu->addAction(grDock->toggleViewAction());

    qRegisterMetaType<alo::CameraEvent>();
    
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
	connect(m_editor, &AttribEditor::sendAttribChanged, 
			m_scene, &VachellScene::recvAttribChanged );

	connect(m_scene, &VachellScene::sendSelectGlyph, 
			m_editor, &AttribEditor::recvSelectGlyph );

    connect(m_scene, &VachellScene::sendUpdateScene,
            m_renderView, &RenderWidget::recvUpdateScene );
	
    connect(m_renderView, &RenderWidget::cameraChanged,
            m_scene, &VachellScene::recvCameraChanged );
					
    connect(m_renderView, &RenderWidget::requestBound,
            m_scene, &VachellScene::recvRequestBound );

    //connect(m_renderView, &RenderWidget::preRenderRestart,
    //        m_scene, &VachellScene::recvPreRenderRestart );
			
    connect(m_scene, &VachellScene::sendBound,
            m_renderView, &RenderWidget::recvBound );

    connect(m_scene, &VachellScene::sendFocusCameraOn,
            m_renderView, &RenderWidget::recvFocusCameraOn );
    
#else
    
#endif

    setWindowTitle(tr("Vachellia"));

    setUnifiedTitleAndToolBarOnMac(true);
}

void MainWindow::save()
{
    QApplication::setOverrideCursor(Qt::WaitCursor);
/// save 
    QApplication::restoreOverrideCursor();

    //statusBar()->showMessage(tr("Saved '%1'").arg(fileName), 2000);
}

void MainWindow::about()
{
   QMessageBox::about(this, tr("About Acacia"),
            tr("Some <b>Doc</b> here "));
}

void MainWindow::createActions()
{
    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));
    /*QToolBar *fileToolBar = addToolBar(tr("File"));

    QAction *saveAct = new QAction(saveIcon, tr("&Save..."), this);
    saveAct->setShortcuts(QKeySequence::Save);
    saveAct->setStatusTip(tr("Save the current form letter"));
    connect(saveAct, &QAction::triggered, this, &MainWindow::save);
    fileMenu->addAction(saveAct);
    
    fileMenu->addSeparator();*/

    QAction *quitAct = fileMenu->addAction(tr("&Quit"), this, &QWidget::close);
    quitAct->setShortcuts(QKeySequence::Quit);
    quitAct->setStatusTip(tr("Quit the application"));

    viewMenu = menuBar()->addMenu(tr("&View"));
    displayMenu = menuBar()->addMenu(tr("&Display"));

    QIcon solidIcon(":/images/solid_ball.png");
    m_displaySolidAct = new QAction(solidIcon, tr("&Solid"), this);
    m_displaySolidAct->setCheckable(true);
    m_displaySolidAct->setChecked(false);
    QIcon wireIcon(":/images/wire_ball.png");
    m_displayWireAct = new QAction(wireIcon, tr("&Wireframe"), this);
    m_displayWireAct->setCheckable(true);
    m_displayWireAct->setChecked(true);

    displayMenu->addAction(m_displaySolidAct);
    displayMenu->addAction(m_displayWireAct);

#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
    //connect(m_displaySolidAct, &QAction::triggered,
    //        this, &MainWindow::recvDisplaySolidChanged );   
    //connect(m_displayWireAct, &QAction::triggered,
    //        this, &MainWindow::recvDisplayWireChanged );   
#endif

    menuBar()->addSeparator();

    QMenu *helpMenu = menuBar()->addMenu(tr("&Help"));

    QAction *aboutAct = helpMenu->addAction(tr("&About"), this, &MainWindow::about);
    aboutAct->setStatusTip(tr("Show the application's About box"));

}

void MainWindow::createStatusBar()
{
    statusBar()->showMessage(tr("Ready"));
}

void MainWindow::recvDisplaySolidChanged(bool checked)
{
    if(!checked) {
        m_displaySolidAct->setChecked(true);
        return;
    }
    m_displayWireAct->setChecked(false);
    //m_renderView->setDisplaySolid();
}

void MainWindow::recvDisplayWireChanged(bool checked)
{
    if(!checked) {
        m_displayWireAct->setChecked(true);
        return;
    }
    m_displaySolidAct->setChecked(false);
    //m_renderView->setDisplayWire();
}
