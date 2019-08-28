
#include <QtWidgets>

#include "MainWindow.h"
#include "AssetPalette.h"
#include "AcaciaScene.h"
#include "AttribEditor.h"
#include "GLWidget.h"
#include <qt_graph/SceneGraph.h>

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

    m_scene = new AcaciaScene(this);
    m_scene->setAssetCollection(m_palette->assetCollector());
    m_scene->initializeGraphics();
    m_glview = new GLWidget(m_scene, this);
    setCentralWidget(m_glview);

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

    connect(m_scene, SIGNAL(sendSelectGlyph(bool)), 
    m_editor, SLOT(recvSelectGlyph(bool)));

    qRegisterMetaType<alo::CameraEvent>();
    
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
    connect(m_editor, &AttribEditor::sendAttribChanged, 
            m_scene, &AcaciaScene::recvAttribChanged);

    connect(m_scene, &AcaciaScene::sendUpdateDrawable,
            m_glview, &GLWidget::recvAttribChanged);
    
    connect(m_glview, &alo::View3DWidget::cameraChanged,
            m_scene, &AcaciaScene::recvCameraChanged );
    
    connect(m_glview, &alo::View3DWidget::requestBound,
            m_scene, &AcaciaScene::recvRequestBound );
    connect(m_scene, &AcaciaScene::sendBound,
            m_glview, &alo::View3DWidget::recvBound );
#else
    connect(m_glview, SIGNAL(cameraChanged(alo::CameraEvent)),
            m_scene, SLOT(recvCameraChanged(alo::CameraEvent)));
    connect(m_graphView, SIGNAL(sendGraphChanged()),
            m_glview, SLOT(recvAttribChanged()));
#endif

    setWindowTitle(tr("Acacia"));

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
    connect(m_displaySolidAct, &QAction::triggered,
            this, &MainWindow::recvDisplaySolidChanged );   
    connect(m_displayWireAct, &QAction::triggered,
            this, &MainWindow::recvDisplayWireChanged );   
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
    m_glview->setDisplaySolid();
}

void MainWindow::recvDisplayWireChanged(bool checked)
{
    if(!checked) {
        m_displayWireAct->setChecked(true);
        return;
    }
    m_displaySolidAct->setChecked(false);
    m_glview->setDisplayWire();
}
