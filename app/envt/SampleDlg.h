/*
 *  SampleDlg.h
 *  envt
 *
 *  Created by jian zhang on Mon Jul 27 18:34:17 CST 2015 
 *  Copyright 2013 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef SAMPLE_DLG_H
#define SAMPLE_DLG_H

#include <QDialog>

QT_BEGIN_NAMESPACE

QT_END_NAMESPACE

#include <interface/light.h>

class SampleWidget;

class SampleDlg : public QDialog
{
    Q_OBJECT

public:
    SampleDlg(QWidget *parent = 0);
	
	void setEnvLight(const alo::EnvLightTyp* lit);
	
public slots:
	
private slots:
		
signals:
    
private:

	SampleWidget* m_wig;
    	
};

#endif