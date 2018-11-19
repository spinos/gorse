/*
 *  AFileDlg.h
 *  aloe
 *
 *  title
 *  icon notice
 *  folder browse
 *  file save cancel
 *  list of files in folder
 *
 */

#ifndef ALO_A_FILE_DLG_H
#define ALO_A_FILE_DLG_H

#include <QDialog>
#include <string>

QT_BEGIN_NAMESPACE
class QLabel;
class QLineEdit;
class QPushButton;
QT_END_NAMESPACE

namespace alo {

struct AFileDlgProfile {

    enum FileMode {
        FRead = 255,
        FWrite
    };

    FileMode _mode;
    std::string _title;
    std::string _iconName;
    std::string _notice;
    std::string _applyName;
    std::string _ext;
    std::string _folder;
    std::string _name;

    AFileDlgProfile();
    AFileDlgProfile(FileMode mode,
        const std::string &title,
        const std::string &iconName,
        const std::string &notice,
        const std::string &applyName,
        const std::string &ext,
        const std::string &folder,
        const std::string &name);
    
    void setName(const std::string &x);
    void setFolder(const std::string &x);

    std::string getFilePath() const;
    
};
    
class ListSelector;

class AFileDlg : public QDialog
{
    Q_OBJECT

public:
    AFileDlg(AFileDlgProfile &prof, QWidget *parent = 0);

protected:
    void lsFilesInFolder();

private slots:
	void getFolderPath();
	void recvListItemSelection(const QPair<std::string, std::string> &x);
    void recvNameChange(const QString &x);
    void recvFolderChange(const QString &x);
    
private:
	QLabel * m_notice;
    QLineEdit *m_folderEdt;
    QLineEdit *m_nameEdt;
	QPushButton * m_applyBtn;
	QPushButton * m_cancelBtn;
	ListSelector *m_fileList;
    AFileDlgProfile *m_profile;
};

}

#endif