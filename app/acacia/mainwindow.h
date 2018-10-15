
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
class QAction;
class QListWidget;
class QMenu;
class QTextEdit;
QT_END_NAMESPACE

namespace alo {
class SceneGraph;
}
class AcaciaScene;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();

private slots:
    void save();
    void about();
    
private:
    void createActions();
    void createStatusBar();
    void createDockWindows();

    QListWidget *paragraphsList;

    QMenu *viewMenu;
    
    alo::SceneGraph *m_graphView;
    AcaciaScene *m_scene;
    
};
//! [0]

#endif
