
#include <QtWidgets>

#include "mainwindow.h"
#include "AssetPalette.h"
#include "AcaciaScene.h"
#include <qt_graph/SceneGraph.h>

MainWindow::MainWindow()
{
    m_scene = new AcaciaScene;
    m_graphView = new alo::SceneGraph(m_scene, this);
    setCentralWidget(m_graphView);

    createActions();
    //createStatusBar();
    createDockWindows();

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
//! [8]

//! [9]
void MainWindow::createDockWindows()
{
    QDockWidget *dock = new QDockWidget(tr("Assets"), this);
    dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    dock->setWidget(new AssetPalette(dock));
    addDockWidget(Qt::LeftDockWidgetArea, dock);
    viewMenu->addAction(dock->toggleViewAction());

    dock = new QDockWidget(tr("Attributes"), this);
    dock->setWidget(new QLabel("efg", dock));
    addDockWidget(Qt::RightDockWidgetArea, dock);
    viewMenu->addAction(dock->toggleViewAction());

}
//! [9]
