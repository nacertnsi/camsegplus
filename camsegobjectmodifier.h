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

#ifndef CAMSEGOBJECTMODIFIER_H
#define CAMSEGOBJECTMODIFIER_H

#include <QWidget>
#include <QLineEdit>
#include <QTextEdit>
#include <QComboBox>
#include <QListWidget>
#include <QSpinBox>
#include <QCheckBox>
#include "global.h"
#include <libCAMSEG/campaign.h>
#include <libCAMSEG/posbuttoncontent.h>
#include <libCAMSEG/currency.h>
#include "frmlocationmodif.h"
#include "frmpicture.h"
#include "frmplu.h"
#include "frmchangepermissions.h"
#include "frmselectdonator.h"
#include "codeedit.h"

#include "amountedit.h"

#define MAX_LABEL 1
#define MAX_LINE 5
#define MAX_TEXT 2
#define MAX_COMBO 4
#define MAX_DSPIN 5
#define MAX_LIST 1
#define MAX_CHK 2
#define MAX_DATE 1
#define MAX_INTSPIN 1
#define MAX_PAYMENT 4
#define MAX_TABWIDGET 1

#define MAX_VALUE_SPINBOX 100000
#define DECIMALS_TAX 4

using namespace CAMSEG;

class PaymentWidget : public QWidget
{
    Q_OBJECT

    public:
        PaymentWidget(CAMSEG::Campaign* p_campaign, QWidget* parent = NULL);
        ~PaymentWidget();

        AmountEdit* amount;
        QComboBox* pmode;
        QDateEdit* date;
        QTextEdit* infos;
        QList<int> listModes;
};

class CAMSEGObjectModifier : public QWidget
{
    Q_OBJECT

    public:

        enum ComboFillType
        {
            SexFill,
            UserTypeFill,
            ActivityStatusFill,
            ActivityFilterFill,
            DeliveryStatusFill,
            DeliveryStatusFilterFill,
            QuantityFormatFill,
            CodeTypeFill,
            CurrencySignPositionFill,
            CustomerCatFill,
            VendorCatFill,
            ProductCatFill,
            PLUCatFill,
            SupplierFill,
            PaymentModeFill,
            UnitTypeFill,
            UnitFill,
            PLUFill,
            ISOCurrencyFill,
            ExpenseTypeFill,
            CustomerFill,
            VendorFill,
            ProductFill,
            ServiceFill
        };

        CAMSEGObjectModifier(ObjectType p_viewType, int p_objectInternalID, Campaign* p_campaign, QWidget* parent = 0);  //Modify
        CAMSEGObjectModifier(Currency* p_currency, Campaign* p_campaign, QWidget* parent = 0);  //Modify Currency
        CAMSEGObjectModifier(ObjectType p_viewType, Campaign* p_campaign, QWidget* parent = 0, unsigned int p_index2 = 0, bool secondButtonIsClear = true);     //Add                      //Add
        ~CAMSEGObjectModifier();

        static void fillComboBox(QComboBox* p_combo, QList<int>* p_listIDs, ComboFillType p_fillType, Campaign* p_campaign);
        static void fillTaxList(QListWidget* p_list, QList<int>* p_listIDs, Campaign* p_campaign);
        static void fillUnitList(QComboBox* p_combo, QList<int>* p_listIDs, UnitType p_type, Campaign* p_campaign);

        bool validation();
        void fillData();
        void reloadUI();

        void showBottomLine(bool p_show);

    public slots:
        void add();
        void clear();
        void modify();

        void modifyLoginInfos();
        void modifyLocation();
        void changePassword();
        void modifyPicture();
        void modifyPermissions();
        void modifyPOSButton();
        void selectDonator();

        void updateConversionLabel();
        void updatePaymentCount(int p_newNn);
        void updateUsername();

    signals:
        void accepted();
        void close();

    private:
        void loadUI();

        int m_index2;
        bool isDonator;

        QList<int>* m_listIDs;
        QList<int>* m_listIDs2;
        QList<int>* m_listIDs3;
        QList<int>* m_listIDs4;

        QPushButton*    btnAdd;
        QPushButton*    btnModify;
        QPushButton*    btnClear;
        QPushButton*    btnCancel;

        QPushButton*    btnLocation;
        QPushButton*    btnLoginInfos;
        QPushButton*    btnPassword;
        QPushButton*    btnPicture;
        QPushButton*    btnPermissions;
        QPushButton*    btnWho;
        QPushButton*    btnPOSButton;

        QLabel*         label[MAX_LABEL];
        QLineEdit*      lineEdit[MAX_LINE];
        QTextEdit*      textEdit[MAX_TEXT];
        QComboBox*      combo[MAX_COMBO];
        QListWidget*    list[MAX_LIST];
        AmountEdit*     doublespin[MAX_DSPIN];
        QCheckBox*      checkbox[MAX_CHK];
        QDateEdit*      dateedit[MAX_DATE];
        QSpinBox*       intspin[MAX_INTSPIN];
        PaymentWidget*  paymentw[MAX_PAYMENT];
        QTabWidget*     tabwidget[MAX_TABWIDGET];

        QFrame*         bottomLine;

        Location*       m_location;
        LoginInfos*     m_loginInfos;
        Permissions*    m_perm;
        POSButtonContent m_buttonContent;
        QPixmap         m_picture;
        Currency*       m_currency;

        bool            m_modification;
        ObjectType      m_type;
        unsigned int    m_objectInternalID;
        Campaign*       m_campaign;
        QWidget*        m_parent;
};

#else
class CAMSEGObjectModifier;

#endif // CAMSEGOBJECTMODIFIER_H
