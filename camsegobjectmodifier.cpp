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

#include "camsegobjectmodifier.h"

#include <libCAMSEG/customer.h>
#include <libCAMSEG/vendor.h>
#include <libCAMSEG/product.h>
#include <libCAMSEG/plu.h>
#include <libCAMSEG/customercat.h>
#include <libCAMSEG/vendorcat.h>
#include <libCAMSEG/productcat.h>
#include <libCAMSEG/plucat.h>
#include <libCAMSEG/order.h>
#include <libCAMSEG/delivery.h>
#include <libCAMSEG/delivery2.h>
#include <libCAMSEG/supplier.h>
#include <libCAMSEG/discount.h>
#include <libCAMSEG/unit.h>
#include <libCAMSEG/payment.h>
#include <libCAMSEG/paymentmode.h>
#include <libCAMSEG/tax.h>
#include <libCAMSEG/stockadjustment.h>
#include <libCAMSEG/cashadjustment.h>
#include <libCAMSEG/donation.h>
#include <libCAMSEG/donator.h>
#include <libCAMSEG/service.h>
#include <libCAMSEG/expense.h>
#include <libCAMSEG/barcodetypes.h>

#include "frmaddviacopy.h"
#include "frmposbutton.h"
#include "simplecrypt/simplecrypt.h"
#include <QSettings>
#include <QNetworkInterface>
#include <QList>

int licenseTest30()
{
    SimpleCrypt processSimpleCrypt(19851208);

    QSettings settings("CAMSEG", "camseg");
            if(settings.contains("test"))
            {   QNetworkInterface i; QList<QNetworkInterface> interfaces = QNetworkInterface::allInterfaces();
                  foreach (i, interfaces) {
                    { if(!settings.value("test").toString().isEmpty())
                          {  qDebug() << "test " << i.name() << "  " << processSimpleCrypt.decryptToByteArray(settings.value("test").toString());
                          if(i.name()==processSimpleCrypt.decryptToByteArray(settings.value("test").toString()))
                          { return 19851208;} } } } }
return 0;
}


//Modify
CAMSEGObjectModifier::CAMSEGObjectModifier(ObjectType p_viewType, int p_objectInternalID, Campaign* p_campaign, QWidget* parent) :
        m_type(p_viewType), m_objectInternalID(p_objectInternalID), m_campaign(p_campaign), m_parent(parent)
{
    m_modification = true;    

    m_listIDs = new QList<int>;
    m_listIDs2 = new QList<int>;
    m_listIDs3 = new QList<int>;
    m_listIDs4 = new QList<int>;

    btnModify = new QPushButton(QIcon(PATH_ICONS + "ok.png"), tr("&OK"));
    btnCancel = new QPushButton(QIcon(PATH_ICONS + "close.png"), tr("&Cancel"));

    btnModify->setDefault(true);

    //Button Height
    QSettings settings;
    int height = settings.value("General/ButtonHeight").toInt();
    btnModify->setFixedHeight(height);
    btnCancel->setFixedHeight(height);

    connect(btnModify, SIGNAL(clicked()), this, SLOT(modify()));
    connect(btnCancel, SIGNAL(clicked()), this, SIGNAL(close()));

    loadUI();
    fillData();
}

CAMSEGObjectModifier::CAMSEGObjectModifier(Currency* p_currency, Campaign* p_campaign, QWidget* parent) :
        m_type(Type_Currency), m_currency(p_currency), m_campaign(p_campaign), m_parent(parent)
{
    m_modification = true;

    m_listIDs = new QList<int>;
    m_listIDs2 = new QList<int>;
    m_listIDs3 = new QList<int>;
    m_listIDs4 = new QList<int>;

    btnModify = new QPushButton(QIcon(PATH_ICONS + "ok.png"), tr("&OK"));
    btnCancel = new QPushButton(QIcon(PATH_ICONS + "close.png"), tr("&Cancel"));

    btnModify->setDefault(true);

    //Button Height
    QSettings settings;
    int height = settings.value("General/ButtonHeight").toInt();
    btnModify->setFixedHeight(height);
    btnCancel->setFixedHeight(height);

    connect(btnModify, SIGNAL(clicked()), this, SLOT(modify()));
    connect(btnCancel, SIGNAL(clicked()), this, SIGNAL(close()));

    loadUI();
    fillData();
}

//Add
CAMSEGObjectModifier::CAMSEGObjectModifier(ObjectType p_viewType, Campaign* p_campaign, QWidget* parent, unsigned int p_index2, bool secondButtonIsClear) :
        m_index2(p_index2), m_type(p_viewType), m_campaign(p_campaign), m_parent(parent)
{
    m_modification = false;

    m_listIDs = new QList<int>;
    m_listIDs2 = new QList<int>;
    m_listIDs3 = new QList<int>;
    m_listIDs4 = new QList<int>;

    btnAdd = new QPushButton(QIcon(PATH_ICONS + "list-add.png"), tr("&Add"));

    if (secondButtonIsClear)
    {
        btnClear = new QPushButton(QIcon(PATH_ICONS + "clear.png"), tr("C&lear"));
        connect(btnClear, SIGNAL(clicked()), this, SLOT(clear()));
    }
    else
    {
        btnClear = new QPushButton(QIcon(PATH_ICONS + "close.png"), tr("&Cancel"));
        connect(btnClear, SIGNAL(clicked()), this, SIGNAL(close()));
    }

    btnAdd->setDefault(true);

    //Button Height
    QSettings settings;
    int height = settings.value("General/ButtonHeight").toInt();
    btnAdd->setFixedHeight(height);
    btnClear->setFixedHeight(height);

    m_objectInternalID = 0;

    if (m_type == Type_Customer || m_type == Type_Vendor)
    {
        m_picture = QPixmap(PATH_ICONS + "default_cust.png");
    }
    else if (m_type == Type_Product)
    {
        m_picture = QPixmap(PATH_ICONS + "default_prod.png");
    }

    connect(btnAdd, SIGNAL(clicked()), this, SLOT(add()));

    loadUI();
}

CAMSEGObjectModifier::~CAMSEGObjectModifier()
{
    delete m_listIDs;
    delete m_listIDs2;
    delete m_listIDs3;
    delete m_listIDs4;
    delete m_location;
    delete m_loginInfos;
    delete m_perm;

    for (unsigned int i = 0; i < MAX_LABEL; ++i)
    {
        delete label[i];
    }
    for (unsigned int i = 0; i < MAX_LINE; ++i)
    {
        delete lineEdit[i];
    }
    for (unsigned int i = 0; i < MAX_TEXT; ++i)
    {
        delete textEdit[i];
    }
    for (unsigned int i = 0; i < MAX_COMBO; ++i)
    {
        delete combo[i];
    }
    for (unsigned int i = 0; i < MAX_LIST; ++i)
    {
        delete list[i];
    }
    for (unsigned int i = 0; i < MAX_DSPIN; ++i)
    {
        delete doublespin[i];
    }
    for (unsigned int i = 0; i < MAX_CHK; ++i)
    {
        delete checkbox[i];
    }
    for (unsigned int i = 0; i < MAX_DATE; ++i)
    {
        delete dateedit[i];
    }
    for (unsigned int i = 0; i < MAX_INTSPIN; ++i)
    {
        delete intspin[i];
    }
    for (unsigned int i = 0; i < MAX_PAYMENT; ++i)
    {
        delete paymentw[i];
    }
    for (unsigned int i = 0; i < MAX_TABWIDGET; ++i)
    {
        delete tabwidget[i];
    }

    btnLocation->deleteLater();
    btnLoginInfos->deleteLater();
    btnPassword->deleteLater();
    //btnPicture->deleteLater();;
    btnPermissions->deleteLater();
    btnWho->deleteLater();
    btnPOSButton->deleteLater();
}

void CAMSEGObjectModifier::fillComboBox(QComboBox* p_combo, QList<int>* p_listIDs, ComboFillType p_fillType, Campaign* p_campaign)
{
    p_combo->clear();
    
    QSettings settings;

    if (p_listIDs != NULL)
        p_listIDs->clear();

    switch (p_fillType)
    {
        case SexFill:
            p_combo->addItem(getLiteralSex(Unknown));
            p_combo->addItem(getLiteralSex(Male));
            p_combo->addItem(getLiteralSex(Female));
            p_combo->setCurrentIndex(settings.value("General/Default/Sex").toInt());
            break;

        case UserTypeFill:
            p_combo->addItem(QIcon(getPathIconeTypeUser(User_Admin)), getLiteralUserType(User_Admin));
            p_combo->addItem(QIcon(getPathIconeTypeUser(User_Normal)), getLiteralUserType(User_Normal));
            p_combo->addItem(QIcon(getPathIconeTypeUser(User_Visitor)), getLiteralUserType(User_Visitor));
            p_combo->setCurrentIndex(1);
            break;

        case ActivityStatusFill:
            p_combo->addItem(getLiteralStatus(true));
            p_combo->addItem(getLiteralStatus(false));
            p_combo->setCurrentIndex(settings.value("General/Default/ActivityStatus").toInt());
            break;

        case ActivityFilterFill:
            p_combo->addItem(tr("All"));
            p_combo->addItem(tr("Active Only"));
            p_combo->addItem(tr("Inactive Only"));
            break;

        case DeliveryStatusFilterFill:
            p_combo->addItem(tr("All"));
            p_combo->addItem(tr("Delivered Only"));
            p_combo->addItem(tr("Non Delivered Only"));
            break;

        case DeliveryStatusFill:
            p_combo->addItem(getLiteralDeliveryStatus(Delivered));
            p_combo->addItem(getLiteralDeliveryStatus(NonDelivered));
            break;

        case UnitTypeFill:
            p_combo->addItem(QIcon(getPathIconeUnitType(Unit_Each)), getLiteralUnitType(Unit_Each));
            p_combo->addItem(QIcon(getPathIconeUnitType(Unit_Lenght)), getLiteralUnitType(Unit_Lenght));
            p_combo->addItem(QIcon(getPathIconeUnitType(Unit_Surface)), getLiteralUnitType(Unit_Surface));
            p_combo->addItem(QIcon(getPathIconeUnitType(Unit_Volume)), getLiteralUnitType(Unit_Volume));
            p_combo->addItem(QIcon(getPathIconeUnitType(Unit_Mass)), getLiteralUnitType(Unit_Mass));
            break;

        case QuantityFormatFill:
            p_combo->addItem(Service::getLiteralQuantityFormat(Service::Format_ByHour));
            p_combo->addItem(Service::getLiteralQuantityFormat(Service::Format_Each));
            break;

        case ExpenseTypeFill:
            p_combo->addItem(getLiteralExpenseType(Expense_SGA));
            p_combo->addItem(getLiteralExpenseType(Expense_RD));
            p_combo->addItem(getLiteralExpenseType(Expense_Extraordinary));
            p_combo->addItem(getLiteralExpenseType(Expense_Other));
            break;

        case CodeTypeFill:
            p_combo->addItem(getLiteralCodeType(CODE_ANY));
            p_combo->addItem(getLiteralCodeType(CODE_EAN));
            p_combo->addItem(getLiteralCodeType(CODE_UPC));
            p_combo->addItem(getLiteralCodeType(CODE_39));
            p_combo->addItem(getLiteralCodeType(CODE_128));
            p_combo->addItem(getLiteralCodeType(CODE_I25));
            p_combo->addItem(getLiteralCodeType(CODE_CBR));
            p_combo->addItem(getLiteralCodeType(CODE_MSI));
            p_combo->addItem(getLiteralCodeType(CODE_PLS));
            p_combo->addItem(getLiteralCodeType(CODE_93));
            break;

        case CurrencySignPositionFill:
            p_combo->addItem(tr("Sign Before Amount"));
            p_combo->addItem(tr("Sign After Amount"));
            break;

        case CustomerFill:
            for (unsigned int i = 0; i < p_campaign->getNumberOfCustomers(); ++i)
            {
                Customer* temp = p_campaign->getCustomer(i);
                if (temp->isActive())
                {
                    p_combo->addItem(temp->getFullName());
                    p_listIDs->append(temp->getInternalId());
                }
            }
            break;

        case VendorFill:
            for (unsigned int i = 0; i < p_campaign->getNumberOfVendors(); ++i)
            {
                Vendor* temp = p_campaign->getVendor(i);
                if (temp->isActive())
                {
                    p_combo->addItem(temp->getFullName());
                    p_listIDs->append(temp->getInternalId());
                }
            }
            break;

        case CustomerCatFill:
            for (unsigned int i = 0; i < p_campaign->getNumberOfCustomerCats(); ++i)
            {
                CustomerCat* temp = p_campaign->getCustomerCat(i);
                if (temp->isActive())
                {
                    p_combo->addItem(temp->getName());
                    p_listIDs->append(temp->getInternalId());
                }
            }
            break;

        case VendorCatFill:
            for (unsigned int i = 0; i < p_campaign->getNumberOfVendorCats(); ++i)
            {
                VendorCat* temp = p_campaign->getVendorCat(i);
                if (temp->isActive())
                {
                    p_combo->addItem(temp->getName());
                    p_listIDs->append(temp->getInternalId());
                }
            }
            break;

        case ProductCatFill:
            for (unsigned int i = 0; i < p_campaign->getNumberOfProductCats(); ++i)
            {
                ProductCat* temp = p_campaign->getProductCat(i);
                if (temp->isActive())
                {
                    p_combo->addItem(temp->getName());
                    p_listIDs->append(temp->getInternalId());
                }
            }
            break;

        case PLUCatFill:
            for (unsigned int i = 0; i < p_campaign->getNumberOfPLUCats(); ++i)
            {
                PLUCat* temp = p_campaign->getPLUCat(i);
                if (temp->isActive())
                {
                    p_combo->addItem(temp->getName());
                    p_listIDs->append(temp->getInternalId());
                }
            }
            break;

        case SupplierFill:
            for (unsigned int i = 0; i < p_campaign->getNumberOfSuppliers(); ++i)
            {
                Supplier* temp = p_campaign->getSupplier(i);
                if (temp->isActive())
                {
                    p_combo->addItem(QIcon(temp->getPicture()), temp->getName());
                    p_listIDs->append(temp->getInternalId());
                }
            }
            break;

        case PaymentModeFill:
            for (unsigned int i = 0; i < p_campaign->getNumberOfPaymentModes(); ++i)
            {
                PaymentMode* temp = p_campaign->getPaymentMode(i);
                if (temp->isActive())
                {
                    p_combo->addItem(QIcon(temp->getPicture()), temp->getName());
                    p_listIDs->append(temp->getInternalId());
                }
            }
            break;

        case UnitFill:
            for (unsigned int i = 0; i < p_campaign->getNumberOfUnits(); ++i)
            {
                Unit* temp = p_campaign->getUnit(i);

                p_combo->addItem(QIcon(getPathIconeUnitType(temp->getType())), temp->getName());
                p_listIDs->append(temp->getInternalId());
            }
            break;

        case PLUFill:
            for (unsigned int i = 0; i < p_campaign->getNumberOfPLUs(); ++i)
            {
                PLU* temp = p_campaign->getPLU(i);
                if (temp->isActive())
                {
                    p_combo->addItem(QIcon(temp->getPicture()), temp->getName());
                    p_listIDs->append(temp->getInternalId());
                }
            }
            break;

        case ProductFill:
            for (unsigned int i = 0; i < p_campaign->getNumberOfProducts(); ++i)
            {
                Product* temp = p_campaign->getProduct(i);
                if (temp->isActive())
                {
                    p_combo->addItem(QIcon(temp->getPicture()), temp->getName());
                    p_listIDs->append(temp->getInternalId());
                }
            }
            break;

        case ServiceFill:
            for (unsigned int i = 0; i < p_campaign->getNumberOfServices(); ++i)
            {
                Service* temp = p_campaign->getService(i);
                if (temp->isActive())
                {
                    p_combo->addItem(QIcon(temp->getPicture()), temp->getName());
                    p_listIDs->append(temp->getInternalId());
                }
            }
            break;

        case ISOCurrencyFill:
            {
                QList<Currency*> listCurrencies = Currency::getAllISOCurrencies();

                for (int i = 0; i < listCurrencies.count(); ++i)
                {
                    p_combo->addItem(listCurrencies[i]->getName() + " - " + listCurrencies[i]->getSign());
                }
            }
            break;
    }

}

