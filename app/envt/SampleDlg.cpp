/*
 *  SampleDlg.cpp
 *  envt
 *
 *  Created by jian zhang on 9/23/13.
 *  Copyright 2013 __MyCompanyName__. All rights reserved.
 *
 */

#include <QVBoxLayout>
#include "SampleDlg.h"
#include "SampleWidget.h"

SampleDlg::SampleDlg(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(tr("Evironment Samples"));
	
	m_wig = new SampleWidget(this);
    
    QVBoxLayout *layout = new QVBoxLayout;
	layout->addWidget(m_wig);
	
	setLayout(layout);
	
	resize(800, 400);
    
}

void SampleDlg::setEnvLight(const alo::EnvLightTyp* lit)
{ m_wig->generateSamples(lit); }
//:~