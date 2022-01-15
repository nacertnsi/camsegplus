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

#include "camsegobjectinfos.h"

#include <libCAMSEG/customer.h>
#include <libCAMSEG/vendor.h>
#include <libCAMSEG/product.h>
#include <libCAMSEG/plu.h>
#include <libCAMSEG/customercat.h>
#include <libCAMSEG/vendorcat.h>
#include <libCAMSEG/productcat.h>
#include <libCAMSEG/plucat.h>
#include <libCAMSEG/order.h>
#include <libCAMSEG/order2.h>
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

#include <QHeaderView>

CAMSEGObjectInfos::CAMSEGObjectInfos(ObjectType p_viewType, int p_objectInternalID, Campaign* p_campaign) :
        m_type(p_viewType), m_campaign(p_campaign)
{
    m_mainLayout = new QVBoxLayout(this);
    m_infosLayout = new QFormLayout();
    m_objectName = new QLabel();

    m_mainLayout->addWidget(m_objectName);
    m_mainLayout->addLayout(m_infosLayout);

    QFont policeGras;
    policeGras.setBold(true);
    m_objectName->setFont(policeGras);

    for (int i = 0; i < MAX_INFOS; i++)
        m_infos[i] = new QLabel();

    table = new QTableWidget();

    QFont font(table->font());
    font.setPointSize(Font_Size);
    table->setFont(font);

    table->setSelectionMode(QAbstractItemView::NoSelection);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->verticalHeader()->hide();

    switch (m_type)
    {
        case Type_Customer:
            m_infosLayout->addRow(tr("Status :"), m_infos[0]);
            m_infosLayout->addRow(tr("Category :"), m_infos[1]);
            m_infosLayout->addRow(tr("Sex :"), m_infos[2]);
            m_infosLayout->addRow(tr("Address :"), m_infos[3]);
            m_infosLayout->addRow(tr("City :"), m_infos[4]);
            m_infosLayout->addRow(tr("Phone :"), m_infos[5]);
            m_infosLayout->addRow(tr("Email :"), m_infos[6]);
            m_infosLayout->addRow(tr("Total Amount :"), m_infos[7]);
            m_infos[3]->setWordWrap(true);

            break;

        case Type_Vendor:
            m_infosLayout->addRow(tr("Status :"), m_infos[0]);
            m_infosLayout->addRow(tr("Category :"), m_infos[1]);
            m_infosLayout->addRow(tr("Sex :"), m_infos[2]);
            m_infosLayout->addRow(tr("Address :"), m_infos[3]);
            m_infosLayout->addRow(tr("City :"), m_infos[4]);
            m_infosLayout->addRow(tr("Phone :"), m_infos[5]);
            m_infosLayout->addRow(tr("Email :"), m_infos[6]);
            m_infosLayout->addRow(tr("Total Sales :"), m_infos[7]);
            break;

        case Type_Product:
            m_infosLayout->addRow(tr("Status :"), m_infos[0]);
            m_infosLayout->addRow(tr("Category :"), m_infos[1]);
            m_infosLayout->addRow(tr("Supplier :"), m_infos[2]);
            m_infosLayout->addRow(tr("Base Price :"), m_infos[3]);
            m_infosLayout->addRow(tr("Rec. Price Sold :"), m_infos[4]);
            m_infosLayout->addRow(tr("Unit :"), m_infos[5]);
            break;

        case Type_PLU:
            m_infosLayout->addRow(tr("Status :"), m_infos[0]);
            m_infosLayout->addRow(tr("Category :"), m_infos[1]);
            m_infosLayout->addRow(tr("Costs :"), m_infos[2]);
            m_infosLayout->addRow(tr("Unit Price :"), m_infos[3]);
            m_infosLayout->addRow(tr("Profit :"), m_infos[4]);
            m_infosLayout->addRow(tr("Taxes :"), m_infos[5]);
            break;

        case Type_CustomerCat:
            m_infosLayout->addRow(tr("Status :"), m_infos[0]);
            m_infosLayout->addRow(tr("Num. of customers :"), m_infos[1]);
            m_infosLayout->addRow(tr("Total Amount :"), m_infos[2]);
            break;

        case Type_VendorCat:
            m_infosLayout->addRow(tr("Status :"), m_infos[0]);
            m_infosLayout->addRow(tr("Num. of vendors :"), m_infos[1]);
            m_infosLayout->addRow(tr("Total Sales :"), m_infos[2]);
            break;

        case Type_ProductCat:
            m_infosLayout->addRow(tr("Status :"), m_infos[0]);
            m_infosLayout->addRow(tr("Num. of products :"), m_infos[1]);
            m_infosLayout->addRow(tr("Average price sold :"), m_infos[2]);
            m_infosLayout->addRow(tr("Average profit :"), m_infos[3]);
            break;

        case Type_PLUCat:
            m_infosLayout->addRow(tr("Status :"), m_infos[0]);
            m_infosLayout->addRow(tr("Num. of PLUs :"), m_infos[1]);
            m_infosLayout->addRow(tr("Average unit price :"), m_infos[2]);
            m_infosLayout->addRow(tr("Average profit :"), m_infos[3]);
            break;

        case Type_Discount:
            m_infosLayout->addRow(tr("Status :"), m_infos[0]);
            m_infosLayout->addRow(tr("Type :"), m_infos[1]);
            m_infosLayout->addRow(tr("Description :"), m_infos[2]);
            m_infos[2]->setWordWrap(true);
            break;

        case Type_Unit:
            m_infosLayout->addRow(tr("Type :"), m_infos[0]);
            break;

        case Type_Order:
        {
            m_infosLayout->addRow(tr("Status :"), m_infos[0]);
            m_infosLayout->addRow(tr("Customer :"), m_infos[1]);
            m_infosLayout->addRow(tr("Vendor :"), m_infos[2]);
            m_infosLayout->addRow(tr("Amount :"), m_infos[3]);
            m_infosLayout->addRow(tr("Amount + taxes :"), m_infos[4]);
            m_infosLayout->addRow(table);

            QStringList headers;
            headers << tr("Code") << tr("Name") << tr("QTY");

            table->setColumnCount(3);
            table->setHorizontalHeaderLabels(headers);

            break;
        }

    case Type_Order2:
    {
        m_infosLayout->addRow(tr("Status :"), m_infos[0]);
        m_infosLayout->addRow(tr("Customer :"), m_infos[1]);
        m_infosLayout->addRow(tr("Vendor :"), m_infos[2]);
        m_infosLayout->addRow(tr("Amount :"), m_infos[3]);
        m_infosLayout->addRow(tr("Amount + taxes :"), m_infos[4]);
        m_infosLayout->addRow(table);

        QStringList headers;
        headers << tr("Code") << tr("Name") << tr("QTY");

        table->setColumnCount(3);
        table->setHorizontalHeaderLabels(headers);

        break;
    }


        case Type_Payment:
            m_infosLayout->addRow(tr("Gross Sales :"), m_infos[0]);
            m_infosLayout->addRow(tr("Amount Paid :"), m_infos[1]);
            m_infosLayout->addRow(tr("Amount Due :"), m_infos[2]);
            break;

        case Type_PaymentMode:
            m_infosLayout->addRow(tr("Status :"), m_infos[0]);
            m_infosLayout->addRow(tr("Num. of payments :"), m_infos[1]);
            m_infosLayout->addRow(tr("Total Amount :"), m_infos[2]);
            break;

        case Type_Supplier:
            m_infosLayout->addRow(tr("Status :"), m_infos[0]);
            m_infosLayout->addRow(tr("Phone :"), m_infos[1]);
            m_infosLayout->addRow(tr("Num. of products :"), m_infos[2]);
            m_infosLayout->addRow(tr("Num. of deliveries :"), m_infos[3]);
            break;

        case Type_Tax:
            m_infosLayout->addRow(tr("Status :"), m_infos[0]);
            m_infosLayout->addRow(tr("Rate :"), m_infos[1]);
            m_infosLayout->addRow(tr("Total Amount :"), m_infos[2]);
            break;

        case Type_User:
            m_infosLayout->addRow(tr("Username :"), m_infos[0]);
            m_infosLayout->addRow(tr("Type :"), m_infos[1]);
            break;

        case Type_Delivery:
        {
            m_infosLayout->addRow(tr("Status :"), m_infos[0]);
            m_infosLayout->addRow(tr("Paid State :"), m_infos[1]);
            m_infosLayout->addRow(tr("Date :"), m_infos[2]);
            m_infosLayout->addRow(tr("Num. of products :"), m_infos[3]);
            m_infosLayout->addRow(tr("Amount :"), m_infos[4]);
            m_infosLayout->addRow(table);

            QStringList headers;
            headers << tr("Code") << tr("Name") << tr("QTY");

            table->setColumnCount(3);
            table->setHorizontalHeaderLabels(headers);
            break;
        }

    case Type_Delivery2:
    {
        m_infosLayout->addRow(tr("Status :"), m_infos[0]);
        m_infosLayout->addRow(tr("Paid State :"), m_infos[1]);
        m_infosLayout->addRow(tr("Date :"), m_infos[2]);
        m_infosLayout->addRow(tr("Num. of products :"), m_infos[3]);
        m_infosLayout->addRow(tr("Amount :"), m_infos[4]);
        m_infosLayout->addRow(table);

        QStringList headers;
        headers << tr("Code") << tr("Name") << tr("QTY");

        table->setColumnCount(3);
        table->setHorizontalHeaderLabels(headers);
        break;
    }

        case Type_Donator:
            m_infosLayout->addRow(tr("Status :"), m_infos[0]);
            m_infosLayout->addRow(tr("Sex :"), m_infos[1]);
            m_infosLayout->addRow(tr("Address :"), m_infos[2]);
            m_infosLayout->addRow(tr("City :"), m_infos[3]);
            m_infosLayout->addRow(tr("Phone :"), m_infos[4]);
            m_infosLayout->addRow(tr("Email :"), m_infos[5]);
            m_infosLayout->addRow(tr("Total Donated :"), m_infos[6]);
            m_infos[2]->setWordWrap(true);
            break;

        case Type_Donation:
            m_infosLayout->addRow(tr("Amount :"), m_infos[0]);
            m_infosLayout->addRow(tr("Who :"), m_infos[1]);
            m_infosLayout->addRow(tr("Date :"), m_infos[2]);
            m_infosLayout->addRow(tr("Payment Mode :"), m_infos[3]);
            break;

        case Type_Service:
            m_infosLayout->addRow(tr("Status :"), m_infos[0]);
            m_infosLayout->addRow(tr("Costs :"), m_infos[1]);
            m_infosLayout->addRow(tr("Unit Price :"), m_infos[2]);
            m_infosLayout->addRow(tr("Profit :"), m_infos[3]);
            m_infosLayout->addRow(tr("Taxes :"), m_infos[4]);
            break;

        default:
            break;
    }

    if (p_objectInternalID != NOT_SET)
    {
        m_objectInternalID = p_objectInternalID;
        fillInfos();
    }

}