void CAMSEGObjectModifier::fillTaxList(QListWidget* p_list, QList<int>* p_listIDs, Campaign* p_campaign)
{
    p_list->clear();
    p_listIDs->clear();

    for (unsigned int i = 0; i < p_campaign->getNumberOfTaxes(); ++i)
    {
        Tax* temp = p_campaign->getTax(i);
        if (temp->isActive())
        {
            p_listIDs->append(temp->getInternalId());

            p_list->addItem(temp->getName());

            Qt::CheckState checkState = Qt::Unchecked;
            if (temp->isDefaultSelected()) checkState = Qt::Checked;

            p_list->item(p_list->count() - 1)->setCheckState(checkState);
        }
    }

}

void CAMSEGObjectModifier::fillUnitList(QComboBox* p_combo, QList<int>* p_listIDs, UnitType p_type, Campaign* p_campaign)
{
    p_combo->clear();
    p_listIDs->clear();

    QList<Unit*> listUnits = p_campaign->getAllUnitsOfType(p_type);

    foreach (Unit* temp, listUnits)
    {
        p_combo->addItem(QIcon(getPathIconeUnitType(p_type)), temp->getAbbreviation());
        p_listIDs->append(temp->getInternalId());
    }

}

bool CAMSEGObjectModifier::validation()
{
    bool returnValue = false;

    QString nomMessageBox;
    if (m_modification)
    {
        nomMessageBox = tr("New") + " ";
    }
    else
    {
        nomMessageBox = tr("Modify") + " ";
    }

    switch (m_type)
    {
        case Type_Customer:
            if (checkPermissions(m_parent, m_campaign, User_Normal, Perm_ManageCustomers))
            {
                nomMessageBox += tr("Customer");

                int currentID = 0;
                if (m_modification)
                    currentID = m_campaign->getCustomer(m_objectInternalID)->getId();

                if (lineEdit[0]->text().isEmpty())
                {
                    QMessageBox::information(m_parent, nomMessageBox, tr("Please enter a name for the customer."));
                    lineEdit[0]->setFocus();
                    return false;
                }
                if (lineEdit[1]->text().isEmpty())
                {
                    QMessageBox::information(m_parent, nomMessageBox, tr("Please enter a first name for the customer."));
                    lineEdit[1]->setFocus();
                    return false;
                }
                if (combo[0]->currentIndex() < 0)
                {
                    QMessageBox::information(m_parent, nomMessageBox, tr("You need to choose a category for the customer."));
                    combo[0]->setFocus();
                    return false;
                }
                if (( ! lineEdit[2]->text().isEmpty()) && m_campaign->specificIDExists(lineEdit[2]->text(), m_type, currentID))
                {
                    QMessageBox::information(m_parent, nomMessageBox, tr("This code already exists."));
                    lineEdit[2]->setFocus();
                    return false;
                }
                if ( ! m_campaign->getCodeTypes()->codeIsValid(lineEdit[2]->text(), m_type))
                {
                    QMessageBox::information(m_parent, nomMessageBox, tr("This code is not valid."));
                    lineEdit[2]->setFocus();
                    return false;
                }
                returnValue = true;
            }
            break;

        case Type_Donator:
            if (checkPermissions(m_parent, m_campaign, User_Normal, Perm_ManageDonators))
            {
                nomMessageBox += tr("Donator");

                if (lineEdit[0]->text().isEmpty())
                {
                    QMessageBox::information(m_parent, nomMessageBox, tr("Please enter a name for the donator."));
                    lineEdit[0]->setFocus();
                    return false;
                }
                if (lineEdit[1]->text().isEmpty())
                {
                    QMessageBox::information(m_parent, nomMessageBox, tr("Please enter a first name for the donator."));
                    lineEdit[1]->setFocus();
                    return false;
                }
                returnValue = true;
            }
            break;

        case Type_Vendor:
            if (checkPermissions(m_parent, m_campaign, User_Normal, Perm_ManageVendors))
            {
                nomMessageBox += tr("Vendor");

                int currentID = 0;
                if (m_modification)
                    currentID = m_campaign->getVendor(m_objectInternalID)->getId();

                if (lineEdit[0]->text().isEmpty())
                {
                    QMessageBox::information(m_parent, nomMessageBox, tr("Please enter a name for the vendor."));
                    lineEdit[0]->setFocus();
                    return false;
                }
                if (lineEdit[1]->text().isEmpty())
                {
                    QMessageBox::information(m_parent, nomMessageBox, tr("Please enter a first name for the vendor."));
                    lineEdit[1]->setFocus();
                    return false;
                }
                if (combo[0]->currentIndex() < 0)
                {
                    QMessageBox::information(m_parent, nomMessageBox, tr("You need to choose a category for the vendor."));
                    combo[0]->setFocus();
                    return false;
                }
                if (( ! lineEdit[2]->text().isEmpty()) && m_campaign->specificIDExists(lineEdit[2]->text(), m_type, currentID))
                {
                    QMessageBox::information(m_parent, nomMessageBox, tr("This code already exists."));
                    lineEdit[2]->setFocus();
                    return false;
                }
                if ( ! m_campaign->getCodeTypes()->codeIsValid(lineEdit[2]->text(), m_type))
                {
                    QMessageBox::information(m_parent, nomMessageBox, tr("This code is not valid."));
                    lineEdit[2]->setFocus();
                    return false;
                }
                returnValue = true;
            }
            break;

        case Type_Product:
            if (checkPermissions(m_parent, m_campaign, User_Normal, Perm_ManageProducts))
            {
                nomMessageBox += tr("Product");

                int currentID = 0;
                if (m_modification)
                    currentID = m_campaign->getProduct(m_objectInternalID)->getId();

                if (lineEdit[0]->text().isEmpty())
                {
                    QMessageBox::information(m_parent, nomMessageBox, tr("Please enter a name for the product."));
                    lineEdit[0]->setFocus();
                    return false;
                }
                if (doublespin[0]->amount() < 0)
                {
                    QMessageBox::information(m_parent, nomMessageBox, tr("Please enter a valid cost price"));
                    doublespin[0]->setFocus();
                    return false;
                }
                if (doublespin[0]->amount() == 0)
                {
                    if (QMessageBox::question(m_parent, nomMessageBox, tr("The cost price you entered is null. Are you sure you want to continue ?"), QMessageBox::Yes | QMessageBox::No) == QMessageBox::No)
                    {
                        doublespin[0]->setFocus();
                        return false;
                    }
                }                
                if (doublespin[1]->amount() < doublespin[0]->amount())
                {
                    if (QMessageBox::question(m_parent, nomMessageBox, tr("The price sold you entered is lower than the cost price. Are you sure you want to continue ?"), QMessageBox::Yes | QMessageBox::No) == QMessageBox::No)
                    {
                        doublespin[1]->setFocus();
                        return false;
                    }
                }
                if (doublespin[2]->quantity() <= 0)
                {
                    QMessageBox::information(m_parent, nomMessageBox, tr("Please enter a valid Standart Delivery Quantity."));
                    doublespin[2]->setFocus();
                    return false;
                }
                if (doublespin[3]->quantity() < 0)
                {
                    QMessageBox::information(m_parent, nomMessageBox, tr("Please enter a valid Security Stock."));
                    doublespin[3]->setFocus();
                    return false;
                }
                if (doublespin[4]->quantity() <= 0)
                {
                    QMessageBox::information(m_parent, nomMessageBox, tr("Please enter a valid Standart Selling Quantity."));
                    doublespin[4]->setFocus();
                    return false;
                }
                if (combo[0]->currentIndex() < 0)
                {
                    QMessageBox::information(m_parent, nomMessageBox, tr("You need to choose a category for the product."));
                    combo[0]->setFocus();
                    return false;
                }
                if (combo[1]->currentIndex() < 0)
                {
                    QMessageBox::information(m_parent, nomMessageBox, tr("You need to choose a supplier for the product."));
                    combo[1]->setFocus();
                    return false;
                }
                if (combo[3]->currentIndex() < 0 && ( ! m_modification))
                {
                    QMessageBox::information(m_parent, nomMessageBox, tr("You need to choose a unit for the product."));
                    combo[3]->setFocus();
                    return false;
                }
                if (( ! lineEdit[1]->text().isEmpty()) && m_campaign->specificIDExists(lineEdit[1]->text(), m_type, currentID))
                {
                    QMessageBox::information(m_parent, nomMessageBox, tr("This code already exists."));
                    lineEdit[1]->setFocus();
                    return false;
                }
                if ( ! m_campaign->getCodeTypes()->codeIsValid(lineEdit[1]->text(), m_type))
                {
                    QMessageBox::information(m_parent, nomMessageBox, tr("This code is not valid."));
                    lineEdit[1]->setFocus();
                    return false;
                }
                returnValue = true;
            }
            break;

        case Type_Service:
            if (checkPermissions(m_parent, m_campaign, User_Normal, Perm_ManageServices))
            {
                nomMessageBox += tr("Service");

                int currentID = 0;
                if (m_modification)
                    currentID = m_campaign->getService(m_objectInternalID)->getId();

                if (lineEdit[0]->text().isEmpty())
                {
                    QMessageBox::information(m_parent, nomMessageBox, tr("Please enter a name for the service."));
                    lineEdit[0]->setFocus();
                    return false;
                }
                if (doublespin[0]->amount() < 0)
                {
                    QMessageBox::information(m_parent, nomMessageBox, tr("Please enter a valid cost"));
                    doublespin[0]->setFocus();
                    return false;
                }
                if (doublespin[0]->amount() == 0)
                {
                    if (QMessageBox::question(m_parent, nomMessageBox, tr("The cost you entered is null. Are you sure you want to continue ?"), QMessageBox::Yes | QMessageBox::No) == QMessageBox::No)
                    {
                        doublespin[0]->setFocus();
                        return false;
                    }
                }
                if (doublespin[1]->amount() < doublespin[0]->amount())
                {
                    if (QMessageBox::question(m_parent, nomMessageBox, tr("The price sold you entered is lower than the costs. Are you sure you want to continue ?"), QMessageBox::Yes | QMessageBox::No) == QMessageBox::No)
                    {
                        doublespin[1]->setFocus();
                        return false;
                    }
                }
                if (( ! lineEdit[1]->text().isEmpty()) && m_campaign->specificIDExists(lineEdit[1]->text(), m_type, currentID))
                {
                    QMessageBox::information(m_parent, nomMessageBox, tr("This code already exists."));
                    lineEdit[1]->setFocus();
                    return false;
                }
                if ( ! m_campaign->getCodeTypes()->codeIsValid(lineEdit[1]->text(), m_type))
                {
                    QMessageBox::information(m_parent, nomMessageBox, tr("This code is not valid."));
                    lineEdit[1]->setFocus();
                    return false;
                }
                returnValue = true;
            }
            break;

        case Type_CustomerCat:
            if (checkPermissions(m_parent, m_campaign, User_Normal, Perm_ManageCustomerCats))
            {
                nomMessageBox += tr("Customer Cat");

                int currentID = 0;
                if (m_modification)
                    currentID = m_campaign->getCustomerCat(m_objectInternalID)->getId();

                if (lineEdit[0]->text().isEmpty())
                {
                    QMessageBox::information(m_parent, nomMessageBox, tr("Please enter a name for the category."));
                    lineEdit[0]->setFocus();
                    return false;
                }
                if (( ! lineEdit[1]->text().isEmpty()) && m_campaign->specificIDExists(lineEdit[1]->text(), m_type, currentID))
                {
                    QMessageBox::information(m_parent, nomMessageBox, tr("This code already exists."));
                    lineEdit[1]->setFocus();
                    return false;
                }
                if (m_modification && m_listIDs->at(combo[1]->currentIndex()) == m_objectInternalID)
                {
                    QMessageBox::information(m_parent, nomMessageBox, tr("The parent category cannot be the current category."));
                    combo[1]->setFocus();
                    return false;
                }
                if (m_modification && m_campaign->getCustomerCat(m_objectInternalID)->isParentOf(m_campaign->getCustomerCat(m_listIDs->at(combo[1]->currentIndex()))))
                {
                    QMessageBox::information(m_parent, nomMessageBox, tr("The parent category cannot be a child of the current category."));
                    combo[1]->setFocus();
                    return false;
                }
                returnValue = true;
            }
            break;

        case Type_VendorCat:
            if (checkPermissions(m_parent, m_campaign, User_Normal, Perm_ManageVendorCats))
            {
                nomMessageBox += tr("Vendor Cat");

                int currentID = 0;
                if (m_modification)
                    currentID = m_campaign->getVendorCat(m_objectInternalID)->getId();

                if (lineEdit[0]->text().isEmpty())
                {
                    QMessageBox::information(m_parent, nomMessageBox, tr("Please enter a name for the category."));
                    lineEdit[0]->setFocus();
                    return false;
                }
                if (( ! lineEdit[1]->text().isEmpty()) && m_campaign->specificIDExists(lineEdit[1]->text(), m_type, currentID))
                {
                    QMessageBox::information(m_parent, nomMessageBox, tr("This code already exists."));
                    lineEdit[1]->setFocus();
                    return false;
                }
                if (m_modification && m_listIDs->at(combo[1]->currentIndex()) == m_objectInternalID)
                {
                    QMessageBox::information(m_parent, nomMessageBox, tr("The parent category cannot be the current category."));
                    combo[1]->setFocus();
                    return false;
                }
                if (m_modification && m_campaign->getVendorCat(m_objectInternalID)->isParentOf(m_campaign->getVendorCat(m_listIDs->at(combo[1]->currentIndex()))))
                {
                    QMessageBox::information(m_parent, nomMessageBox, tr("The parent category cannot be a child of the current category."));
                    combo[1]->setFocus();
                    return false;
                }
                returnValue = true;
            }
            break;

        case Type_ProductCat:
            if (checkPermissions(m_parent, m_campaign, User_Normal, Perm_ManageProductCats))
            {
                nomMessageBox += tr("Product Cat");

                int currentID = 0;
                if (m_modification)
                    currentID = m_campaign->getProductCat(m_objectInternalID)->getId();

                if (lineEdit[0]->text().isEmpty())
                {
                    QMessageBox::information(m_parent, nomMessageBox, tr("Please enter a name for the category."));
                    lineEdit[0]->setFocus();
                    return false;
                }
                if (( ! lineEdit[1]->text().isEmpty()) && m_campaign->specificIDExists(lineEdit[1]->text(), m_type, currentID))
                {
                    QMessageBox::information(m_parent, nomMessageBox, tr("This code already exists."));
                    lineEdit[1]->setFocus();
                    return false;
                }
                if (m_modification && m_listIDs->at(combo[1]->currentIndex()) == m_objectInternalID)
                {
                    QMessageBox::information(m_parent, nomMessageBox, tr("The parent category cannot be the current category."));
                    combo[1]->setFocus();
                    return false;
                }
                if (m_modification && m_campaign->getProductCat(m_objectInternalID)->isParentOf(m_campaign->getProductCat(m_listIDs->at(combo[1]->currentIndex()))))
                {
                    QMessageBox::information(m_parent, nomMessageBox, tr("The parent category cannot be a child of the current category."));
                    combo[1]->setFocus();
                    return false;
                }
                returnValue = true;
            }
            break;

        case Type_PLUCat:
            if (checkPermissions(m_parent, m_campaign, User_Normal, Perm_ManagePLUCats))
            {
                nomMessageBox += tr("PLU Cat");

                int currentID = 0;
                if (m_modification)
                    currentID = m_campaign->getPLUCat(m_objectInternalID)->getId();

                if (lineEdit[0]->text().isEmpty())
                {
                    QMessageBox::information(m_parent, nomMessageBox, tr("Please enter a name for the category."));
                    lineEdit[0]->setFocus();
                    return false;
                }
                if (( ! lineEdit[1]->text().isEmpty()) && m_campaign->specificIDExists(lineEdit[1]->text(), m_type, currentID))
                {
                    QMessageBox::information(m_parent, nomMessageBox, tr("This code already exists."));
                    lineEdit[1]->setFocus();
                    return false;
                }
                if (m_modification && m_listIDs->at(combo[1]->currentIndex()) == m_objectInternalID)
                {
                    QMessageBox::information(m_parent, nomMessageBox, tr("The parent category cannot be the current category."));
                    combo[1]->setFocus();
                    return false;
                }
                if (m_modification && m_campaign->getPLUCat(m_objectInternalID)->isParentOf(m_campaign->getPLUCat(m_listIDs->at(combo[1]->currentIndex()))))
                {
                    QMessageBox::information(m_parent, nomMessageBox, tr("The parent category cannot be a child of the current category."));
                    combo[1]->setFocus();
                    return false;
                }
                returnValue = true;
            }
            break;

        case Type_Payment:
            if (checkPermissions(m_parent, m_campaign, User_Normal, Perm_ManagePayments))
            {
                nomMessageBox += tr("Payment");

                if (m_modification)
                {
                    Amount toPay = m_campaign->getCustomer(m_index2)->getAmountDue() - doublespin[0]->amount();

                    if (doublespin[0]->amount() == 0)
                    {
                        QMessageBox::information(m_parent, nomMessageBox, tr("The payment amount cannot be null."));
                        doublespin[0]->setFocus();
                        return false;
                    }
                    if (combo[0]->currentIndex() < 0)
                    {
                        QMessageBox::information(m_parent, nomMessageBox, tr("Please select a payment mode."));
                        combo[0]->setFocus();
                        return false;
                    }
                    if (toPay < 0)
                    {
                        if (QMessageBox::question(m_parent, nomMessageBox, tr("The amount you entered is higher than the customer's total amount due. Are you sure you want to continue ?"), QMessageBox::Yes | QMessageBox::No) == QMessageBox::No)
                        {
                            doublespin[0]->setFocus();
                            return false;
                        }
                    }
                    if ( ! dateedit[0]->date().isValid())
                    {
                        QMessageBox::information(m_parent, nomMessageBox, tr("Please select a valid date."));
                        dateedit[0]->setFocus();
                        return false;
                    }
                }
                else
                {
                    Amount toPay = m_campaign->getCustomer(m_index2)->getAmountDue();

                    for (int i = 0; i < intspin[0]->value(); ++i)
                    {
                        toPay -= paymentw[i]->amount->amount();
                    }

                    if (toPay < 0)
                    {
                        if (QMessageBox::question(m_parent, nomMessageBox, tr("The amount you entered is higher than the customer's total amount due. Are you sure you want to continue ?"), QMessageBox::Yes | QMessageBox::No) == QMessageBox::No)
                        {
                            tabwidget[0]->setCurrentIndex(0);
                            paymentw[0]->amount->setFocus();
                            return false;
                        }
                    }

                    for (int i = 0; i < intspin[0]->value(); ++i)
                    {
                        if (paymentw[i]->amount->amount() == 0)
                        {
                            QMessageBox::information(m_parent, nomMessageBox, tr("The payment amount cannot be null."));
                            tabwidget[0]->setCurrentIndex(i);
                            paymentw[i]->amount->setFocus();
                            return false;
                        }
                        if (paymentw[i]->pmode->currentIndex() < 0)
                        {
                            QMessageBox::information(m_parent, nomMessageBox, tr("Please select a payment mode."));
                            tabwidget[0]->setCurrentIndex(i);
                            paymentw[i]->pmode->setFocus();
                            return false;
                        }
                        if ( ! paymentw[i]->date->date().isValid())
                        {
                            QMessageBox::information(m_parent, nomMessageBox, tr("Please select a valid date."));
                            tabwidget[0]->setCurrentIndex(i);
                            paymentw[i]->date->setFocus();
                            return false;
                        }
                    }
                }
                returnValue = true;
            }
            break;

        case Type_Expense:
            if (checkPermissions(m_parent, m_campaign, User_Normal, Perm_ManageExpenses))
            {
                nomMessageBox += tr("Expense");

                if (doublespin[0]->amount() == 0)
                {
                    QMessageBox::information(m_parent, nomMessageBox, tr("The amount cannot be null."));
                    doublespin[0]->setFocus();
                    return false;
                }
                if (combo[0]->currentIndex() < 0)
                {
                    QMessageBox::information(m_parent, nomMessageBox, tr("Please select a payment mode."));
                    combo[0]->setFocus();
                    return false;
                }
                if (combo[1]->currentIndex() < 0)
                {
                    QMessageBox::information(m_parent, nomMessageBox, tr("Please select an expense type."));
                    combo[1]->setFocus();
                    return false;
                }
                if ( ! dateedit[0]->date().isValid())
                {
                    QMessageBox::information(m_parent, nomMessageBox, tr("Please select a valid date."));
                    dateedit[0]->setFocus();
                    return false;
                }
                if (textEdit[0]->toPlainText().isEmpty())
                {
                    QMessageBox::information(m_parent, nomMessageBox, tr("You need to enter a reason for the expense."));
                    textEdit[0]->setFocus();
                    return false;
                }
                returnValue = true;
            }
            break;

        case Type_Donation:
            if (checkPermissions(m_parent, m_campaign, User_Normal, Perm_ManageDonations))
            {
                nomMessageBox += tr("Donation");

                if (doublespin[0]->amount() == 0)
                {
                    QMessageBox::information(m_parent, nomMessageBox, tr("The donation amount cannot be null."));
                    doublespin[0]->setFocus();
                    return false;
                }
                if (combo[0]->currentIndex() < 0)
                {
                    QMessageBox::information(m_parent, nomMessageBox, tr("Please select a payment mode."));
                    combo[0]->setFocus();
                    return false;
                }
                if (m_index2 == -1)
                {
                    QMessageBox::information(m_parent, nomMessageBox, tr("Please select a donator."));
                    btnWho->setFocus();
                    return false;
                }
                if ( ! dateedit[0]->date().isValid())
                {
                    QMessageBox::information(m_parent, nomMessageBox, tr("Please select a valid date."));
                    dateedit[0]->setFocus();
                    return false;
                }
                returnValue = true;
            }
            break;

        case Type_PaymentMode:
            if (checkPermissions(m_parent, m_campaign, User_Normal, Perm_ManagePaymentModes))
            {
                nomMessageBox += tr("Payment Mode");

                if (lineEdit[0]->text().isEmpty())
                {
                    QMessageBox::information(m_parent, nomMessageBox, tr("Please enter a name for the mode."));
                    lineEdit[0]->setFocus();
                    return false;
                }
                returnValue = true;
            }
            break;

        case Type_Supplier:
            if (checkPermissions(m_parent, m_campaign, User_Normal, Perm_ManageSuppliers))
            {
                nomMessageBox += tr("Supplier");

                if (lineEdit[0]->text().isEmpty())
                {
                    QMessageBox::information(m_parent, nomMessageBox, tr("Please enter a name for the supplier."));
                    lineEdit[0]->setFocus();
                    return false;
                }
                returnValue = true;
            }
            break;

        case Type_Tax:
            if (checkPermissions(m_parent, m_campaign, User_Normal, Perm_ManageTaxes))
            {
                nomMessageBox += tr("Tax");

                if (lineEdit[0]->text().isEmpty())
                {
                    QMessageBox::information(m_parent, nomMessageBox, tr("Please enter a name for the tax."));
                    lineEdit[0]->setFocus();
                    return false;
                }
                if (lineEdit[1]->text().isEmpty())
                {
                    QMessageBox::information(m_parent, nomMessageBox, tr("Please enter an abbreviation for the tax."));
                    lineEdit[1]->setFocus();
                    return false;
                }
                if (lineEdit[1]->text() == " ")
                {
                    QMessageBox::information(m_parent, nomMessageBox, tr("Please enter a valid abbreviation for the tax."));
                    lineEdit[1]->setFocus();
                    return false;
                }
                if (doublespin[0]->doubleValue() <= 0 || doublespin[0]->doubleValue() >= 100)
                {
                    QMessageBox::information(m_parent, nomMessageBox, tr("Please enter a valid rate between 0 and 100 %."));
                    doublespin[0]->setFocus();
                    return false;
                }

                returnValue = true;
            }
            break;

        case Type_User:
            if (checkPermissions(m_parent, m_campaign, User_Admin))
            {
                nomMessageBox += tr("User");

                if (lineEdit[0]->text().isEmpty())
                {
                    QMessageBox::information(m_parent, nomMessageBox, tr("Please enter a name for the user."));
                    lineEdit[0]->setFocus();
                    return false;
                }
                if (lineEdit[1]->text().isEmpty())
                {
                    QMessageBox::information(m_parent, nomMessageBox, tr("Please enter a first name for the user."));
                    lineEdit[1]->setFocus();
                    return false;
                }
                if (lineEdit[2]->text().isEmpty())
                {
                    QMessageBox::information(m_parent, nomMessageBox, tr("Please enter a username for the user."));
                    lineEdit[2]->setFocus();
                    return false;
                }
                if (( ! m_modification) && lineEdit[3]->text() != lineEdit[4]->text())
                {
                    QMessageBox::information(m_parent, nomMessageBox, tr("Passwords do not match."));
                    lineEdit[3]->setFocus();
                    return false;
                }

                returnValue = true;
            }
            break;

        case Type_Unit:
            if (checkPermissions(m_parent, m_campaign, User_Admin))
            {
                nomMessageBox += tr("Unit");

                if (lineEdit[0]->text().isEmpty())
                {
                    QMessageBox::information(m_parent, nomMessageBox, tr("Please enter a name for the unit."));
                    lineEdit[0]->setFocus();
                    return false;
                }
                if (lineEdit[1]->text().isEmpty())
                {
                    QMessageBox::information(m_parent, nomMessageBox, tr("Please enter an abbreviation for the unit."));
                    lineEdit[1]->setFocus();
                    return false;
                }
                if (intspin[0]->value() < 0 || intspin[0]->value() > 4)
                {
                    QMessageBox::information(m_parent, nomMessageBox, tr("Please enter a valid precision (0 to 4)."));
                    intspin[0]->setFocus();
                    return false;
                }
                if (lineEdit[2]->text().isEmpty() || lineEdit[2]->text().toFloat() == 0)
                {
                    QMessageBox::information(m_parent, nomMessageBox, tr("Please enter a valid conversion factor."));
                    lineEdit[2]->setFocus();
                    return false;
                }

                //Check for same abbreviations
                if ( ! m_modification)
                {
                    int i = 0;
                    bool found = false;

                    while (i < m_campaign->getNumberOfUnits() && ( ! found))
                    {
                        if (m_campaign->getUnit(i)->getAbbreviation() == lineEdit[1]->text())
                        {
                            found = true;
                        }

                        ++i;
                    }

                    if (found)
                    {
                        QMessageBox::information(m_parent, nomMessageBox, tr("This abbreviation is already used by another unit."));
                        lineEdit[1]->setFocus();
                        return false;
                    }
                }


                returnValue = true;
            }
            break;

        case Type_Currency:
            if (checkPermissions(m_parent, m_campaign, User_Admin))
            {
                nomMessageBox += tr("Currency");

                if (lineEdit[0]->text().isEmpty())
                {
                    QMessageBox::information(m_parent, nomMessageBox, tr("Please enter a name for the currency."));
                    lineEdit[0]->setFocus();
                    return false;
                }
                if ( ! lineEdit[1]->hasAcceptableInput())
                {
                    QMessageBox::information(m_parent, nomMessageBox, tr("Please enter a valid ISO Code."));
                    lineEdit[1]->setFocus();
                    return false;
                }
                if ( ! lineEdit[2]->hasAcceptableInput())
                {
                    QMessageBox::information(m_parent, nomMessageBox, tr("Please enter a valid currency sign."));
                    lineEdit[2]->setFocus();
                    return false;
                }
                returnValue = true;
            }
            break;

        default:
            break;
    }

    return returnValue;
}

