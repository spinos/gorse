
#include <QtWidgets>

#include "mainwindow.h"
#include "AssetPalette.h"
#include "AcaciaScene.h"
#include "AttribEditor.h"
#include <qt_graph/SceneGraph.h>

MainWindow::MainWindow()
{
    createActions();
    //createStatusBar();
    QDockWidget *dock = new QDockWidget(tr("Assets"), this);
    dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    m_palette = new AssetPalette(dock);
    dock->setWidget(m_palette);
    addDockWidget(Qt::LeftDockWidgetArea, dock);
    viewMenu->addAction(dock->toggleViewAction());

    m_scene = new AcaciaScene(m_palette->assetCollector());
    m_graphView = new alo::SceneGraph(m_scene, this);
    setCentralWidget(m_graphView);

    m_editor = new AttribEditor(m_scene, dock);
    dock = new QDockWidget(tr("Attributes"), this);
    dock->setWidget(m_editor);
    addDockWidget(Qt::LeftDockWidgetArea, dock);
    //dock->setFloating(true);
    viewMenu->addAction(dock->toggleViewAction());

    connect(m_scene, SIGNAL(sendSelectGlyph(bool)), 
    m_editor, SLOT(recvSelectGlyph(bool)));

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

//! [8]
void MainWindow::createStatusBar()
{
    statusBar()->showMessage(tr("Ready"));
}