void CAMSEGObjectInfos::reloadInfos()
{
    fillInfos();
}

void CAMSEGObjectInfos::changeID(unsigned int p_objectInternalID)
{
    m_objectInternalID = p_objectInternalID;
    reloadInfos();
}

void CAMSEGObjectInfos::fillInfos()
{
    QFont boldFont;
    boldFont.setBold(true);

    switch (m_type)
    {
        case Type_Customer:
        {
            Customer* temp = m_campaign->getCustomer(m_objectInternalID);
            if (temp == NULL)
            {
                break;
            }

            m_objectName->setText(temp->getFirstName() + " " + temp->getName());

            m_infos[0]->setText(getLiteralStatus(temp->isActive()));
            m_infos[1]->setText(temp->getCategory()->getName());
            m_infos[2]->setText(getLiteralSex(temp->getSex()));
            m_infos[3]->setText(temp->getLocation()->adress);
            m_infos[4]->setText(temp->getLocation()->city + ", " + temp->getLocation()->country);
            m_infos[5]->setText(temp->getLocation()->phone1);
            m_infos[6]->setText(temp->getLocation()->email);
            m_infos[7]->setText(m_campaign->formatCurrency(temp->getTotalAmount()));

            break;
        }
        case Type_Vendor:
        {
            Vendor* temp = m_campaign->getVendor(m_objectInternalID);
            if (temp == NULL)
            {
                break;
            }

            m_objectName->setText(temp->getFirstName() + " " + temp->getName());

            m_infos[0]->setText(getLiteralStatus(temp->isActive()));
            m_infos[1]->setText(temp->getCategory()->getName());
            m_infos[2]->setText(getLiteralSex(temp->getSex()));
            m_infos[3]->setText(temp->getLocation()->adress);
            m_infos[4]->setText(temp->getLocation()->city + ", " + temp->getLocation()->country);
            m_infos[5]->setText(temp->getLocation()->phone1);
            m_infos[6]->setText(temp->getLocation()->email);
            m_infos[7]->setText(m_campaign->formatCurrency(temp->getTotalSales()));

            break;
        }
        case Type_Product:
        {
            Product* temp = m_campaign->getProduct(m_objectInternalID);
            if (temp == NULL)
            {
                break;
            }

            m_objectName->setText(temp->getName());

            m_infos[0]->setText(getLiteralStatus(temp->isActive()));
            m_infos[1]->setText(temp->getCategory()->getName());
            m_infos[2]->setText(temp->getSupplier()->getName());
            m_infos[3]->setText(m_campaign->formatCurrency(temp->getBasePrice()));
            m_infos[4]->setText(m_campaign->formatCurrency(temp->getPriceSold()));
            m_infos[5]->setText(temp->getUnit()->getName());

            break;
        }
        case Type_PLU:
        {
            PLU* temp = m_campaign->getPLU(m_objectInternalID);
            if (temp == NULL)
            {
                break;
            }

            m_objectName->setText(temp->getName());

            m_infos[0]->setText(getLiteralStatus(temp->isActive()));
            m_infos[1]->setText(temp->getCategory()->getName());
            m_infos[2]->setText(m_campaign->formatCurrency(temp->getCosts()));
            m_infos[3]->setText(m_campaign->formatCurrency(temp->getPriceSold()));
            m_infos[4]->setText(m_campaign->formatCurrency(temp->getProfit()));
            m_infos[5]->setText(temp->getTaxAbbreviations());

            break;
        }
        case Type_CustomerCat:
        {
            CustomerCat* temp = m_campaign->getCustomerCat(m_objectInternalID);
            if (temp == NULL)
            {
                break;
            }

            m_objectName->setText(temp->getName());

            m_infos[0]->setText(getLiteralStatus(temp->isActive()));
            m_infos[1]->setText(QString::number(temp->getNumberOfCustomers()));
            m_infos[2]->setText(m_campaign->formatCurrency(temp->getTotalAmount()));

            break;
        }
        case Type_VendorCat:
        {
            VendorCat* temp = m_campaign->getVendorCat(m_objectInternalID);
            if (temp == NULL)
            {
                break;
            }

            m_objectName->setText(temp->getName());

            m_infos[0]->setText(getLiteralStatus(temp->isActive()));
            m_infos[1]->setText(QString::number(temp->getNumberOfVendors()));
            m_infos[2]->setText(m_campaign->formatCurrency(temp->getTotalSales()));

            break;
        }
        case Type_ProductCat:
        {
            ProductCat* temp = m_campaign->getProductCat(m_objectInternalID);
            if (temp == NULL)
            {
                break;
            }

            m_objectName->setText(temp->getName());

            m_infos[0]->setText(getLiteralStatus(temp->isActive()));
            m_infos[1]->setText(QString::number(temp->getNumberOfProducts()));
            m_infos[2]->setText(m_campaign->formatCurrency(temp->getAveragePriceSold()));
            m_infos[3]->setText(m_campaign->formatCurrency(temp->getAverageProfit()));

            break;
        }
        case Type_PLUCat:
        {
            PLUCat* temp = m_campaign->getPLUCat(m_objectInternalID);
            if (temp == NULL)
            {
                break;
            }

            m_objectName->setText(temp->getName());

            m_infos[0]->setText(getLiteralStatus(temp->isActive()));
            m_infos[1]->setText(QString::number(temp->getNumberOfPLUs()));
            m_infos[2]->setText(m_campaign->formatCurrency(temp->getAveragePriceSold()));
            m_infos[3]->setText(m_campaign->formatCurrency(temp->getAverageProfit()));

            break;
        }
        case Type_Discount:
        {
            Discount* temp = m_campaign->getDiscount(m_objectInternalID);
            if (temp == NULL)
            {
                break;
            }

            m_objectName->setText(tr("Discount # %1").arg(temp->getFormattedId()));

            m_infos[0]->setText(getLiteralStatus(temp->isActive()));
            m_infos[1]->setText(getLiteralDiscountApplicationType(temp->getApplicationType()));
            m_infos[2]->setText(temp->getInformations());

            break;
        }
        case Type_Unit:
        {
            Unit* temp = m_campaign->getUnit(m_objectInternalID);
            if (temp == NULL)
            {
                break;
            }

            m_objectName->setText(temp->getName());

            m_infos[0]->setText(getLiteralUnitType(temp->getType()));

            break;
        }
        case Type_Order:
        {
            Order* temp = m_campaign->getOrder(m_objectInternalID);
            if (temp == NULL)
            {
                break;
            }

            m_objectName->setText(tr("Order # %1").arg(temp->getFormattedId()));

            m_infos[0]->setText(getLiteralDeliveryStatus(temp->getStatus()));
            m_infos[1]->setText(temp->getCustomer()->getName() + ", " + temp->getCustomer()->getFirstName());
            m_infos[2]->setText(temp->getVendor()->getName() + ", " + temp->getVendor()->getFirstName());
            m_infos[3]->setText(m_campaign->formatCurrency(temp->getAmount()));
            m_infos[4]->setText(m_campaign->formatCurrency(temp->getAmountWithTaxes()));

            table->setRowCount(temp->getNumberOfItems());

            for (int i = 0; i < temp->getNumberOfItems(); i++)
            {
                OrderItem* item = temp->getItem(i);

                QString code, name;

                if (item->isPLU())
                {
                    code = item->getPLU()->getFormattedId();
                    name = item->getPLU()->getName();
                }
                else if (item->isService())
                {
                    code = item->getService()->getFormattedId();
                    name = item->getService()->getName();
                }
                else if (item->isProduct())
                {
                    code = item->getProduct()->getFormattedId();
                    name = item->getProduct()->getName();
                }

                QTableWidgetItem* codeWidget = new QTableWidgetItem(code);
                codeWidget->setFont(boldFont);

                table->setItem(i, 0, codeWidget);
                table->setItem(i, 1, new QTableWidgetItem(name));
                table->setItem(i, 2, new QTableWidgetItem(item->getFormattedQuantity()));
            }

            table->resizeColumnsToContents();

            break;
        }

    case Type_Order2:
          {
              Order2* temp = m_campaign->getOrder2(m_objectInternalID);
              if (temp == NULL)
              {
                  break;
              }

              m_objectName->setText(tr("Order2 # %1").arg(temp->getFormattedId()));

              m_infos[0]->setText(getLiteralDeliveryStatus(temp->getStatus()));
              m_infos[1]->setText(temp->getCustomer()->getName() + ", " + temp->getCustomer()->getFirstName());
              m_infos[2]->setText(temp->getVendor()->getName() + ", " + temp->getVendor()->getFirstName());
              m_infos[3]->setText(m_campaign->formatCurrency(temp->getAmount()));
              m_infos[4]->setText(m_campaign->formatCurrency(temp->getAmountWithTaxes()));

              table->setRowCount(temp->getNumberOfItems());

              for (int i = 0; i < temp->getNumberOfItems(); i++)
              {
                  OrderItem* item = temp->getItem(i);

                  QString code, name;

                  if (item->isPLU())
                  {
                      code = item->getPLU()->getFormattedId();
                      name = item->getPLU()->getName();
                  }
                  else if (item->isService())
                  {
                      code = item->getService()->getFormattedId();
                      name = item->getService()->getName();
                  }
                  else if (item->isProduct())
                  {
                      code = item->getProduct()->getFormattedId();
                      name = item->getProduct()->getName();
                  }

                  QTableWidgetItem* codeWidget = new QTableWidgetItem(code);
                  codeWidget->setFont(boldFont);

                  table->setItem(i, 0, codeWidget);
                  table->setItem(i, 1, new QTableWidgetItem(name));
                  table->setItem(i, 2, new QTableWidgetItem(item->getFormattedQuantity()));
              }

              table->resizeColumnsToContents();

              break;
          }

        case Type_Payment:
        {
            Customer* temp = m_campaign->getCustomer(m_objectInternalID);
            if (temp == NULL)
            {
                break;
            }

            m_objectName->setText(tr("Payments of %1 %2").arg(temp->getFirstName()).arg(temp->getName()));

            m_infos[0]->setText(m_campaign->formatCurrency(temp->getTotalAmountWithTaxes()));
            m_infos[1]->setText(m_campaign->formatCurrency(temp->getAmountPaid()));
            m_infos[2]->setText(m_campaign->formatCurrency(temp->getAmountDue()));

            break;
        }
        case Type_PaymentMode:
        {
            PaymentMode* temp = m_campaign->getPaymentMode(m_objectInternalID);
            if (temp == NULL)
            {
                break;
            }

            m_objectName->setText(temp->getName());

            m_infos[0]->setText(getLiteralStatus(temp->isActive()));
            m_infos[1]->setText(QString::number(temp->getNumberOfPayments()));
            m_infos[2]->setText(m_campaign->formatCurrency(temp->getTotalAmount()));

            break;
        }
        case Type_Supplier:
        {
            Supplier* temp = m_campaign->getSupplier(m_objectInternalID);
            if (temp == NULL)
            {
                break;
            }

            m_objectName->setText(temp->getName());

            m_infos[0]->setText(getLiteralStatus(temp->isActive()));
            m_infos[1]->setText(temp->getLocation()->phone1);
            m_infos[2]->setText(QString::number(temp->getNumberOfProducts()));
            m_infos[3]->setText(QString::number(temp->getNumberOfDeliveries()));

            break;
        }
        case Type_Tax:
        {
            Tax* temp = m_campaign->getTax(m_objectInternalID);
            if (temp == NULL)
            {
                break;
            }

            m_objectName->setText(temp->getName());

            m_infos[0]->setText(getLiteralStatus(temp->isActive()));
            m_infos[1]->setText(formatPercentage(temp->getRate(), 4));
            m_infos[2]->setText(m_campaign->formatCurrency(temp->getTotalAmount()));

            break;
        }
        case Type_User:
        {
            User* temp = m_campaign->getUser(m_objectInternalID);
            if (temp == NULL)
            {
                break;
            }

            m_objectName->setText(temp->getFirstName() + " " + temp->getName());

            m_infos[0]->setText(temp->getLoginInfos()->login);
            m_infos[1]->setText(getLiteralUserType(temp->getType()));

            break;
        }
        case Type_Delivery:
        {
            Delivery* temp = m_campaign->getDelivery(m_objectInternalID);
            if (temp == NULL)
            {
                break;
            }

            m_objectName->setText(tr("Delivery # %1").arg(temp->getFormattedId()));

            m_infos[0]->setText(getLiteralDeliveryStatus(temp->getStatus()));
            m_infos[1]->setText(temp->isPaid() ? tr("Paid") : tr("Not Paid"));
            m_infos[2]->setText(temp->getDate().toString(Qt::DefaultLocaleShortDate));
            m_infos[3]->setText(temp->getQuantityOfProducts().toString());
            m_infos[4]->setText(m_campaign->formatCurrency(temp->getAmount()));

            table->setRowCount(temp->getNumberOfItems());

            for (int i = 0; i < temp->getNumberOfItems(); i++)
            {
                DeliveryItem* item = temp->getItem(i);

                QTableWidgetItem* codeWidget = new QTableWidgetItem(item->getProduct()->getFormattedId());
                codeWidget->setFont(boldFont);

                table->setItem(i, 0, codeWidget);
                table->setItem(i, 1, new QTableWidgetItem(item->getProduct()->getName()));
                table->setItem(i, 2, new QTableWidgetItem(item->getProduct()->getUnit()->formatQuantity(item->getQuantity())));
            }

            table->resizeColumnsToContents();

            break;
        }


    case Type_Delivery2:
    {
        Delivery2* temp = m_campaign->getDelivery2(m_objectInternalID);
        if (temp == NULL)
        {
            break;
        }

        m_objectName->setText(tr("Delivery2 # %1").arg(temp->getFormattedId()));

        m_infos[0]->setText(getLiteralDeliveryStatus(temp->getStatus()));
        m_infos[1]->setText(temp->isPaid() ? tr("Paid") : tr("Not Paid"));
        m_infos[2]->setText(temp->getDate().toString(Qt::DefaultLocaleShortDate));
        m_infos[3]->setText(temp->getQuantityOfProducts().toString());
        m_infos[4]->setText(m_campaign->formatCurrency(temp->getAmount()));

        table->setRowCount(temp->getNumberOfItems());

        for (int i = 0; i < temp->getNumberOfItems(); i++)
        {
            DeliveryItem* item = temp->getItem(i);

            QTableWidgetItem* codeWidget = new QTableWidgetItem(item->getProduct()->getFormattedId());
            codeWidget->setFont(boldFont);

            table->setItem(i, 0, codeWidget);
            table->setItem(i, 1, new QTableWidgetItem(item->getProduct()->getName()));
            table->setItem(i, 2, new QTableWidgetItem(item->getProduct()->getUnit()->formatQuantity(item->getQuantity())));
        }

        table->resizeColumnsToContents();

        break;
    }

        case Type_Donator:
        {
            Donator* temp = m_campaign->getDonator(m_objectInternalID);
            if (temp == NULL)
            {
                break;
            }

            m_objectName->setText(temp->getFirstName() + " " + temp->getName());

            m_infos[0]->setText(getLiteralStatus(temp->isActive()));
            m_infos[1]->setText(getLiteralSex(temp->getSex()));
            m_infos[2]->setText(temp->getLocation()->adress);
            m_infos[3]->setText(temp->getLocation()->city + ", " + temp->getLocation()->country);
            m_infos[4]->setText(temp->getLocation()->phone1);
            m_infos[5]->setText(temp->getLocation()->email);
            m_infos[6]->setText(m_campaign->formatCurrency(temp->getTotalDonations()));

            break;
        }
        case Type_Donation:
        {
            Donation* temp = m_campaign->getDonation(m_objectInternalID);
            if (temp == NULL)
            {
                break;
            }

            m_objectName->setText(tr("Donation # %1").arg(temp->getFormattedId()));
            QString nameWho;

            if (temp->isDonator())
            {
                nameWho = temp->getDonator()->getName() + ", " + temp->getCustomer()->getFirstName() + tr(" (donator)");
            }
            else
            {
                nameWho = temp->getCustomer()->getName() + ", " + temp->getCustomer()->getFirstName() + tr(" (customer)");
            }

            m_infos[0]->setText(m_campaign->formatCurrency(temp->getAmount()));
            m_infos[1]->setText(nameWho);
            m_infos[2]->setText(temp->getDate().toString(Qt::DefaultLocaleShortDate));
            m_infos[3]->setText(temp->getPaymentMode()->getName());

            break;
        }
        case Type_Service:
        {
            Service* temp = m_campaign->getService(m_objectInternalID);
            if (temp == NULL)
            {
                break;
            }

            m_objectName->setText(temp->getName());

            m_infos[0]->setText(getLiteralStatus(temp->isActive()));
            m_infos[1]->setText(m_campaign->formatCurrency(temp->getCosts()));
            m_infos[2]->setText(m_campaign->formatCurrency(temp->getPriceSold()));
            m_infos[3]->setText(m_campaign->formatCurrency(temp->getProfit()));
            m_infos[4]->setText(temp->getTaxAbbreviations());

            break;
        }


        default:
            break;
    }
}

void CAMSEGObjectInfos::clearInfos()
{

    for(int i = 0; i < table->rowCount(); i++)
    table->removeRow(i);


}