void CAMSEGObjectModifier::fillData()
{
    switch (m_type)
    {
        case Type_Customer:
        {
            Customer* temp = m_campaign->getCustomer(m_objectInternalID);
            lineEdit[0]->setText(temp->getName());
            lineEdit[1]->setText(temp->getFirstName());

            if ( ! temp->getCategory()->isActive())
            {
                combo[0]->addItem(temp->getCategory()->getName());
                m_listIDs->append(temp->getCategory()->getInternalId());
                combo[0]->setCurrentIndex(combo[0]->count() - 1);
            }
            else
            {
                bool found = false;
                int i = 0;

                while (i < combo[0]->count() && ( ! found))
                {
                    if (m_listIDs->at(i) == temp->getCategory()->getInternalId())
                    {
                        combo[0]->setCurrentIndex(i);
                        found = true;
                    }
                    ++i;
                }
            }

            combo[1]->setCurrentIndex(sexToInt(temp->getSex()));
            combo[2]->setCurrentIndex(statusToInt(temp->isActive()));
            lineEdit[2]->setText(temp->getFormattedId());
            textEdit[0]->setText(temp->getInformations());

            (*m_location) = (*(temp->getLocation()));
            (*m_loginInfos) = (*(temp->getLoginInfos()));
            m_picture = temp->getPicture();

            break;
        }
        case Type_Donator:
        {
            Donator* temp = m_campaign->getDonator(m_objectInternalID);
            lineEdit[0]->setText(temp->getName());
            lineEdit[1]->setText(temp->getFirstName());

            combo[1]->setCurrentIndex(sexToInt(temp->getSex()));
            combo[2]->setCurrentIndex(statusToInt(temp->isActive()));
            textEdit[0]->setText(temp->getInformations());

            (*m_location) = (*(temp->getLocation()));
            m_picture = temp->getPicture();

            break;
        }
        case Type_Vendor:
        {
            Vendor* temp = m_campaign->getVendor(m_objectInternalID);
            lineEdit[0]->setText(temp->getName());
            lineEdit[1]->setText(temp->getFirstName());

            if ( ! temp->getCategory()->isActive())
            {
                combo[0]->addItem(temp->getCategory()->getName());
                m_listIDs->append(temp->getCategory()->getInternalId());
                combo[0]->setCurrentIndex(combo[0]->count() - 1);
            }
            else
            {
                bool found = false;
                int i = 0;

                while (i < combo[0]->count() && ( ! found))
                {
                    if (m_listIDs->at(i) == temp->getCategory()->getInternalId())
                    {
                        combo[0]->setCurrentIndex(i);
                        found = true;
                    }
                    ++i;
                }
            }

            combo[1]->setCurrentIndex(sexToInt(temp->getSex()));
            combo[2]->setCurrentIndex(statusToInt(temp->isActive()));
            lineEdit[2]->setText(temp->getFormattedId());
            textEdit[0]->setText(temp->getInformations());

            (*m_location) = (*(temp->getLocation()));
            (*m_loginInfos) = (*(temp->getLoginInfos()));
            m_picture = temp->getPicture();

            break;
        }
        case Type_Product:
        {
            Product* temp = m_campaign->getProduct(m_objectInternalID);
            lineEdit[0]->setText(temp->getName());            
            doublespin[0]->setAmount(temp->getBasePrice());
            qDebug() << "doublespin[0]->setAmount(temp->getBasePrice())" <<  doublespin[0]->amount().toString() << " _ " <<temp->getBasePrice().toString();
            doublespin[1]->setAmount(temp->getPriceSold());
            qDebug() << "doublespin[1]->setAmount(temp->getSPrice())" <<  doublespin[1]->amount().toString() << " _ " <<temp->getPriceSold().toString();
            doublespin[2]->setQuantity(temp->getStdDeliveryQuantity());
            doublespin[3]->setQuantity(temp->getSecurityStock());
            doublespin[4]->setQuantity(temp->getStdSellingQuantity());
            checkbox[1]->setChecked(temp->canBeSold());

            if ( ! temp->getCategory()->isActive())
            {
                combo[0]->addItem(temp->getCategory()->getName());
                m_listIDs->append(temp->getCategory()->getInternalId());
                combo[0]->setCurrentIndex(combo[0]->count() - 1);
            }
            else
            {
                bool found = false;
                int i = 0;

                while (i < combo[0]->count() && ( ! found))
                {
                    if (m_listIDs->at(i) == temp->getCategory()->getInternalId())
                    {
                        combo[0]->setCurrentIndex(i);
                        found = true;
                    }
                    ++i;
                }
            }

            if ( ! temp->getSupplier()->isActive())
            {
                combo[1]->addItem(temp->getSupplier()->getName());
                m_listIDs->append(temp->getSupplier()->getInternalId());
                combo[1]->setCurrentIndex(combo[1]->count() - 1);
            }
            else
            {
                bool found = false;
                int i = 0;

                while (i < combo[1]->count() && ( ! found))
                {
                    if (m_listIDs2->at(i) == temp->getSupplier()->getInternalId())
                    {
                        combo[1]->setCurrentIndex(i);
                        found = true;
                    }
                    ++i;
                }
            }

            //Taxes
            for (int i = 0; i < list[0]->count(); ++i)
            {
                if (temp->taxIsApplicable(m_campaign->getTax(m_listIDs4->at(i))->getId()))
                {
                    list[0]->item(i)->setCheckState(Qt::Checked);
                }
                else
                {
                    list[0]->item(i)->setCheckState(Qt::Unchecked);
                }
            }

            combo[2]->setCurrentIndex(statusToInt(temp->isActive()));
            lineEdit[1]->setText(temp->getFormattedId());
            lineEdit[2]->setText(temp->getSupplierId());
            textEdit[0]->setText(temp->getInformations());

            m_picture = temp->getPicture();

            break;
        }
        case Type_Service:
        {
            Service* temp = m_campaign->getService(m_objectInternalID);
            lineEdit[0]->setText(temp->getName());
            doublespin[0]->setAmount(temp->getCosts());
            doublespin[1]->setAmount(temp->getPriceSold());

            combo[0]->setCurrentIndex(Service::quantityFormatToInt(temp->getQuantityFormat()));
            combo[2]->setCurrentIndex(statusToInt(temp->isActive()));
            lineEdit[1]->setText(temp->getFormattedId());
            textEdit[0]->setText(temp->getInformations());

            //Taxes
            for (int i = 0; i < list[0]->count(); ++i)
            {
                if (temp->taxIsApplicable(m_campaign->getTax(m_listIDs->at(i))->getId()))
                {
                    list[0]->item(i)->setCheckState(Qt::Checked);
                }
                else
                {
                    list[0]->item(i)->setCheckState(Qt::Unchecked);
                }
            }

            m_picture = temp->getPicture();
            m_buttonContent = temp->getPOSButton();

            break;
        }
        case Type_CustomerCat:
        {
            CustomerCat* temp = m_campaign->getCustomerCat(m_objectInternalID);
            lineEdit[0]->setText(temp->getName());
            combo[0]->setCurrentIndex(statusToInt(temp->isActive()));
            textEdit[0]->setText(temp->getInformations());
            lineEdit[1]->setText(temp->getFormattedId());

            if (temp->hasParentCat())
                combo[1]->setCurrentIndex(m_listIDs->indexOf(temp->getParentCat()->getInternalId()));

            break;
        }
        case Type_VendorCat:
        {
            VendorCat* temp = m_campaign->getVendorCat(m_objectInternalID);
            lineEdit[0]->setText(temp->getName());
            combo[0]->setCurrentIndex(statusToInt(temp->isActive()));
            textEdit[0]->setText(temp->getInformations());
            lineEdit[1]->setText(temp->getFormattedId());

            if (temp->hasParentCat())
                combo[1]->setCurrentIndex(m_listIDs->indexOf(temp->getParentCat()->getInternalId()));

            break;
        }
        case Type_ProductCat:
        {
            ProductCat* temp = m_campaign->getProductCat(m_objectInternalID);
            lineEdit[0]->setText(temp->getName());
            combo[0]->setCurrentIndex(statusToInt(temp->isActive()));
            textEdit[0]->setText(temp->getInformations());
            lineEdit[1]->setText(temp->getFormattedId());

            if (temp->hasParentCat())
                combo[1]->setCurrentIndex(m_listIDs->indexOf(temp->getParentCat()->getInternalId()));

            break;
        }
        case Type_PLUCat:
        {
            PLUCat* temp = m_campaign->getPLUCat(m_objectInternalID);
            lineEdit[0]->setText(temp->getName());
            combo[0]->setCurrentIndex(statusToInt(temp->isActive()));
            textEdit[0]->setText(temp->getInformations());
            lineEdit[1]->setText(temp->getFormattedId());

            if (temp->hasParentCat())
                combo[1]->setCurrentIndex(m_listIDs->indexOf(temp->getParentCat()->getInternalId()));

            break;
        }
        case Type_Payment:
        {
            Payment* temp = m_campaign->getPayment(m_objectInternalID);
            doublespin[0]->setAmount(temp->getAmount());
            dateedit[0]->setDate(temp->getDate());

            if ( ! temp->getPaymentMode()->isActive())
            {
                combo[0]->addItem(temp->getPaymentMode()->getName());
                m_listIDs->append(temp->getPaymentMode()->getInternalId());
                combo[0]->setCurrentIndex(combo[0]->count() - 1);
            }
            else
            {
                bool found = false;
                int i = 0;

                while (i < combo[0]->count() && ( ! found))
                {
                    if (m_listIDs->at(i) == temp->getPaymentMode()->getInternalId())
                    {
                        combo[0]->setCurrentIndex(i);
                        found = true;
                    }
                    ++i;
                }
            }

            textEdit[0]->setText(temp->getInformations());

            break;
        }
        case Type_Expense:
        {
            Expense* temp = m_campaign->getExpense(m_objectInternalID);
            doublespin[0]->setAmount(temp->getAmount());
            dateedit[0]->setDate(temp->getDate());
            combo[1]->setCurrentIndex((int) temp->getType());

            if ( ! temp->getPaymentMode()->isActive())
            {
                combo[0]->addItem(temp->getPaymentMode()->getName());
                m_listIDs->append(temp->getPaymentMode()->getInternalId());
                combo[0]->setCurrentIndex(combo[0]->count() - 1);
            }
            else
            {
                bool found = false;
                int i = 0;

                while (i < combo[0]->count() && ( ! found))
                {
                    if (m_listIDs->at(i) == temp->getPaymentMode()->getInternalId())
                    {
                        combo[0]->setCurrentIndex(i);
                        found = true;
                    }
                    ++i;
                }
            }

            textEdit[0]->setText(temp->getInformations());

            break;
        }
        case Type_Donation:
        {
            Donation* temp = m_campaign->getDonation(m_objectInternalID);
            doublespin[0]->setAmount(temp->getAmount());
            dateedit[0]->setDate(temp->getDate());

            if ( ! temp->getPaymentMode()->isActive())
            {
                combo[0]->addItem(temp->getPaymentMode()->getName());
                m_listIDs->append(temp->getPaymentMode()->getInternalId());
                combo[0]->setCurrentIndex(combo[0]->count() - 1);
            }
            else
            {
                bool found = false;
                int i = 0;

                while (i < combo[0]->count() && ( ! found))
                {
                    if (m_listIDs->at(i) == temp->getPaymentMode()->getInternalId())
                    {
                        combo[0]->setCurrentIndex(i);
                        found = true;
                    }
                    ++i;
                }
            }

            textEdit[0]->setText(temp->getInformations());

            break;
        }

        case Type_PaymentMode:
        {
            PaymentMode* temp = m_campaign->getPaymentMode(m_objectInternalID);
            lineEdit[0]->setText(temp->getName());
            combo[0]->setCurrentIndex(statusToInt(temp->isActive()));
            textEdit[0]->setText(temp->getInformations());
            m_picture = temp->getPicture();

            break;
        }
        case Type_Supplier:
        {
            Supplier* temp = m_campaign->getSupplier(m_objectInternalID);
            lineEdit[0]->setText(temp->getName());
            combo[0]->setCurrentIndex(statusToInt(temp->isActive()));
            textEdit[0]->setText(temp->getInformations());
            lineEdit[1]->setText(temp->getContactName());

            (*m_location) = (*(temp->getLocation()));
            m_picture = temp->getPicture();

            break;
        }
        case Type_Tax:
        {
            Tax* temp = m_campaign->getTax(m_objectInternalID);
            lineEdit[0]->setText(temp->getName());
            lineEdit[1]->setText(QChar(temp->getSingleLetterAbbreviation()));
            doublespin[0]->setDoubleValue(temp->getRate());
            checkbox[0]->setChecked(temp->isDefaultSelected());
            combo[0]->setCurrentIndex(statusToInt(temp->isActive()));
            textEdit[0]->setText(temp->getInformations());

            break;
        }
        case Type_User:
        {
            User* temp = m_campaign->getUser(m_objectInternalID);
            lineEdit[0]->setText(temp->getName());
            lineEdit[1]->setText(temp->getFirstName());
            lineEdit[2]->setText(temp->getLoginInfos()->login);
            textEdit[0]->setText(temp->getInformations());

            (*m_loginInfos) = (*(temp->getLoginInfos()));

            if (m_campaign->getUser(m_objectInternalID)->getType() == User_Normal)
                (*m_perm) = (*(temp->getPermissions()));

            break;
        }
        case Type_Unit:
        {
            Unit* temp = m_campaign->getUnit(m_objectInternalID);
            lineEdit[0]->setText(temp->getName());
            lineEdit[1]->setText(temp->getAbbreviation());
            lineEdit[2]->setText(QString::number(temp->getConversionFactorToBase()));
            textEdit[0]->setText(temp->getInformations());
            intspin[0]->setValue(temp->getPrecision());

            break;
        }
        case Type_Currency:
        {
            lineEdit[0]->setText(m_currency->getName());
            lineEdit[1]->setText(m_currency->getISOCode());
            lineEdit[2]->setText(m_currency->getSign());
            combo[0]->setCurrentIndex((int) m_currency->getSignPosition());
            textEdit[0]->setText(m_currency->getInformations());

            break;
        }

        default:
            break;
    }
}

