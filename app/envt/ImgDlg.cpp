/*
 *  ImgDlg.cpp
 *  envt
 *
 *  Created by jian zhang on 9/23/13.
 *  Copyright 2013 __MyCompanyName__. All rights reserved.
 *
 */

#include <QVBoxLayout>
#include "ImgDlg.h"
#include "ImgWidget.h"
#include <interface/EnvironmentLight.h>

ImgDlg::ImgDlg(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(tr("Evironment Map"));
	
	m_wig = new ImgWidget(this);
    
    QVBoxLayout *layout = new QVBoxLayout;
	layout->addWidget(m_wig);
	
	setLayout(layout);
	
	resize(800, 400);
    
}

void ImgDlg::setEnvLight(const alo::EnvLightTyp* lit)
{ m_wig->addImage(lit->levelImage(5) ); }
