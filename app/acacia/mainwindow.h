
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
class AssetPalette;
class AttribEditor;
class GLWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();

private slots:
    void save();
    void about();

private slots:
    void recvDisplaySolidChanged(bool checked);
    void recvDisplayWireChanged(bool checked);
    
private:
    void createActions();
    void createStatusBar();

    QMenu *viewMenu;
    QMenu *displayMenu;
    QAction *m_displaySolidAct;
    QAction *m_displayWireAct;
    
    alo::SceneGraph *m_graphView;
    AcaciaScene *m_scene;
    AssetPalette *m_palette;
    AttribEditor *m_editor;
    GLWidget *m_glview;
    
};
//! [0]

#endif