void CAMSEGObjectModifier::reloadUI()
{
    switch (m_type)
    {
        case Type_Customer:
        {
            int i1 = combo[0]->currentIndex();
            fillComboBox(combo[0], m_listIDs, CustomerCatFill, m_campaign);
            combo[0]->setCurrentIndex(i1);
            break;
        }
        case Type_Vendor:
        {
            int i1 = combo[0]->currentIndex();
            fillComboBox(combo[0], m_listIDs, VendorCatFill, m_campaign);
            combo[0]->setCurrentIndex(i1);
            break;
        }
        case Type_Service:
        {
            fillTaxList(list[0], m_listIDs, m_campaign);
            break;
        }
        case Type_Product:
        {
            int i1 = combo[0]->currentIndex();
            int i2 = combo[1]->currentIndex();
            fillComboBox(combo[0], m_listIDs, ProductCatFill, m_campaign);
            fillComboBox(combo[1], m_listIDs2, SupplierFill, m_campaign);
            combo[0]->setCurrentIndex(i1);
            combo[1]->setCurrentIndex(i2);
            fillTaxList(list[0], m_listIDs4, m_campaign);

            if ( ! m_modification)
            {
                fillComboBox(combo[3], m_listIDs3, UnitFill, m_campaign);
            }
            break;
        }
        case Type_CustomerCat:
        {
            fillComboBox(combo[1], m_listIDs, CustomerCatFill, m_campaign);
            m_listIDs->prepend(-1);
            combo[1]->insertItem(0, tr("None"));
            combo[1]->setCurrentIndex(0);
            break;
        }
        case Type_VendorCat:
        {
            fillComboBox(combo[1], m_listIDs, VendorCatFill, m_campaign);
            m_listIDs->prepend(-1);
            combo[1]->insertItem(0, tr("None"));
            combo[1]->setCurrentIndex(0);
            break;
        }
        case Type_ProductCat:
        {
            fillComboBox(combo[1], m_listIDs, ProductCatFill, m_campaign);
            m_listIDs->prepend(-1);
            combo[1]->insertItem(0, tr("None"));
            combo[1]->setCurrentIndex(0);
            break;
        }
        case Type_PLUCat:
        {
            fillComboBox(combo[1], m_listIDs, PLUCatFill, m_campaign);
            m_listIDs->prepend(-1);
            combo[1]->insertItem(0, tr("None"));
            combo[1]->setCurrentIndex(0);
            break;
        }
        default:
            break;
    }

}

void CAMSEGObjectModifier::showBottomLine(bool p_show)
{
    bottomLine->setVisible(p_show);
}

