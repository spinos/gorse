/*
 *  MainWindow.h
 *  gorse vachellia
 *
 *  2019/5/25
 */

#ifndef VCHL_MAIN_WINDOW_H
#define VCHL_MAIN_WINDOW_H

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
class VachellScene;
class AssetPalette;
class AttribEditor;
class RenderWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();

private slots:
    void setProject();
    void clear();
    void open();
    void save();
    void saveAs();
    void about();

private slots:
    void recvDisplaySolidChanged(bool checked);
    void recvDisplayWireChanged(bool checked);
    
private:
    void createActions();
    void createStatusBar();
    void updateTitle();

    QMenu *viewMenu;
    QMenu *displayMenu;
    QAction *m_displaySolidAct;
    QAction *m_displayWireAct;
    
    alo::SceneGraph *m_graphView;
    VachellScene *m_scene;
    AssetPalette *m_palette;
    AttribEditor *m_editor;
    RenderWidget *m_renderView;
    
};

#endif
