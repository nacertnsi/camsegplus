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

#ifndef CAMSEGDIALOG_H
#define CAMSEGDIALOG_H

#include <QDialog>
#include <libCAMSEG/campaign.h>

#define TITLE_FONT_SIZE 18
#define PICTURE_SIZE 48

class QVBoxLayout;
class QHBoxLayout;
class QPushButton;
class QLabel;
class QFrame;

class CAMSEGDialog : public QDialog
{
    Q_OBJECT    

    public:

        enum DialogType
        {
            DialogWithPicture,
            DialogWithoutPicture
        };

        enum ButtonType
        {
            OkCancelButtons,
            CloseOnly,
            NoButtons
        };

        CAMSEGDialog(DialogType p_dialogType, ButtonType p_buttonType, CAMSEG::Campaign* p_campaign, QWidget* parent = NULL);
        CAMSEGDialog(DialogType p_dialogType, ButtonType p_buttonType, QWidget* p_centralWidget, CAMSEG::Campaign* p_campaign, QWidget* parent = NULL);
        ~CAMSEGDialog();

        void loadUI();

        void setPicture(const QPixmap p_picture);
        void setTitle(const QString p_title);
        void setCentralWidget(QWidget* p_centralWidget);
        void removeTitle();
        void showBottomLine(bool p_show);
        void lockSize();
        void disableCancelButton();
        void setHasDefaultButton(const bool p_value);

        void addButton(QWidget* p_button, int p_position = 0);

        const QPixmap* getPicture() const;
        QString getTitle() const;
        QWidget* getCentralWidget();

        bool hasDefaultButton() const { return m_hasDefaultButton; }

    signals:
        void btnOKClicked();
        void btnCancelClicked();
        void btnCloseClicked();

    protected:
        void keyPressEvent(QKeyEvent *e);

        DialogType m_dialogType;
        ButtonType m_buttonType;
        QWidget* m_centralWidget;

        CAMSEG::Campaign* m_campaign;

        QVBoxLayout* m_mainLayout;
        QHBoxLayout* m_topLayout;
        QHBoxLayout* m_buttonLayout;

        QLabel* lblPicture;
        QLabel* lblTitle;

        QPushButton* btnOK;
        QPushButton* btnCancel;
        QPushButton* btnClose;

        QFrame* bottomLine;

        bool m_hasDefaultButton;

    private:
        void loadMainUI();


};

#endif // CAMSEGDIALOG_H