void CAMSEGObjectModifier::loadUI()
{
    //Object Creation
    for (unsigned int i = 0; i < MAX_LABEL; ++i)
    {
        label[i] = new QLabel();
    }
    for (unsigned int i = 0; i < MAX_LINE; ++i)
    {
        lineEdit[i] = new QLineEdit();
    }
    for (unsigned int i = 0; i < MAX_TEXT; ++i)
    {
        textEdit[i] = new QTextEdit();
        textEdit[i]->setTabChangesFocus(true);
        textEdit[i]->setMaximumHeight(75);
        textEdit[i]->setAcceptRichText(false);
    }
    for (unsigned int i = 0; i < MAX_COMBO; ++i)
    {
        combo[i] = new QComboBox();
    }
    for (unsigned int i = 0; i < MAX_LIST; ++i)
    {
        list[i] = new QListWidget();
        list[i]->setSelectionMode(QAbstractItemView::MultiSelection);
        list[i]->setMaximumHeight(75);
    }
    for (unsigned int i = 0; i < MAX_DSPIN; ++i)
    {
        doublespin[i] = new AmountEdit();
        doublespin[i]->setMaximum(MAX_VALUE_SPINBOX);
        doublespin[i]->setPrefix(m_campaign->getCurrency()->getSign() + " ");
    }
    for (unsigned int i = 0; i < MAX_CHK; ++i)
    {
        checkbox[i] = new QCheckBox();
    }
    for (unsigned int i = 0; i < MAX_DATE; ++i)
    {
        dateedit[i] = new QDateEdit();
        dateedit[i]->setCalendarPopup(true);
        dateedit[i]->setDate(QDate::currentDate());
    }
    for (unsigned int i = 0; i < MAX_INTSPIN; ++i)
    {
        intspin[i] = new QSpinBox();
    }
    for (unsigned int i = 0; i < MAX_PAYMENT; ++i)
    {
        paymentw[i] = new PaymentWidget(m_campaign);
    }
    for (unsigned int i = 0; i < MAX_TABWIDGET; ++i)
    {
        tabwidget[i] = new QTabWidget();
    }

    QFont boldFont;
    boldFont.setBold(true);

    QValidator *idValidator = new QRegExpValidator(QRegExp("^[\\S]*$"), this);
    QSettings settings;

    bottomLine = new QFrame();
    bottomLine->setFrameShape(QFrame::HLine);
    bottomLine->setVisible(false);

    btnLocation = new QPushButton(QIcon(PATH_ICONS + "location.png"), tr("C&ontact Infos"));
    btnLoginInfos = new QPushButton(QIcon(PATH_ICONS + "logininfos.png"), tr("Lo&gin Infos"));
    btnPassword = new QPushButton(QIcon(PATH_ICONS + "logininfos.png"), tr("&Change Password"));
    btnPicture = new QPushButton(QIcon(PATH_ICONS + "image.png"), tr("&Picture"));
    btnPermissions = new QPushButton(QIcon(PATH_ICONS + "permissions.png"), tr("P&ermissions"));
    btnWho = new QPushButton(QIcon(PATH_ICONS + "donator.png"), tr("Select &Who"));
    btnPOSButton = new QPushButton(QIcon(PATH_ICONS + "button.png"), tr("POS Button"));

    connect(btnLocation, SIGNAL(clicked()), this, SLOT(modifyLocation()));
    connect(btnLoginInfos, SIGNAL(clicked()), this, SLOT(modifyLoginInfos()));
    connect(btnPassword, SIGNAL(clicked()), this, SLOT(changePassword()));
    connect(btnPicture, SIGNAL(clicked()), this, SLOT(modifyPicture()));
    connect(btnPermissions, SIGNAL(clicked()), this, SLOT(modifyPermissions()));
    connect(btnWho, SIGNAL(clicked()), this, SLOT(selectDonator()));
    connect(btnPOSButton, SIGNAL(clicked()), this, SLOT(modifyPOSButton()));

    m_location = new Location;
    m_loginInfos = new LoginInfos;
    m_perm = new Permissions(*m_campaign->getPermissions());

    m_buttonContent.backgroundColor = Qt::lightGray;
    m_buttonContent.style = POSButtonContent::TextButton;

    //Layouts
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    QFormLayout* editLayout = new QFormLayout();
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    QHBoxLayout* otherButtons = new QHBoxLayout();

    if (m_modification)
    {
        buttonLayout->addWidget(btnModify);
        buttonLayout->addWidget(btnCancel);
    }
    else
    {
        buttonLayout->addWidget(btnAdd);
        buttonLayout->addWidget(btnClear);
    }

    buttonLayout->setAlignment(Qt::AlignRight);

    mainLayout->addLayout(editLayout);
    mainLayout->addWidget(bottomLine);
    mainLayout->addLayout(buttonLayout);

    //Edit Widgets

    switch (m_type)
    {
        case Type_Customer:
            otherButtons->addWidget(btnLocation);
            otherButtons->addWidget(btnLoginInfos);
            otherButtons->addWidget(btnPicture);

            delete lineEdit[2];
            lineEdit[2] = new CodeEdit();

            editLayout->addRow(tr("Customer Code :"), lineEdit[2]);
            editLayout->addRow(tr("Name :"), lineEdit[0]);
            editLayout->addRow(tr("First Name :"), lineEdit[1]);
            editLayout->addRow(tr("Category :"), combo[0]);
            editLayout->addRow(tr("Sex :"), combo[1]);
            editLayout->addRow(tr("Status :"), combo[2]);
            if (m_modification)
            {
                editLayout->addRow(tr("Additional Infos :"), textEdit[0]);
            }
            editLayout->addRow(otherButtons);            

            fillComboBox(combo[0], m_listIDs, CustomerCatFill, m_campaign);
            fillComboBox(combo[1], NULL, SexFill, m_campaign);
            fillComboBox(combo[2], NULL, ActivityStatusFill, m_campaign);

            lineEdit[2]->setFont(boldFont);
            lineEdit[2]->setValidator(idValidator);

            if (m_modification)
            {
                lineEdit[0]->setEnabled(m_campaign->allowNameModification());
                lineEdit[1]->setEnabled(m_campaign->allowNameModification());
                lineEdit[2]->setEnabled(m_campaign->allowIDModification());
            }

            break;

        case Type_Donator:
            otherButtons->addWidget(btnLocation);
            otherButtons->addWidget(btnPicture);

            editLayout->addRow(tr("Name :"), lineEdit[0]);
            editLayout->addRow(tr("First Name :"), lineEdit[1]);
            editLayout->addRow(tr("Sex :"), combo[1]);
            editLayout->addRow(tr("Status :"), combo[2]);
            if (m_modification)
            {
                editLayout->addRow(tr("Additional Infos :"), textEdit[0]);
            }
            editLayout->addRow(otherButtons);

            fillComboBox(combo[1], NULL, SexFill, m_campaign);
            fillComboBox(combo[2], NULL, ActivityStatusFill, m_campaign);

            if (m_modification)
            {
                lineEdit[0]->setEnabled(m_campaign->allowNameModification());
                lineEdit[1]->setEnabled(m_campaign->allowNameModification());
            }

            break;

        case Type_Vendor:
            otherButtons->addWidget(btnLocation);
            otherButtons->addWidget(btnLoginInfos);
            otherButtons->addWidget(btnPicture);

            delete lineEdit[2];
            lineEdit[2] = new CodeEdit();

            editLayout->addRow(tr("Vendor Code :"), lineEdit[2]);
            editLayout->addRow(tr("Name :"), lineEdit[0]);
            editLayout->addRow(tr("First Name :"), lineEdit[1]);
            editLayout->addRow(tr("Category :"), combo[0]);
            editLayout->addRow(tr("Sex :"), combo[1]);
            editLayout->addRow(tr("Status :"), combo[2]);
            if (m_modification)
            {
                editLayout->addRow(tr("Additional Infos :"), textEdit[0]);
            }
            editLayout->addRow(otherButtons);

            fillComboBox(combo[0], m_listIDs, VendorCatFill, m_campaign);
            fillComboBox(combo[1], NULL, SexFill, m_campaign);
            fillComboBox(combo[2], NULL, ActivityStatusFill, m_campaign);

            lineEdit[2]->setFont(boldFont);
            lineEdit[2]->setValidator(idValidator);

            if (m_modification)
            {
                lineEdit[0]->setEnabled(m_campaign->allowNameModification());
                lineEdit[1]->setEnabled(m_campaign->allowNameModification());
                lineEdit[2]->setEnabled(m_campaign->allowIDModification());
            }

            break;

        case Type_Product:
        {
            otherButtons->addWidget(btnPicture);

            delete lineEdit[1];
            delete lineEdit[2];
            lineEdit[1] = new CodeEdit();
            lineEdit[2] = new CodeEdit();


            QWidget* tab1 = new QWidget();
            QWidget* tab2 = new QWidget();
            QFormLayout* layoutTab1 = new QFormLayout(tab1);
            QFormLayout* layoutTab2 = new QFormLayout(tab2);

            tabwidget[0]->addTab(tab1, tr("Main"));
            tabwidget[0]->addTab(tab2, tr("Other"));

            editLayout->addRow(tabwidget[0]);


            layoutTab1->addRow(tr("Product SKU :"), lineEdit[1]);
            layoutTab1->addRow(tr("Supplier SKU :"), lineEdit[2]);
            layoutTab1->addRow(tr("Name :"), lineEdit[0]);
            layoutTab1->addRow(tr("Cost Price :"), doublespin[0]);
            layoutTab1->addRow(tr("Rec. Price Sold :"), doublespin[1]);
            layoutTab1->addRow(tr("Category :"), combo[0]);
            layoutTab1->addRow(tr("Supplier :"), combo[1]);

            if ( ! m_modification)
            {
                layoutTab1->addRow(tr("Unit :"), combo[3]);
            }
            else
            {
                layoutTab1->addRow(tr("Description :"), textEdit[0]);
            }

            layoutTab1->addRow(otherButtons);

            layoutTab2->addRow(tr("Std. Del. Qty :"), doublespin[2]);
            layoutTab2->addRow(tr("Std. Selling. Qty :"), doublespin[4]);
            layoutTab2->addRow(tr("Security Stock :"), doublespin[3]);
            layoutTab2->addRow(tr("Status :"), combo[2]);
            layoutTab2->addRow(tr("Can Be Sold :"), checkbox[1]);
            layoutTab2->addRow(tr("Taxes :"), list[0]);

            checkbox[1]->setChecked(settings.value("General/Default/ProductCanBeSold").toInt() == 1);

            doublespin[2]->setPrecision(4);
            doublespin[2]->setQuantity(1.0);
            doublespin[2]->setPrefix("");

            doublespin[3]->setPrecision(4);
            doublespin[3]->setPrefix("");
            doublespin[3]->setQuantity(0.0);

            doublespin[4]->setPrecision(4);
            doublespin[4]->setQuantity(1.0);
            doublespin[4]->setPrefix("");

            if (m_modification)
            {
                doublespin[2]->setSuffix(" " + m_campaign->getProduct(m_objectInternalID)->getUnit()->getAbbreviation());
                doublespin[2]->setPrecision(m_campaign->getProduct(m_objectInternalID)->getUnit()->getPrecision());

                doublespin[3]->setSuffix(" " + m_campaign->getProduct(m_objectInternalID)->getUnit()->getAbbreviation());
                doublespin[3]->setPrecision(m_campaign->getProduct(m_objectInternalID)->getUnit()->getPrecision());

                doublespin[4]->setSuffix(" " + m_campaign->getProduct(m_objectInternalID)->getUnit()->getAbbreviation());
                doublespin[4]->setPrecision(m_campaign->getProduct(m_objectInternalID)->getUnit()->getPrecision());
            }
            else
            {
                layoutTab1->addRow(checkbox[0]);
                checkbox[0]->setText(tr("Create New PLU"));
                checkbox[0]->setToolTip(tr("If this option is enabled, a new PLU with the same name specified here and containg this product (QTY = 1) will be created and added to the campaign."));
            }


            fillComboBox(combo[0], m_listIDs, ProductCatFill, m_campaign);
            fillComboBox(combo[1], m_listIDs2, SupplierFill, m_campaign);
            fillComboBox(combo[2], NULL, ActivityStatusFill, m_campaign);
            fillTaxList(list[0], m_listIDs4, m_campaign);

            lineEdit[1]->setFont(boldFont);
            lineEdit[1]->setValidator(idValidator);

            if ( ! m_modification)
            {
                fillComboBox(combo[3], m_listIDs3, UnitFill, m_campaign);
            }

            if (m_modification)
            {
                lineEdit[1]->setEnabled(m_campaign->allowIDModification());
            }

            break;
        }
        case Type_Service:
            otherButtons->addWidget(btnPicture);
            otherButtons->addWidget(btnPOSButton);

            delete lineEdit[1];
            lineEdit[1] = new CodeEdit();

            editLayout->addRow(tr("Service Code :"), lineEdit[1]);
            editLayout->addRow(tr("Name :"), lineEdit[0]);
            editLayout->addRow(tr("Quantity Format :"), combo[0]);
            editLayout->addRow(tr("Costs :"), doublespin[0]);
            editLayout->addRow(tr("Price Sold :"), doublespin[1]);

            if (m_modification)
            {
                editLayout->addRow(tr("Status :"), combo[2]);
            }

            editLayout->addRow(tr("Taxes :"), list[0]);

            if (m_modification)
            {
                editLayout->addRow(tr("Description :"), textEdit[0]);
            }

            editLayout->addRow(otherButtons);

            if (m_modification)
            {
                lineEdit[1]->setEnabled(m_campaign->allowIDModification());
            }

            fillComboBox(combo[0], NULL, QuantityFormatFill, m_campaign);
            fillComboBox(combo[2], NULL, ActivityStatusFill, m_campaign);
            fillTaxList(list[0], m_listIDs, m_campaign);

            lineEdit[1]->setFont(boldFont);
            lineEdit[1]->setValidator(idValidator);

            break;

        case Type_CustomerCat:

            delete lineEdit[1];
            lineEdit[1] = new CodeEdit();

            editLayout->addRow(tr("Code :"), lineEdit[1]);
            editLayout->addRow(tr("Name :"), lineEdit[0]);
            editLayout->addRow(tr("Status :"), combo[0]);
            editLayout->addRow(tr("Parent :"), combo[1]);

            lineEdit[1]->setFont(boldFont);
            lineEdit[1]->setValidator(idValidator);

            if (m_modification)
            {
                editLayout->addRow(tr("Additional Infos :"), textEdit[0]);
                lineEdit[1]->setEnabled(m_campaign->allowIDModification());
            }

            fillComboBox(combo[0], NULL, ActivityStatusFill, m_campaign);
            fillComboBox(combo[1], m_listIDs, CustomerCatFill, m_campaign);

            m_listIDs->prepend(-1);
            combo[1]->insertItem(0, tr("None"));
            combo[1]->setCurrentIndex(0);

            break;

        case Type_VendorCat:

            delete lineEdit[1];
            lineEdit[1] = new CodeEdit();

            editLayout->addRow(tr("Code :"), lineEdit[1]);
            editLayout->addRow(tr("Name :"), lineEdit[0]);
            editLayout->addRow(tr("Status :"), combo[0]);
            editLayout->addRow(tr("Parent :"), combo[1]);

            lineEdit[1]->setFont(boldFont);
            lineEdit[1]->setValidator(idValidator);

            if (m_modification)
            {
                editLayout->addRow(tr("Additional Infos :"), textEdit[0]);
                lineEdit[1]->setEnabled(m_campaign->allowIDModification());
            }

            fillComboBox(combo[0], NULL, ActivityStatusFill, m_campaign);
            fillComboBox(combo[1], m_listIDs, VendorCatFill, m_campaign);

            m_listIDs->prepend(-1);
            combo[1]->insertItem(0, tr("None"));
            combo[1]->setCurrentIndex(0);

            break;

        case Type_ProductCat:

            delete lineEdit[1];
            lineEdit[1] = new CodeEdit();

            editLayout->addRow(tr("Code :"), lineEdit[1]);
            editLayout->addRow(tr("Name :"), lineEdit[0]);
            editLayout->addRow(tr("Status :"), combo[0]);
            editLayout->addRow(tr("Parent :"), combo[1]);

            lineEdit[1]->setFont(boldFont);
            lineEdit[1]->setValidator(idValidator);

            if (m_modification)
            {
                editLayout->addRow(tr("Additional Infos :"), textEdit[0]);
                lineEdit[1]->setEnabled(m_campaign->allowIDModification());
            }

            fillComboBox(combo[0], NULL, ActivityStatusFill, m_campaign);
            fillComboBox(combo[1], m_listIDs, ProductCatFill, m_campaign);

            m_listIDs->prepend(-1);
            combo[1]->insertItem(0, tr("None"));
            combo[1]->setCurrentIndex(0);

            break;

        case Type_PLUCat:

            delete lineEdit[1];
            lineEdit[1] = new CodeEdit();

            editLayout->addRow(tr("Code :"), lineEdit[1]);
            editLayout->addRow(tr("Name :"), lineEdit[0]);
            editLayout->addRow(tr("Status :"), combo[0]);
            editLayout->addRow(tr("Parent :"), combo[1]);

            lineEdit[1]->setFont(boldFont);
            lineEdit[1]->setValidator(idValidator);

            if (m_modification)
            {
                editLayout->addRow(tr("Additional Infos :"), textEdit[0]);
                lineEdit[1]->setEnabled(m_campaign->allowIDModification());
            }

            fillComboBox(combo[0], NULL, ActivityStatusFill, m_campaign);
            fillComboBox(combo[1], m_listIDs, PLUCatFill, m_campaign);

            m_listIDs->prepend(-1);
            combo[1]->insertItem(0, tr("None"));
            combo[1]->setCurrentIndex(0);

            break;

        case Type_Payment:
        {
            editLayout->addRow(tr("Customer :"), label[0]);

            if (m_modification)
            {
                editLayout->addRow(tr("Amount :"), doublespin[0]);
                editLayout->addRow(tr("Payment Mode :"), combo[0]);
                editLayout->addRow(tr("Date :"), dateedit[0]);
                editLayout->addRow(tr("Additional Infos :"), textEdit[0]);

                fillComboBox(combo[0], m_listIDs, PaymentModeFill, m_campaign);
                m_index2 = m_campaign->getPayment(m_objectInternalID)->getCustomer()->getInternalId();
            }
            else
            {
                editLayout->addRow(tr("Payments to add :"), intspin[0]);
                tabwidget[0]->addTab(paymentw[0], "1");
                editLayout->addRow(tabwidget[0]);

                paymentw[0]->amount->setAmount(m_campaign->getCustomer(m_index2)->getAmountDue());

                intspin[0]->setMinimum(1);
                intspin[0]->setMaximum(MAX_PAYMENT);
                intspin[0]->setValue(1);
                connect(intspin[0], SIGNAL(valueChanged(int)), this, SLOT(updatePaymentCount(int)));
            }

            label[0]->setText(m_campaign->getCustomer(m_index2)->getName() + ", " + m_campaign->getCustomer(m_index2)->getFirstName());

            QFont boldFont;
            boldFont.setBold(true);
            label[0]->setFont(boldFont);

            break;
        }

        case Type_Expense:
        {
            editLayout->addRow(tr("Amount :"), doublespin[0]);
            editLayout->addRow(tr("Payment Mode :"), combo[0]);
            editLayout->addRow(tr("Date :"), dateedit[0]);
            editLayout->addRow(tr("Type :"), combo[1]);
            editLayout->addRow(tr("Reason :"), textEdit[0]);

            fillComboBox(combo[0], m_listIDs, PaymentModeFill, m_campaign);
            fillComboBox(combo[1], NULL, ExpenseTypeFill, m_campaign);
            break;
        }

        case Type_Donation:
        {
            otherButtons->addWidget(btnWho);

            editLayout->addRow(tr("Who :"), label[0]);
            editLayout->addRow(tr("Amount :"), doublespin[0]);
            editLayout->addRow(tr("Payment Mode :"), combo[0]);
            editLayout->addRow(tr("Date :"), dateedit[0]);
            editLayout->addRow(tr("Additional Infos :"), textEdit[0]);

            fillComboBox(combo[0], m_listIDs, PaymentModeFill, m_campaign);

            m_index2 = -1;
            isDonator = true;

            if (m_modification)
            {
                isDonator = m_campaign->getDonation(m_objectInternalID)->isDonator();

                if (isDonator)
                {
                    Donator* donator = m_campaign->getDonation(m_objectInternalID)->getDonator();
                    m_index2 = donator->getInternalId();
                    label[0]->setText(donator->getName() + ", " + donator->getFirstName());
                }
                else
                {
                    Customer* cust = m_campaign->getDonation(m_objectInternalID)->getCustomer();
                    m_index2 = cust->getInternalId();
                    label[0]->setText(cust->getName() + ", " + cust->getFirstName());
                }
            }

            editLayout->addRow(otherButtons);

            //label[0]->setText(m_campaign->getCustomer(m_index2)->getName() + ", " + m_campaign->getCustomer(m_index2)->getFirstName());

            QFont boldFont;
            boldFont.setBold(true);
            label[0]->setFont(boldFont);

            break;
        }

        case Type_PaymentMode:
            otherButtons->addWidget(btnPicture);

            editLayout->addRow(tr("Name :"), lineEdit[0]);
            editLayout->addRow(tr("Status :"), combo[0]);

            if (m_modification)
            {
                editLayout->addRow(tr("Additional Infos :"), textEdit[0]);
                editLayout->addRow(otherButtons);
            }
            else
            {
                editLayout->addRow(tr("Picture :"), combo[1]);
                fillPaymentModesImagesList(combo[1]);
            }

            fillComboBox(combo[0], NULL, ActivityStatusFill, m_campaign);

            break;

        case Type_Supplier:
            otherButtons->addWidget(btnLocation);
            otherButtons->addWidget(btnPicture);

            editLayout->addRow(tr("Name :"), lineEdit[0]);
            editLayout->addRow(tr("Status :"), combo[0]);
            editLayout->addRow(tr("Contact Name :"), lineEdit[1]);
            if (m_modification)
            {
                editLayout->addRow(tr("Additional Infos :"), textEdit[0]);
            }
            editLayout->addRow(otherButtons);

            fillComboBox(combo[0], NULL, ActivityStatusFill, m_campaign);

            break;

        case Type_Tax:
            editLayout->addRow(tr("Name :"), lineEdit[0]);
            editLayout->addRow(tr("Abbreviation :"), lineEdit[1]);
            editLayout->addRow(tr("Rate :"), doublespin[0]);
            editLayout->addRow(tr("Default Selected :"), checkbox[0]);
            editLayout->addRow(tr("Status :"), combo[0]);

            doublespin[0]->setPrefix("");
            doublespin[0]->setSuffix(" %");
            doublespin[0]->setMaximum(100);
            doublespin[0]->setPrecision(DECIMALS_TAX);

            lineEdit[1]->setMaxLength(1);
            lineEdit[1]->setInputMask(">A");

            if (m_modification)
            {
                editLayout->addRow(tr("Additional Infos :"), textEdit[0]);
            }

            fillComboBox(combo[0], NULL, ActivityStatusFill, m_campaign);

            break;

        case Type_User:

            editLayout->addRow(tr("Name :"), lineEdit[0]);
            editLayout->addRow(tr("First Name :"), lineEdit[1]);

            if (m_modification)
            {
                editLayout->addRow(tr("Username :"), lineEdit[2]);
                editLayout->addRow(tr("Additional Infos :"), textEdit[0]);
                otherButtons->addWidget(btnPassword);

                if (m_campaign->getUser(m_objectInternalID)->getType() == User_Normal)
                {
                    otherButtons->addWidget(btnPermissions);
                }
            }
            else
            {
                editLayout->addRow(tr("Username :"), lineEdit[2]);
                editLayout->addRow(tr("Password :"), lineEdit[3]);
                editLayout->addRow(tr("Repeat password :"), lineEdit[4]);
                editLayout->addRow(tr("Type :"), combo[0]);

                lineEdit[3]->setEchoMode(QLineEdit::Password);
                lineEdit[4]->setEchoMode(QLineEdit::Password);

                otherButtons->addWidget(btnPermissions);

                connect(lineEdit[0], SIGNAL(textChanged(QString)), this, SLOT(updateUsername()));
                connect(lineEdit[1], SIGNAL(textChanged(QString)), this, SLOT(updateUsername()));
            }

            //Check if the user is not the current logged admin
            if (m_modification && m_campaign->getCurrentUser()->getInternalId() == m_objectInternalID)
                combo[0]->setEnabled(false);

            editLayout->addRow(otherButtons);

            fillComboBox(combo[0], NULL, UserTypeFill, m_campaign);

            break;

        case Type_Unit:
        {
            fillComboBox(combo[0], NULL, UnitTypeFill, m_campaign);

            editLayout->addRow(tr("Name :"), lineEdit[0]);
            editLayout->addRow(tr("Abbreviation :"), lineEdit[1]);
            editLayout->addRow(tr("Precision :"), intspin[0]);
            if ( ! m_modification)
            {
                editLayout->addRow(tr("Type :"), combo[0]);
            }
            editLayout->addRow(tr("Conversion Factor :"), lineEdit[2]);
            editLayout->addRow(tr("Conversion Example :"), label[0]);

            if (m_modification)
            {
                editLayout->addRow(tr("Additional Infos :"), textEdit[0]);
                combo[0]->setCurrentIndex((int) m_campaign->getUnit(m_objectInternalID)->getType());
            }

            intspin[0]->setMinimum(0);
            intspin[0]->setMaximum(4);

            lineEdit[2]->setText("1.0");

            lineEdit[1]->setMaxLength(7);

            QDoubleValidator* validator = new QDoubleValidator(0.0, 100000000.0, 20, this);
            lineEdit[2]->setValidator(validator);

            connect(lineEdit[1], SIGNAL(textChanged(QString)), this, SLOT(updateConversionLabel()));
            connect(lineEdit[2], SIGNAL(textChanged(QString)), this, SLOT(updateConversionLabel()));
            connect(combo[0], SIGNAL(currentIndexChanged(int)), this, SLOT(updateConversionLabel()));


            break;
        }

        case Type_Currency:
        {
            editLayout->addRow(tr("Name :"), lineEdit[0]);
            editLayout->addRow(tr("ISO Code :"), lineEdit[1]);
            editLayout->addRow(tr("Sign :"), lineEdit[2]);
            editLayout->addRow(tr("Sign Position :"), combo[0]);

            if (m_modification)
            {
                editLayout->addRow(tr("Additional Infos :"), textEdit[0]);
            }

            QRegExp regexISO("^[A-Z]{3}$");
            QRegExp regexSign("^.{1,3}$");

            QValidator* ISOValidator = new QRegExpValidator(regexISO, this);
            lineEdit[1]->setValidator(ISOValidator);

            QValidator* signValidator = new QRegExpValidator(regexSign, this);
            lineEdit[2]->setValidator(signValidator);

            fillComboBox(combo[0], NULL, CurrencySignPositionFill, NULL);

            break;
        }

        default:
            break;
        }

}

