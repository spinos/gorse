
#include <QtWidgets>

#include "mainwindow.h"
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

    m_scene = new AcaciaScene(m_palette->assetCollector());
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

    connect(m_editor, SIGNAL(sendAttribChanged()), 
    m_glview, SLOT(recvAttribChanged()));

    connect(m_scene, SIGNAL(sendUpdateDrawable()),
    m_glview, SLOT(recvAttribChanged()));

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
    //QToolBar *fileToolBar = addToolBar(tr("File"));

    const QIcon saveIcon = QIcon::fromTheme("document-save", QIcon(":/images/save.png"));
    QAction *saveAct = new QAction(saveIcon, tr("&Save..."), this);
    saveAct->setShortcuts(QKeySequence::Save);
    saveAct->setStatusTip(tr("Save the current form letter"));
    connect(saveAct, &QAction::triggered, this, &MainWindow::save);
    fileMenu->addAction(saveAct);
    //fileToolBar->addAction(saveAct);

    fileMenu->addSeparator();

    QAction *quitAct = fileMenu->addAction(tr("&Quit"), this, &QWidget::close);
    quitAct->setShortcuts(QKeySequence::Quit);
    quitAct->setStatusTip(tr("Quit the application"));

    viewMenu = menuBar()->addMenu(tr("&View"));

    menuBar()->addSeparator();

    QMenu *helpMenu = menuBar()->addMenu(tr("&Help"));

    QAction *aboutAct = helpMenu->addAction(tr("&About"), this, &MainWindow::about);
    aboutAct->setStatusTip(tr("Show the application's About box"));

}

void MainWindow::createStatusBar()
{
    statusBar()->showMessage(tr("Ready"));
}
