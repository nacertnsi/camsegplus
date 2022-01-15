/*
CAMSEG SCM
Copyright (C) 2008-2010 CAMSEG Technologies

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "camsegdialog.h"

#include <QSettings>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QKeyEvent>

#include "global.h"
#include "qtwin.h"

using namespace CAMSEG;

CAMSEGDialog::CAMSEGDialog(DialogType p_dialogType, ButtonType p_buttonType, Campaign* p_campaign, QWidget* parent) :
        QDialog(parent), m_dialogType(p_dialogType), m_buttonType(p_buttonType), m_campaign(p_campaign)
{
    m_hasDefaultButton = true;
    m_centralWidget = new QWidget();
    loadMainUI();
}

CAMSEGDialog::CAMSEGDialog(DialogType p_dialogType, ButtonType p_buttonType, QWidget* p_centralWidget, Campaign* p_campaign, QWidget* parent) :
        QDialog(parent), m_dialogType(p_dialogType), m_buttonType(p_buttonType), m_centralWidget(p_centralWidget), m_campaign(p_campaign)
{
    m_hasDefaultButton = true;
    loadMainUI();
}

CAMSEGDialog::~CAMSEGDialog()
{
    delete lblTitle;
    delete lblPicture;
    delete m_topLayout;
    delete btnOK;
    delete btnCancel;
    delete btnClose;
}

void CAMSEGDialog::setPicture(const QPixmap p_picture)
{
    lblPicture->setPixmap(p_picture);
}

void CAMSEGDialog::setTitle(const QString p_title)
{
    lblTitle->setText(p_title);
}

void CAMSEGDialog::setCentralWidget(QWidget* p_centralWidget)
{
    m_mainLayout->removeWidget(m_centralWidget);

    if (m_topLayout->parent() == m_mainLayout)
    {
        m_mainLayout->insertWidget(1, p_centralWidget);
    }
    else
    {
        m_mainLayout->insertWidget(0, p_centralWidget);
    }

    m_centralWidget = p_centralWidget;
    //m_centralWidget->setAutoFillBackground(true);
}

void CAMSEGDialog::removeTitle()
{
    m_mainLayout->removeItem(m_topLayout);
    m_topLayout->setParent(NULL);
    lblTitle->setVisible(false);
    lblPicture->setVisible(false);
}

void CAMSEGDialog::showBottomLine(bool p_show)
{
    bottomLine->setVisible(p_show);
}

void CAMSEGDialog::lockSize()
{
    m_mainLayout->setSizeConstraint(QLayout::SetFixedSize);
}

void CAMSEGDialog::disableCancelButton()
{
    btnCancel->setEnabled(false);
}

void CAMSEGDialog::setHasDefaultButton(const bool p_value)
{
    m_hasDefaultButton = p_value;
}

void CAMSEGDialog::addButton(QWidget* p_button, int p_position)
{
    m_buttonLayout->insertWidget(p_position, p_button);
}

const QPixmap* CAMSEGDialog::getPicture() const
{
    return lblPicture->pixmap();
}

QString CAMSEGDialog::getTitle() const
{
    return lblTitle->text();
}

QWidget* CAMSEGDialog::getCentralWidget()
{
    return m_centralWidget;
}

void CAMSEGDialog::loadMainUI()
{
    m_mainLayout = new QVBoxLayout(this);
    m_topLayout = new QHBoxLayout();
    m_buttonLayout = new QHBoxLayout();

    lblPicture = new QLabel();
    lblTitle = new QLabel(tr("CAMSEG Dialog"));
    setWindowTitle(lblTitle->text());

    QFont titleFont;
    titleFont.setBold(true);
    titleFont.setPointSize(TITLE_FONT_SIZE);
    lblTitle->setFont(titleFont);

    lblTitle->setMaximumHeight(PICTURE_SIZE);

    btnOK = new QPushButton(QIcon(PATH_ICONS + "ok.png"), tr("&OK"));
    btnCancel = new QPushButton(QIcon(PATH_ICONS + "close.png"), tr("&Cancel"));
    btnClose = new QPushButton(QIcon(PATH_ICONS + "close.png"), tr("&Close"));

    //Button Height
    QSettings settings;
    int height = settings.value("General/ButtonHeight").toInt();
    btnOK->setFixedHeight(height);
    btnCancel->setFixedHeight(height);
    btnClose->setFixedHeight(height);

    bottomLine = new QFrame();
    bottomLine->setFrameShape(QFrame::HLine);

    if (m_dialogType == DialogWithPicture)
    {
        lblPicture->setMaximumSize(PICTURE_SIZE, PICTURE_SIZE);
        m_topLayout->addWidget(lblPicture);
    }

    m_topLayout->addWidget(lblTitle);

    if (m_buttonType == OkCancelButtons)
    {
        m_buttonLayout->addWidget(btnOK);
        m_buttonLayout->addWidget(btnCancel);
        btnOK->setDefault(true);
    }
    else if (m_buttonType == CloseOnly)
    {
        m_buttonLayout->addWidget(btnClose);
        btnClose->setDefault(true);
    }

    m_buttonLayout->setAlignment(Qt::AlignRight);

    m_mainLayout->addLayout(m_topLayout);
    m_mainLayout->addWidget(m_centralWidget);
    m_mainLayout->addWidget(bottomLine);
    m_mainLayout->addLayout(m_buttonLayout);


    //Connexions
    connect(btnOK, SIGNAL(clicked()), this, SIGNAL(btnOKClicked()));
    connect(btnCancel, SIGNAL(clicked()), this, SIGNAL(btnCancelClicked()));
    connect(btnClose, SIGNAL(clicked()), this, SIGNAL(btnCloseClicked()));

    //Blur
    if (settings.value("General/DialogBackgroundBlur").toBool())
    {
#ifdef Q_WS_X11
        setAttribute(Qt::WA_TranslucentBackground);
        setAttribute(Qt::WA_NoSystemBackground, false);
        QPalette pal = palette();
        QColor bg = pal.window().color();
        bg.setAlpha(180);
        pal.setColor(QPalette::Window, bg);
        setPalette(pal);
        ensurePolished(); // workaround Oxygen filling the background
        setAttribute(Qt::WA_StyledBackground, false);
#endif
        if (QtWin::isCompositionEnabled())
        {
            QtWin::extendFrameIntoClientArea(this);
            setContentsMargins(0, 0, 0, 0);
        }
    }
}

void CAMSEGDialog::keyPressEvent(QKeyEvent *e)
{
    //   Calls reject() if Escape is pressed. Simulates a button
    //   click for the default button if Enter is pressed. Move focus
    //   for the arrow keys. Ignore the rest.

#ifdef Q_WS_MAC
    if (e->modifiers() == Qt::ControlModifier && e->key() == Qt::Key_Period)
    {
        reject();
    } else
#endif
    if ((!e->modifiers() || (e->modifiers() & Qt::KeypadModifier && e->key() == Qt::Key_Enter)) && m_hasDefaultButton)
    {
        switch (e->key())
        {
        case Qt::Key_Enter:
        case Qt::Key_Return:
        {
            QList<QPushButton*> list = qFindChildren<QPushButton*>(this);

            for (int i=0; i<list.size(); ++i)
            {
                QPushButton *pb = list.at(i);

                if (pb->isDefault() && pb->isVisible())
                {
                    if (pb->isEnabled())
                        pb->click();

                    return;
                }
            }
            break;
        }
        case Qt::Key_Escape:
            reject();
            break;

        default:
            e->ignore();
            return;
        }
    }
    else
    {
        e->ignore();
    }
}