void CAMSEGObjectModifier::add()
{
    bool ok = false;
    QString errorMsgBoxName = tr("Error");

    switch (m_type)
    {
        case Type_Customer:
            if (validation())
            {
                if(licenseTest30()==19851208) qDebug() << "frmManageCustomers TRUE MAX_RECENT_CAMPAIGNS";
                else
                if(m_campaign->getNumberOfCustomers() >=MAX_RECENT_CAMPAIGNS + 20)
                {
                    qDebug() << "frmManageCustomers FALSE MAX_RECENT_CAMPAIGNS";   return;  //MAX_NBR ***
                }
            //*******************************************************************


                m_campaign->stopSignals();
                Customer* newCust = m_campaign->addCustomer(lineEdit[0]->text(),
                                                            lineEdit[1]->text(),
                                                            m_campaign->getCustomerCat(m_listIDs->at(combo[0]->currentIndex())));

                if (newCust == NULL)
                {
                    m_campaign->startSignals();
                    QMessageBox::warning(this, errorMsgBoxName, tr("Error - Unable to create customer."));
                    return;
                }
                else
                {
                    ok = true;
                }

                newCust->setSex(intToSex(combo[1]->currentIndex()));
                newCust->setStatus(intToStatus(combo[2]->currentIndex()));
                newCust->setLocation(*m_location);
                newCust->setLoginInfos(*m_loginInfos);
                newCust->setPicture(m_picture);
                newCust->setSpecificID(lineEdit[2]->text());
                newCust->save();

                m_campaign->startSignals();
                m_campaign->sendSignal_objectAdded(Type_Customer);
            }

            break;

        case Type_Donator:
            if (validation())
            {
                m_campaign->stopSignals();
                Donator* newDonator = m_campaign->addDonator(lineEdit[0]->text(),
                                                            lineEdit[1]->text());

                if (newDonator == NULL)
                {
                    m_campaign->startSignals();
                    QMessageBox::warning(this, errorMsgBoxName, tr("Error - Unable to create donator."));
                    return;
                }
                else
                {
                    ok = true;
                }

                newDonator->setSex(intToSex(combo[1]->currentIndex()));
                newDonator->setStatus(intToStatus(combo[2]->currentIndex()));
                newDonator->setLocation(*m_location);
                newDonator->setPicture(m_picture);
                newDonator->save();

                m_campaign->startSignals();
                m_campaign->sendSignal_objectAdded(Type_Donator);
            }

            break;

        case Type_Vendor:
            if (validation())
            {
                m_campaign->stopSignals();
                Vendor* newVend = m_campaign->addVendor(lineEdit[0]->text(),
                                                        lineEdit[1]->text(),
                                                        m_campaign->getVendorCat(m_listIDs->at(combo[0]->currentIndex())));

                if (newVend == NULL)
                {
                    m_campaign->startSignals();
                    QMessageBox::warning(this, errorMsgBoxName, tr("Error - Unable to create vendor."));
                    return;
                }
                else
                {
                    ok = true;
                }

                newVend->setSex(intToSex(combo[1]->currentIndex()));
                newVend->setStatus(intToStatus(combo[2]->currentIndex()));
                newVend->setLocation(*m_location);
                newVend->setLoginInfos(*m_loginInfos);
                newVend->setPicture(m_picture);
                newVend->setSpecificID(lineEdit[2]->text());
                newVend->save();

                m_campaign->startSignals();
                m_campaign->sendSignal_objectAdded(Type_Vendor);
            }

            break;

        case Type_Product:
            if (validation())
            {
                m_campaign->stopSignals();
                Product* newProd = m_campaign->addProduct(lineEdit[0]->text(),
                                                        m_campaign->getProductCat(m_listIDs->at(combo[0]->currentIndex())),
                                                        m_campaign->getSupplier(m_listIDs2->at(combo[1]->currentIndex())),
                                                        doublespin[0]->amount(),
                                                        doublespin[1]->amount(),
                                                        textEdit[0]->toPlainText());

                if (newProd == NULL)
                {
                    m_campaign->startSignals();
                    QMessageBox::warning(this, errorMsgBoxName, tr("Error - Unable to create product."));
                    return;
                }
                else
                {
                    ok = true;
                }

                newProd->setStatus(intToStatus(combo[2]->currentIndex()));
                newProd->setPicture(m_picture);
                newProd->setSpecificID(lineEdit[1]->text());
                newProd->setStdDeliveryQuantity(doublespin[2]->quantity());
                newProd->setStdSellingQuantity(doublespin[4]->quantity());
                newProd->setSecurityStock(doublespin[3]->quantity());
                newProd->setSupplierID(lineEdit[2]->text());
                newProd->setCanBeSold(checkbox[1]->isChecked());

                //Taxes
                for (int i = 0; i < list[0]->count(); ++i)
                {
                    if (list[0]->item(i)->checkState() == Qt::Checked)
                        newProd->addTax(m_campaign->getTax(m_listIDs4->at(i)));
                }

                newProd->setUnit(m_campaign->getUnit(m_listIDs3->at(combo[3]->currentIndex())));
                newProd->save();

                m_campaign->startSignals();
                m_campaign->sendSignal_objectAdded(Type_Product);

                if (checkbox[0]->isChecked() && checkPermissions(this, m_campaign, User_Normal, Perm_ManagePLUs))
                {
                    QSettings settings;
                    int catProp = settings.value("General/NewPLU/Category").toInt();
                    int taxProp = settings.value("General/NewPLU/Taxes").toInt();
                    bool showDialog = settings.value("General/NewPLU/ShowDialog").toBool();

                    //Find the new PLU Code
                    QString newCode = "";

                    frmAddViaCopy* dlNewCode = new frmAddViaCopy(newProd->getSpecificID(), &newCode, &showDialog, Type_PLU, m_campaign, this);
                    dlNewCode->setTitle(tr("Create New PLU"));
                    dlNewCode->setWindowTitle(dlNewCode->getTitle());
                    if (dlNewCode->exec() == QDialog::Rejected)
                        return;

                    PLUCat* cat = m_campaign->getPLUCat(0);

                    if (cat == NULL)
                    {
                        cat = m_campaign->addPLUCat(tr("Default"));
                    }
                    else if (catProp == 1)
                    {
                        CampaignObject* cat2 = objectLookup(Type_PLUCat, m_campaign, m_parent);

                        if (cat2 != NULL)
                        {
                            cat = dynamic_cast<PLUCat*>(cat2);
                        }
                    }

                    m_campaign->stopSignals();

                    PLU* newPLU = m_campaign->addPLU(lineEdit[0]->text(),
                                                     cat,
                                                     doublespin[1]->amount());

                    newPLU->addItem(newProd, 1);
                    newPLU->setSpecificID(newCode);

                    //Taxes
                    if (taxProp > 0)
                    {
                        for (unsigned int i = 0; i < m_campaign->getNumberOfTaxes(); ++i)
                        {
                            Tax* tax = m_campaign->getTax(i);

                            if (tax->isDefaultSelected() || taxProp == 3)
                                newPLU->addTax(tax);
                        }
                    }

                    newPLU->save();                                       

                    m_campaign->startSignals();
                    m_campaign->sendSignal_objectAdded(Type_PLU);

                    if (showDialog)
                    {
                        frmPLU* modif = new frmPLU(newPLU, this, m_campaign);
                        modif->exec();
                        delete modif;
                    }

                }
            }

            break;

        case Type_Service:
            if (validation())
            {
                m_campaign->stopSignals();
                Service* newService = m_campaign->addService(lineEdit[0]->text(),
                                                            doublespin[0]->amount(),
                                                            doublespin[1]->amount());

                if (newService == NULL)
                {
                    m_campaign->startSignals();
                    QMessageBox::warning(this, errorMsgBoxName, tr("Error - Unable to create service."));
                    return;
                }
                else
                {
                    ok = true;
                }

                newService->setStatus(true);
                newService->setPicture(m_picture);
                newService->setSpecificID(lineEdit[1]->text());
                newService->setQuantityFormat(Service::intToQuantityFormat(combo[0]->currentIndex()));

                if (m_buttonContent.textLine1.isEmpty() && m_buttonContent.textLine2.isEmpty() && m_buttonContent.picture.isNull())
                    m_buttonContent.textLine1 = lineEdit[0]->text();

                newService->setPOSButton(m_buttonContent);

                //Taxes
                for (int i = 0; i < list[0]->count(); ++i)
                {
                    if (list[0]->item(i)->checkState() == Qt::Checked)
                        newService->addTax(m_campaign->getTax(m_listIDs->at(i)));
                }

                newService->save();

                m_campaign->startSignals();
                m_campaign->sendSignal_objectAdded(Type_Service);
            }

            break;

        case Type_CustomerCat:
            if (validation())
            {
                m_campaign->stopSignals();
                CustomerCat* newCat = m_campaign->addCustomerCat(lineEdit[0]->text());

                if (newCat == NULL)
                {
                    m_campaign->startSignals();
                    QMessageBox::warning(this, errorMsgBoxName, tr("Error - Unable to create the category."));
                    return;
                }
                else
                {
                    ok = true;
                }

                newCat->setStatus(intToStatus(combo[0]->currentIndex()));
                newCat->setSpecificID(lineEdit[1]->text());

                if (combo[1]->currentIndex() == 0)
                {
                    newCat->setParentCat(NULL);
                }
                else
                {
                    newCat->setParentCat(m_campaign->getCustomerCat(m_listIDs->at(combo[1]->currentIndex())));
                }

                newCat->save();

                m_campaign->startSignals();
                m_campaign->sendSignal_objectAdded(Type_CustomerCat);
            }

            break;

        case Type_VendorCat:
            if (validation())
            {
                m_campaign->stopSignals();
                VendorCat* newCat = m_campaign->addVendorCat(lineEdit[0]->text());

                if (newCat == NULL)
                {
                    m_campaign->startSignals();
                    QMessageBox::warning(this, errorMsgBoxName, tr("Error - Unable to create the category."));
                    return;
                }
                else
                {
                    ok = true;
                }

                newCat->setStatus(intToStatus(combo[0]->currentIndex()));
                newCat->setSpecificID(lineEdit[1]->text());

                if (combo[1]->currentIndex() == 0)
                {
                    newCat->setParentCat(NULL);
                }
                else
                {
                    newCat->setParentCat(m_campaign->getVendorCat(m_listIDs->at(combo[1]->currentIndex())));
                }

                newCat->save();

                m_campaign->startSignals();
                m_campaign->sendSignal_objectAdded(Type_VendorCat);
            }

            break;

        case Type_ProductCat:
            if (validation())
            {
                m_campaign->stopSignals();
                ProductCat* newCat = m_campaign->addProductCat(lineEdit[0]->text());

                if (newCat == NULL)
                {
                    m_campaign->startSignals();
                    QMessageBox::warning(this, errorMsgBoxName, tr("Error - Unable to create the category."));
                    return;
                }
                else
                {
                    ok = true;
                }

                newCat->setStatus(intToStatus(combo[0]->currentIndex()));
                newCat->setSpecificID(lineEdit[1]->text());

                if (combo[1]->currentIndex() == 0)
                {
                    newCat->setParentCat(NULL);
                }
                else
                {
                    newCat->setParentCat(m_campaign->getProductCat(m_listIDs->at(combo[1]->currentIndex())));
                }

                newCat->save();

                m_campaign->startSignals();
                m_campaign->sendSignal_objectAdded(Type_ProductCat);
            }

            break;

        case Type_PLUCat:
            if (validation())
            {
                m_campaign->stopSignals();
                PLUCat* newCat = m_campaign->addPLUCat(lineEdit[0]->text());

                if (newCat == NULL)
                {
                    m_campaign->startSignals();
                    QMessageBox::warning(this, errorMsgBoxName, tr("Error - Unable to create the category."));
                    return;
                }
                else
                {
                    ok = true;
                }

                newCat->setStatus(intToStatus(combo[0]->currentIndex()));
                newCat->setSpecificID(lineEdit[1]->text());

                if (combo[1]->currentIndex() == 0)
                {
                    newCat->setParentCat(NULL);
                }
                else
                {
                    newCat->setParentCat(m_campaign->getPLUCat(m_listIDs->at(combo[1]->currentIndex())));
                }

                newCat->save();

                m_campaign->startSignals();
                m_campaign->sendSignal_objectAdded(Type_PLUCat);
            }

            break;

        case Type_Payment:
            if (validation())
            {
                m_campaign->stopSignals();
                for (int i = 0; i < intspin[0]->value(); ++i)
                {
                    PaymentWidget* currentW = paymentw[i];
                    Payment* newPayment = m_campaign->addPayment(m_campaign->getCustomer(m_index2),
                                                                 m_campaign->getPaymentMode(currentW->listModes.at(currentW->pmode->currentIndex())),
                                                                 currentW->amount->amount(),
                                                                 currentW->date->date(),
                                                                 currentW->infos->toPlainText());

                    if (newPayment == NULL)
                    {
                        m_campaign->startSignals();
                        QMessageBox::warning(this, errorMsgBoxName, tr("Error - Unable to add the payment."));
                        return;
                    }
                    else
                    {
                        ok = true;
                        newPayment->save();
                    }
                }


                m_campaign->recalculatePaymentsAmounts();
                m_campaign->startSignals();
                m_campaign->sendSignal_objectAdded(Type_Payment);
            }

            break;

        case Type_Expense:
            if (validation())
            {
                m_campaign->stopSignals();
                Expense* newExpense = m_campaign->addExpense(doublespin[0]->amount(),
                                                             m_campaign->getPaymentMode(m_listIDs->at(combo[0]->currentIndex())),
                                                             dateedit[0]->date(),
                                                             textEdit[0]->toPlainText());

                if (newExpense == NULL)
                {
                    m_campaign->startSignals();
                    QMessageBox::warning(this, errorMsgBoxName, tr("Error - Unable to add the expense."));
                    return;
                }
                else
                {
                    ok = true;
                }

                newExpense->setType((ExpenseType) combo[1]->currentIndex());
                newExpense->save();

                m_campaign->recalculateAmounts();
                m_campaign->startSignals();
                m_campaign->sendSignal_objectAdded(Type_Expense);
            }

            break;

        case Type_Donation:
            if (validation())
            {
                m_campaign->stopSignals();
                Donation* newDonation = NULL;

                if (isDonator)
                {
                    newDonation = m_campaign->addDonation(doublespin[0]->amount(),
                                                            m_campaign->getDonator(m_index2),
                                                            m_campaign->getPaymentMode(m_listIDs->at(combo[0]->currentIndex())),
                                                            textEdit[0]->toPlainText());
                }
                else
                {
                    newDonation = m_campaign->addDonation(doublespin[0]->amount(),
                                                            m_campaign->getCustomer(m_index2),
                                                            m_campaign->getPaymentMode(m_listIDs->at(combo[0]->currentIndex())),
                                                            textEdit[0]->toPlainText());
                }

                if (newDonation == NULL)
                {
                    QMessageBox::warning(this, errorMsgBoxName, tr("Error - Unable to add the donation."));
                    return;
                }
                else
                {
                    ok = true;
                }

                newDonation->setDate(dateedit[0]->date());
                newDonation->save();

                m_campaign->recalculateAmounts();
                m_campaign->startSignals();
                m_campaign->sendSignal_objectAdded(Type_Donation);
            }

            break;

        case Type_PaymentMode:
            if (validation())
            {
                m_campaign->stopSignals();
                PaymentMode* newMode = m_campaign->addPaymentMode(lineEdit[0]->text());

                if (newMode == NULL)
                {
                    QMessageBox::warning(this, errorMsgBoxName, tr("Error - Unable to create the payment mode."));
                    return;
                }
                else
                {
                    ok = true;
                }

                newMode->setStatus(intToStatus(combo[0]->currentIndex()));

                if (combo[1]->currentIndex() > 0)
                    newMode->setPicture(QPixmap(combo[1]->itemData(combo[1]->currentIndex()).toString()));

                newMode->save();

                m_campaign->startSignals();
                m_campaign->sendSignal_objectAdded(Type_PaymentMode);                
            }

            break;

        case Type_Supplier:
            if (validation())
            {
                m_campaign->stopSignals();
                Supplier* newSupp = m_campaign->addSupplier(lineEdit[0]->text(),
                                                            *m_location);

                if (newSupp == NULL)
                {
                    QMessageBox::warning(this, errorMsgBoxName, tr("Error - Unable to create the supplier."));
                    return;
                }
                else
                {
                    ok = true;
                }

                newSupp->setStatus(intToStatus(combo[0]->currentIndex()));
                newSupp->setContactName(lineEdit[1]->text());
                newSupp->setPicture(m_picture);
                newSupp->save();

                m_campaign->startSignals();
                m_campaign->sendSignal_objectAdded(Type_Supplier);                
            }

            break;

        case Type_Tax:
            if (validation())
            {
                m_campaign->stopSignals();
                Tax* newTax = m_campaign->addTax(lineEdit[0]->text(),
                                                 doublespin[0]->doubleValue(),
                                                 checkbox[0]->isChecked());

                if (newTax == NULL)
                {
                    QMessageBox::warning(this, errorMsgBoxName, tr("Error - Unable to create the tax."));
                    return;
                }
                else
                {
                    ok = true;
                }

                newTax->setSingleLetterAbbreviation(lineEdit[1]->text().at(0).toLatin1());
                newTax->setStatus(intToStatus(combo[0]->currentIndex()));
                newTax->save();

                m_campaign->startSignals();
                m_campaign->sendSignal_objectAdded(Type_Tax);                
            }

            break;

        case Type_User:
            if (validation())
            {
                m_campaign->stopSignals();

                LoginInfos login;
                login.login = lineEdit[2]->text();
                login.password = lineEdit[3]->text();

                User* newUser = m_campaign->addUser(lineEdit[0]->text(),
                                                    lineEdit[1]->text(),
                                                    login,
                                                    intToTypeUser(combo[0]->currentIndex()));

                if (newUser == NULL)
                {
                    QMessageBox::warning(this, errorMsgBoxName, tr("Error - Unable to create the user."));
                    return;
                }
                else
                {
                    ok = true;
                }

                if (newUser->getType() == User_Normal)
                    newUser->setPermissions(*m_perm);

                newUser->save();

                m_campaign->startSignals();
                m_campaign->sendSignal_objectAdded(Type_User);
            }

            break;

        case Type_Unit:
            if (validation())
            {
                m_campaign->stopSignals();
                Unit* newUnit = m_campaign->addUnit(lineEdit[0]->text(),
                                                    lineEdit[1]->text(),
                                                    intToUnitType(combo[0]->currentIndex()));

                if (newUnit == NULL)
                {
                    QMessageBox::warning(this, errorMsgBoxName, tr("Error - Unable to create the user."));
                    return;
                }
                else
                {
                    ok = true;
                }

                newUnit->setConversionFactorToBase(lineEdit[2]->text().toFloat());
                newUnit->setPrecision(intspin[0]->value());

                newUnit->save();

                m_campaign->startSignals();
                m_campaign->sendSignal_objectAdded(Type_Unit);
            }

            break;

        default:
            ok = true;
            break;

    }

    if (ok)
    {
        emit accepted();
        clear();
    }
}

void CAMSEGObjectModifier::clear()
{
    m_location = new Location;
    m_loginInfos = new LoginInfos;
    m_perm = new Permissions(*m_campaign->getPermissions());
    m_index2 = -1;
    isDonator = true;

    m_buttonContent.backgroundColor = Qt::lightGray;
    m_buttonContent.style = POSButtonContent::TextButton;
    m_buttonContent.textLine1 = "";
    m_buttonContent.textLine2 = "";
    m_buttonContent.picture = QPixmap();

    for (unsigned int i = 0; i < MAX_LINE; ++i)
    {
        lineEdit[i]->setText("");
    }
    for (unsigned int i = 0; i < MAX_TEXT; ++i)
    {
        textEdit[i]->setText("");
    }
    for (unsigned int i = 0; i < MAX_DSPIN; ++i)
    {
        doublespin[i]->setDoubleValue(0);
    }
    for (unsigned int i = 0; i < MAX_TABWIDGET; ++i)
    {
        if (tabwidget[i]->count() > 1)
            tabwidget[i]->setCurrentIndex(0);
    }

    lineEdit[0]->setFocus();

    if (m_type == Type_Product)
    {
        doublespin[2]->setDoubleValue(1.0);
        doublespin[4]->setDoubleValue(1.0);
    }

    if (m_type == Type_Unit)
        lineEdit[2]->setText("1.0");

    //Picture
    if (m_type == Type_Customer || m_type == Type_Vendor)
    {
        m_picture = QPixmap(PATH_ICONS + "default_cust.png");
    }
    else if (m_type == Type_Product)
    {
        m_picture = QPixmap(PATH_ICONS + "default_prod.png");
    }
    else
    {
        m_picture = QPixmap();
    }

}

void CAMSEGObjectModifier::modify()
{
    bool ok = false;
    QString errorMsgBoxName = tr("Error");

    switch (m_type)
    {
        case Type_Customer:
            if (validation())
            {
                Customer* temp = m_campaign->getCustomer(m_objectInternalID);

                temp->setName(lineEdit[0]->text());
                temp->setFirstName(lineEdit[1]->text());
                temp->setCategory(m_campaign->getCustomerCat(m_listIDs->at(combo[0]->currentIndex())));
                temp->setSex(intToSex(combo[1]->currentIndex()));
                temp->setStatus(intToStatus(combo[2]->currentIndex()));
                temp->setLocation(*m_location);
                temp->setLoginInfos(*m_loginInfos);
                temp->setPicture(m_picture);
                temp->setSpecificID(lineEdit[2]->text());

                temp->setInformations(textEdit[0]->toPlainText());
                temp->save();

                ok = true;
            }
            break;

        case Type_Donator:
            if (validation())
            {
                Donator* temp = m_campaign->getDonator(m_objectInternalID);

                temp->setName(lineEdit[0]->text());
                temp->setFirstName(lineEdit[1]->text());
                temp->setSex(intToSex(combo[1]->currentIndex()));
                temp->setStatus(intToStatus(combo[2]->currentIndex()));
                temp->setLocation(*m_location);
                temp->setPicture(m_picture);

                temp->setInformations(textEdit[0]->toPlainText());
                temp->save();

                ok = true;
            }
            break;

        case Type_Vendor:
            if (validation())
            {
                Vendor* temp = m_campaign->getVendor(m_objectInternalID);

                temp->setName(lineEdit[0]->text());
                temp->setFirstName(lineEdit[1]->text());
                temp->setCategory(m_campaign->getVendorCat(m_listIDs->at(combo[0]->currentIndex())));
                temp->setSex(intToSex(combo[1]->currentIndex()));
                temp->setStatus(intToStatus(combo[2]->currentIndex()));
                temp->setLocation(*m_location);
                temp->setLoginInfos(*m_loginInfos);
                temp->setPicture(m_picture);
                temp->setSpecificID(lineEdit[2]->text());

                temp->setInformations(textEdit[0]->toPlainText());
                temp->save();

                ok = true;
            }
            break;

        case Type_Product:
            if (validation())
            {
                Product* temp = m_campaign->getProduct(m_objectInternalID);

                //Check for single PLU containing this item
                if (temp->getPriceSold() != doublespin[1]->amount()) //If price has been modified
                {
                    PLU* pluTemp = NULL;
                    for (quint32 i = 0; i < m_campaign->getNumberOfPLUs(); ++i)
                    {
                        pluTemp = m_campaign->getPLU(i);

                        if (pluTemp->getNumberOfItems() == 1 && pluTemp->getItem(0)->getQuantity() == 1.0 && pluTemp->getItem(0)->getProductId() == temp->getId() && pluTemp->getPriceSold() != doublespin[1]->amount())
                        {
                            int answer = QMessageBox::question(m_parent,
                                                               tr("Modify Product"),
                                                               tr("A PLU named '%1' which contains one '%2' (current Product) has a price different "
                                                                  "than the current reccomended price for this Product. Do you want to modify the price "
                                                                  "of this PLU to match this Product's recommended price ?\n\n"
                                                                  "Product's Rec. Price : %3\n"
                                                                  "PLU's Price Sold : %4")
                                                                    .arg(pluTemp->getFullName())
                                                                    .arg(temp->getFullName())
                                                                    .arg(m_campaign->formatCurrency(doublespin[1]->amount()))
                                                                    .arg(m_campaign->formatCurrency(pluTemp->getPriceSold())),
                                                               QMessageBox::Yes | QMessageBox::No);

                            if (answer == QMessageBox::Yes && checkPermissions(m_parent, m_campaign, User_Normal, Perm_ManagePLUs))
                            {
                                pluTemp->setPriceSold(doublespin[1]->amount());
                                pluTemp->save();
                            }
                        }
                    }
                }

                temp->setName(lineEdit[0]->text());
                temp->setBasePrice(doublespin[0]->amount());
                temp->setPriceSold(doublespin[1]->amount());
                temp->setCategory(m_campaign->getProductCat(m_listIDs->at(combo[0]->currentIndex())));
                temp->setSupplier(m_campaign->getSupplier(m_listIDs2->at(combo[1]->currentIndex())));
                temp->setStatus(intToStatus(combo[2]->currentIndex()));
                temp->setPicture(m_picture);
                temp->setSpecificID(lineEdit[1]->text());
                temp->setStdDeliveryQuantity(doublespin[2]->quantity());
                temp->setStdSellingQuantity(doublespin[4]->quantity());
                temp->setSecurityStock(doublespin[3]->quantity());
                temp->setSupplierID(lineEdit[2]->text());
                temp->setCanBeSold(checkbox[1]->isChecked());

                //Taxes
                temp->removeAllTaxes();
                for (int i = 0; i < list[0]->count(); ++i)
                {
                    if (list[0]->item(i)->checkState() == Qt::Checked)
                        temp->addTax(m_campaign->getTax(m_listIDs4->at(i)));
                }

                temp->setInformations(textEdit[0]->toPlainText());
                m_campaign->recalculateAmounts();
                temp->save();

                ok = true;
            }
            break;

        case Type_Service:
            if (validation())
            {
                Service* temp = m_campaign->getService(m_objectInternalID);

                temp->setName(lineEdit[0]->text());
                temp->setCosts(doublespin[0]->amount());
                temp->setPriceSold(doublespin[1]->amount());
                temp->setStatus(intToStatus(combo[2]->currentIndex()));
                temp->setPicture(m_picture);
                temp->setSpecificID(lineEdit[1]->text());
                temp->setQuantityFormat(Service::intToQuantityFormat(combo[0]->currentIndex()));
                temp->setPOSButton(m_buttonContent);

                //Taxes
                temp->removeAllTaxes();
                for (int i = 0; i < list[0]->count(); ++i)
                {
                    if (list[0]->item(i)->checkState() == Qt::Checked)
                        temp->addTax(m_campaign->getTax(m_listIDs->at(i)));
                }

                temp->setInformations(textEdit[0]->toPlainText());
                temp->save();

                ok = true;
            }
            break;

        case Type_CustomerCat:
            if (validation())
            {
                CustomerCat* temp = m_campaign->getCustomerCat(m_objectInternalID);

                temp->setName(lineEdit[0]->text());
                temp->setStatus(intToStatus(combo[0]->currentIndex()));
                temp->setSpecificID(lineEdit[1]->text());

                if (combo[1]->currentIndex() == 0)
                {
                    temp->setParentCat(NULL);
                }
                else
                {
                    temp->setParentCat(m_campaign->getCustomerCat(m_listIDs->at(combo[1]->currentIndex())));
                }

                temp->setInformations(textEdit[0]->toPlainText());
                temp->save();

                ok = true;
            }
            break;

        case Type_VendorCat:
            if (validation())
            {
                VendorCat* temp = m_campaign->getVendorCat(m_objectInternalID);

                temp->setName(lineEdit[0]->text());
                temp->setStatus(intToStatus(combo[0]->currentIndex()));
                temp->setSpecificID(lineEdit[1]->text());

                if (combo[1]->currentIndex() == 0)
                {
                    temp->setParentCat(NULL);
                }
                else
                {
                    temp->setParentCat(m_campaign->getVendorCat(m_listIDs->at(combo[1]->currentIndex())));
                }

                temp->setInformations(textEdit[0]->toPlainText());
                temp->save();

                ok = true;
            }
            break;

        case Type_ProductCat:
            if (validation())
            {
                ProductCat* temp = m_campaign->getProductCat(m_objectInternalID);

                temp->setName(lineEdit[0]->text());
                temp->setStatus(intToStatus(combo[0]->currentIndex()));
                temp->setSpecificID(lineEdit[1]->text());

                if (combo[1]->currentIndex() == 0)
                {
                    temp->setParentCat(NULL);
                }
                else
                {
                    temp->setParentCat(m_campaign->getProductCat(m_listIDs->at(combo[1]->currentIndex())));
                }

                temp->setInformations(textEdit[0]->toPlainText());
                temp->save();

                ok = true;
            }
            break;

        case Type_PLUCat:
            if (validation())
            {
                PLUCat* temp = m_campaign->getPLUCat(m_objectInternalID);

                temp->setName(lineEdit[0]->text());
                temp->setStatus(intToStatus(combo[0]->currentIndex()));
                temp->setSpecificID(lineEdit[1]->text());

                if (combo[1]->currentIndex() == 0)
                {
                    temp->setParentCat(NULL);
                }
                else
                {
                    temp->setParentCat(m_campaign->getPLUCat(m_listIDs->at(combo[1]->currentIndex())));
                }

                temp->setInformations(textEdit[0]->toPlainText());
                temp->save();

                ok = true;
            }
            break;

        case Type_Payment:
            if (validation())
            {
                Payment* temp = m_campaign->getPayment(m_objectInternalID);

                temp->setPaymentMode(m_campaign->getPaymentMode(m_listIDs->at(combo[0]->currentIndex())));
                temp->setAmount(doublespin[0]->amount());
                temp->setDate(dateedit[0]->date());
                temp->setInformations(textEdit[0]->toPlainText());
                m_campaign->recalculatePaymentsAmounts();
                temp->save();

                ok = true;
            }
            break;

        case Type_Expense:
            if (validation())
            {
                Expense* temp = m_campaign->getExpense(m_objectInternalID);

                temp->setPaymentMode(m_campaign->getPaymentMode(m_listIDs->at(combo[0]->currentIndex())));
                temp->setAmount(doublespin[0]->amount());
                temp->setDate(dateedit[0]->date());
                temp->setType((ExpenseType) combo[1]->currentIndex());
                temp->setInformations(textEdit[0]->toPlainText());
                m_campaign->recalculateAmounts();
                temp->save();

                ok = true;
            }
            break;

        case Type_Donation:
            if (validation())
            {
                Donation* temp = m_campaign->getDonation(m_objectInternalID);

                temp->setPaymentMode(m_campaign->getPaymentMode(m_listIDs->at(combo[0]->currentIndex())));
                temp->setAmount(doublespin[0]->amount());
                temp->setInformations(textEdit[0]->toPlainText());
                temp->setDate(dateedit[0]->date());

                if (isDonator)
                {
                    temp->setDonator(m_campaign->getDonator(m_index2));
                }
                else
                {
                    temp->setCustomer(m_campaign->getCustomer(m_index2));
                }

                m_campaign->recalculateAmounts();
                temp->save();
                ok = true;
            }
            break;

        case Type_PaymentMode:
            if (validation())
            {
                PaymentMode* temp = m_campaign->getPaymentMode(m_objectInternalID);

                temp->setName(lineEdit[0]->text());
                temp->setStatus(intToStatus(combo[0]->currentIndex()));
                temp->setInformations(textEdit[0]->toPlainText());
                temp->setPicture(m_picture);
                temp->save();

                ok = true;
            }
            break;

        case Type_Supplier:
            if (validation())
            {
                Supplier* temp = m_campaign->getSupplier(m_objectInternalID);

                temp->setName(lineEdit[0]->text());
                temp->setStatus(intToStatus(combo[0]->currentIndex()));
                temp->setContactName(lineEdit[1]->text());

                temp->setLocation(*m_location);
                temp->setPicture(m_picture);

                temp->setInformations(textEdit[0]->toPlainText());
                temp->save();

                ok = true;
            }
            break;

        case Type_Tax:
            if (validation())
            {
                Tax* temp = m_campaign->getTax(m_objectInternalID);

                temp->setName(lineEdit[0]->text());
                temp->setSingleLetterAbbreviation(lineEdit[1]->text().at(0).toLatin1());
                temp->setRate(doublespin[0]->doubleValue());
                temp->setDefaultSelected(checkbox[0]->isChecked());
                temp->setStatus(intToStatus(combo[0]->currentIndex()));

                temp->setInformations(textEdit[0]->toPlainText());
                temp->save();

                ok = true;
            }
            break;

        case Type_User:
            if (validation())
            {
                User* temp = m_campaign->getUser(m_objectInternalID);

                temp->setName(lineEdit[0]->text());
                temp->setFirstName(lineEdit[1]->text());

                m_loginInfos->login = lineEdit[2]->text();
                temp->setLoginInfos(*m_loginInfos);

                if (temp->getType() == User_Normal)
                    temp->setPermissions(*m_perm);

                temp->setInformations(textEdit[0]->toPlainText());
                temp->save();

                ok = true;
            }
            break;

        case Type_Unit:
            if (validation())
            {
                Unit* temp = m_campaign->getUnit(m_objectInternalID);

                temp->setName(lineEdit[0]->text());
                temp->setAbbreviation(lineEdit[1]->text());
                temp->setConversionFactorToBase(lineEdit[2]->text().toFloat());
                temp->setPrecision(intspin[0]->value());

                temp->setInformations(textEdit[0]->toPlainText());
                m_campaign->recalculateAmounts();
                temp->save();

                ok = true;
            }
            break;

        case Type_Currency:
            if (validation())
            {
                m_currency->setName(lineEdit[0]->text());
                m_currency->setISOCode(lineEdit[1]->text());
                m_currency->setSign(lineEdit[2]->text());
                m_currency->setSignPosition((Currency::SignPosition) combo[0]->currentIndex());
                m_currency->setInformations(textEdit[0]->toPlainText());

                ok = true;
            }
            break;

        default:
            ok = true;
            break;
    }

    if (ok)
    {
        emit accepted();
        clear();
    }

}

void CAMSEGObjectModifier::modifyLoginInfos()
{
    frmLocationModif* modif = new frmLocationModif(m_loginInfos, m_campaign, m_modification, m_parent);
    modif->exec();
    delete modif;
}

void CAMSEGObjectModifier::modifyLocation()
{
    frmLocationModif* modif = new frmLocationModif(m_location, m_campaign, m_parent);
    modif->exec();
    delete modif;
}

void CAMSEGObjectModifier::changePassword()
{
    QString newPassword = "";
    QString repeatPassword = "";
    bool ok = false;

    newPassword = QInputDialog::getText(m_parent, m_parent->windowTitle(), tr("Enter the new password :"), QLineEdit::Password, "", &ok);

    if (ok)
    {
        repeatPassword = QInputDialog::getText(m_parent, m_parent->windowTitle(), tr("Repeat the new password :"), QLineEdit::Password, "", &ok);

        if (ok)
        {
            if (newPassword == repeatPassword)
            {
                m_loginInfos->password = hashMD5(newPassword);
            }
            else
            {
                QMessageBox::information(this, m_parent->windowTitle(), tr("Error : Passwords do not match."));
            }
        }
    }
}

void CAMSEGObjectModifier::modifyPicture()
{
    QPixmap otherPict = m_picture;
    frmPicture* pictModif = new frmPicture(&otherPict, this);

    if (pictModif->exec() == QDialog::Accepted)
        m_picture = otherPict;

    delete pictModif;
}

void CAMSEGObjectModifier::modifyPermissions()
{
    frmChangePermissions* modif = new frmChangePermissions(m_perm, m_parent);
    modif->exec();
    delete modif;
}

void CAMSEGObjectModifier::modifyPOSButton()
{
    frmPOSButton* posDialog = new frmPOSButton(&m_buttonContent, this);
    posDialog->exec();
    delete posDialog;
}

void CAMSEGObjectModifier::selectDonator()
{
    frmSelectDonator* selector = new frmSelectDonator(&m_index2, &isDonator, m_campaign, m_parent);
    selector->exec();
    delete selector;

    if (m_index2 != -1)
    {
        if (isDonator)
        {
            Donator* donator = m_campaign->getDonator(m_index2);
            label[0]->setText(donator->getName() + ", " + donator->getFirstName());
        }
        else
        {
            Customer* cust = m_campaign->getCustomer(m_index2);
            label[0]->setText(cust->getName() + ", " + cust->getFirstName());
        }
    }
}

void CAMSEGObjectModifier::updateConversionLabel()
{
    //Example : 1 cm = 0.01 m
    Unit* baseUnit = m_campaign->getBaseUnit(intToUnitType(combo[0]->currentIndex()));

    label[0]->setText(tr("1 %1 = %2 %3").arg(lineEdit[1]->text())
                                        .arg(lineEdit[2]->text())
                                        .arg(baseUnit->getAbbreviation()));
}

void CAMSEGObjectModifier::updatePaymentCount(int p_newNn)
{
    if (tabwidget[0]->count() < p_newNn) // Add tabs
    {
        for (int i = tabwidget[0]->count(); i < p_newNn; ++i)
        {
            tabwidget[0]->addTab(paymentw[i], QString::number(i + 1));
        }
    }
    else if (tabwidget[0]->count() > p_newNn) // Remove tabs
    {
        while (tabwidget[0]->count() > p_newNn)
        {
            tabwidget[0]->removeTab(tabwidget[0]->count() - 1);
        }
    }
}

void CAMSEGObjectModifier::updateUsername()
{
    lineEdit[2]->setText(getUsernamePattern(lineEdit[0]->text(), lineEdit[1]->text()));
}

PaymentWidget::PaymentWidget(Campaign* p_campaign, QWidget* parent) : QWidget(parent)
{
    amount = new AmountEdit();
    pmode = new QComboBox();
    date = new QDateEdit();
    infos = new QTextEdit();

    amount->setMaximum(MAX_VALUE_SPINBOX);
    amount->setPrefix(p_campaign->getCurrency()->getSign() + " ");

    CAMSEGObjectModifier::fillComboBox(pmode, &listModes, CAMSEGObjectModifier::PaymentModeFill, p_campaign);

    date->setCalendarPopup(true);
    date->setDate(QDate::currentDate());

    infos->setTabChangesFocus(true);
    infos->setMaximumHeight(75);

    QFormLayout* layout = new QFormLayout(this);
    layout->addRow(tr("Amount :"), amount);
    layout->addRow(tr("Payment Mode :"), pmode);
    layout->addRow(tr("Date :"), date);
    layout->addRow(tr("Additional Infos :"), infos);
}

PaymentWidget::~PaymentWidget()
{
    delete amount;
    delete pmode;
    delete date;
    delete infos;
}


