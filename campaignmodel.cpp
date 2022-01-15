#include "campaignmodel.h"
#include <QDebug>
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
#include <libCAMSEG/paymentmode.h>
#include <libCAMSEG/tax.h>
#include <libCAMSEG/stockadjustment.h>
#include <libCAMSEG/cashadjustment.h>
#include <libCAMSEG/donation.h>
#include <libCAMSEG/donator.h>
#include <libCAMSEG/service.h>
#include <libCAMSEG/expense.h>
#include <libCAMSEG/user.h>

using namespace CAMSEG;

CampaignModel::CampaignModel(const CAMSEG::ObjectType p_objectType, const CAMSEG::Campaign* p_campaign, QObject* parent) :
        QAbstractTableModel(parent),
        m_campaign(p_campaign),
        m_objectType(p_objectType),
        m_idCategory(-1),
        m_activityFilter(All),
        m_deliveryFilter(AllStatus),
        m_stringFilter(""),
        m_sortOrder(Qt::AscendingOrder),
        m_sortedColumn(-1)
{    
    setupHeaders();
    reloadObjectList();

    connect(m_campaign, SIGNAL(objectAdded(CAMSEG::ObjectType)), this, SLOT(objectAddedModified(CAMSEG::ObjectType)));
    connect(m_campaign, SIGNAL(objectRemoved(CAMSEG::ObjectType)), this, SLOT(objectAddedModified(CAMSEG::ObjectType)));
}

int CampaignModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return m_objects.count();
}

int CampaignModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return m_listHeaders.count();
}

QVariant CampaignModel::data(const QModelIndex &index, int role) const
{
    if (( ! index.isValid()) || index.row() >= rowCount() || index.column() >= columnCount() || role != Qt::DisplayRole)
        return QVariant();

    return m_objects[index.row()].value(index.column());
}

QVariant CampaignModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    return orientation == Qt::Horizontal ? m_listHeaders[section] : QString::number(section);
}

void CampaignModel::sort(int p_column, Qt::SortOrder p_order)
{
    m_sortedColumn = p_column;
    m_sortOrder = p_order;
    sortList();
}

void CampaignModel::reloadModel()
{
    reloadObjectList();
    emit layoutChanged();
}

quint32 CampaignModel::internalIdForRow(const quint32 p_row)
{
    if (p_row >= static_cast<quint32>(rowCount()))
        return 0;

    return m_objects[p_row].internalID();
}

void CampaignModel::setCategory(const int p_idCategory)
{
    m_idCategory = p_idCategory;
    reloadModel();
}

void CampaignModel::setActivityFilter(const CAMSEG::ActivityFilter p_filter)
{
    m_activityFilter = p_filter;
    reloadModel();
}

void CampaignModel::setDeliveryFilter(const CAMSEG::DeliveryStatusFilter p_filter)
{
    m_deliveryFilter = p_filter;
    reloadModel();
}

void CampaignModel::setStringFilter(const QString & p_filter)
{
    m_stringFilter = p_filter;
    reloadModel();
}

void CampaignModel::objectAddedModified(CAMSEG::ObjectType p_type)
{
    if (p_type == m_objectType)
    {
        reloadObjectList();
        emit layoutChanged();
    }
}

void CampaignModel::reloadObjectList()
{
    m_objects.clear();

    switch (m_objectType)
    {
    /*
        case Type_Customer:
        {
            Customer* temp = NULL;
            CustomerCat* cat = m_campaign->getCustomerCat(m_idCategory);

            for (quint32 i = 0; i < m_campaign->getNumberOfCustomers(); ++i)
            {
                temp = m_campaign->getCustomer(i);

                if ((cat == NULL || cat->isParentOf(temp)) &&
                    (temp->getFormattedId().contains(m_stringFilter, Qt::CaseInsensitive) || temp->getFullName().contains(m_stringFilter, Qt::CaseInsensitive)) &&
                    (m_activityFilter == All || (temp->isActive() && m_activityFilter == ActiveOnly) || (( ! temp->isActive()) && m_activityFilter == InactiveOnly)) )
                {
                    addObjectToList(temp);
                }
            }
            break;
        }
        */
    case Type_Customer:
    {
        Customer* temp = NULL;
        CustomerCat* cat = m_campaign->getCustomerCat(m_idCategory, RealID);

        for (quint32 i = 0; i < m_campaign->getNumberOfCustomers(); ++i)
        {
            temp = m_campaign->getCustomer(i);

            if ((cat == NULL || cat->isParentOf(temp)) &&
                (

temp->getLocation()->adress.contains(m_stringFilter, Qt::CaseInsensitive) ||
temp->getLocation()->cellphone.contains(m_stringFilter, Qt::CaseInsensitive) ||
temp->getLocation()->city.contains(m_stringFilter, Qt::CaseInsensitive) ||
temp->getLocation()->email.contains(m_stringFilter, Qt::CaseInsensitive) ||
temp->getLocation()->fax.contains(m_stringFilter, Qt::CaseInsensitive) ||
temp->getLocation()->phone1.contains(m_stringFilter, Qt::CaseInsensitive) ||
temp->getLocation()->phone2.contains(m_stringFilter, Qt::CaseInsensitive) ||
temp->getLocation()->zipcode.contains(m_stringFilter, Qt::CaseInsensitive) ||

                        temp->getFormattedId().contains(m_stringFilter, Qt::CaseInsensitive) || temp->getFullName().contains(m_stringFilter, Qt::CaseInsensitive)) &&
                (m_activityFilter == All || (temp->isActive() && m_activityFilter == ActiveOnly) || (( ! temp->isActive()) && m_activityFilter == InactiveOnly)) )
            {
                addObjectToList(temp);
            }
        }
        break;
    }

        case Type_Vendor:
        {
            Vendor* temp = NULL;
            VendorCat* cat = m_campaign->getVendorCat(m_idCategory, RealID);

            for (quint32 i = 0; i < m_campaign->getNumberOfVendors(); ++i)
            {
                temp = m_campaign->getVendor(i);

                if ((cat == NULL || cat->isParentOf(temp)) &&
                    (temp->getFormattedId().contains(m_stringFilter, Qt::CaseInsensitive) || temp->getFullName().contains(m_stringFilter, Qt::CaseInsensitive)) &&
                    (m_activityFilter == All || (temp->isActive() && m_activityFilter == ActiveOnly) || (( ! temp->isActive()) && m_activityFilter == InactiveOnly)) )
                {
                    addObjectToList(temp);
                }
            }
            break;
        }
    case Type_Product:
    {
        Product* temp = NULL;
        ProductCat* cat = m_campaign->getProductCat(m_idCategory, RealID);

        for (quint32 i = 0; i < m_campaign->getNumberOfProducts(); ++i)
        {
            temp = m_campaign->getProduct(i);

            if ((cat == NULL || cat->isParentOf(temp)) &&
                (temp->getFormattedId().contains(m_stringFilter, Qt::CaseInsensitive) || temp->getFullName().contains(m_stringFilter, Qt::CaseInsensitive)) &&
                (m_activityFilter == All || (temp->isActive() && m_activityFilter == ActiveOnly) || (( ! temp->isActive()) && m_activityFilter == InactiveOnly)) )
            {
                addObjectToList(temp);
            }
        }
        break;
    }
    case Type_PLU:
    {
        PLU* temp = NULL;
        PLUCat* cat = m_campaign->getPLUCat(m_idCategory, RealID);

        for (quint32 i = 0; i < m_campaign->getNumberOfPLUs(); ++i)
        {
            temp = m_campaign->getPLU(i);

            if ((cat == NULL || cat->isParentOf(temp)) &&
                (temp->getFormattedId().contains(m_stringFilter, Qt::CaseInsensitive) || temp->getFullName().contains(m_stringFilter, Qt::CaseInsensitive)) &&
                (m_activityFilter == All || (temp->isActive() && m_activityFilter == ActiveOnly) || (( ! temp->isActive()) && m_activityFilter == InactiveOnly)) )
            {
                addObjectToList(temp);
            }
        }
        break;
    }
    case Type_CustomerCat:
    {
        CustomerCat* temp = NULL;

        for (quint32 i = 0; i < m_campaign->getNumberOfCustomerCats(); ++i)
        {
            temp = m_campaign->getCustomerCat(i);

            if ((temp->getFormattedId().contains(m_stringFilter, Qt::CaseInsensitive) || temp->getFullName().contains(m_stringFilter, Qt::CaseInsensitive)) &&
                (m_activityFilter == All || (temp->isActive() && m_activityFilter == ActiveOnly) || (( ! temp->isActive()) && m_activityFilter == InactiveOnly)) )
            {
                addObjectToList(temp);
            }
        }
        break;
    }
    case Type_VendorCat:
    {
        VendorCat* temp = NULL;

        for (quint32 i = 0; i < m_campaign->getNumberOfVendorCats(); ++i)
        {
            temp = m_campaign->getVendorCat(i);

            if ((temp->getFormattedId().contains(m_stringFilter, Qt::CaseInsensitive) || temp->getFullName().contains(m_stringFilter, Qt::CaseInsensitive)) &&
                (m_activityFilter == All || (temp->isActive() && m_activityFilter == ActiveOnly) || (( ! temp->isActive()) && m_activityFilter == InactiveOnly)) )
            {
                addObjectToList(temp);
            }
        }
        break;
    }
    case Type_ProductCat:
    {
        ProductCat* temp = NULL;

        for (quint32 i = 0; i < m_campaign->getNumberOfProductCats(); ++i)
        {
            temp = m_campaign->getProductCat(i);

            if ((temp->getFormattedId().contains(m_stringFilter, Qt::CaseInsensitive) || temp->getFullName().contains(m_stringFilter, Qt::CaseInsensitive)) &&
                (m_activityFilter == All || (temp->isActive() && m_activityFilter == ActiveOnly) || (( ! temp->isActive()) && m_activityFilter == InactiveOnly)) )
            {
                addObjectToList(temp);
            }
        }
        break;
    }
    case Type_PLUCat:
    {
        PLUCat* temp = NULL;

        for (quint32 i = 0; i < m_campaign->getNumberOfPLUCats(); ++i)
        {
            temp = m_campaign->getPLUCat(i);

            if ((temp->getFormattedId().contains(m_stringFilter, Qt::CaseInsensitive) || temp->getFullName().contains(m_stringFilter, Qt::CaseInsensitive)) &&
                (m_activityFilter == All || (temp->isActive() && m_activityFilter == ActiveOnly) || (( ! temp->isActive()) && m_activityFilter == InactiveOnly)) )
            {
                addObjectToList(temp);
            }
        }
        break;
    }
    case Type_Discount:
    {
        Discount* temp = NULL;

        for (quint32 i = 0; i < m_campaign->getNumberOfDiscounts(); ++i)
        {
            temp = m_campaign->getDiscount(i);

            if ((temp->getFormattedId().contains(m_stringFilter, Qt::CaseInsensitive) || temp->getFullName().contains(m_stringFilter, Qt::CaseInsensitive)) &&
                (m_activityFilter == All || (temp->isActive() && m_activityFilter == ActiveOnly) || (( ! temp->isActive()) && m_activityFilter == InactiveOnly)) )
            {
                addObjectToList(temp);
            }
        }
        break;
    }
    case Type_Unit:
    {
        Unit* temp = NULL;

        for (quint32 i = 0; i < m_campaign->getNumberOfUnits(); ++i)
        {
            temp = m_campaign->getUnit(i);

            if (temp->getFormattedId().contains(m_stringFilter, Qt::CaseInsensitive) || temp->getFullName().contains(m_stringFilter, Qt::CaseInsensitive))
            {
                addObjectToList(temp);
            }
        }
        break;
    }
        /*
    case Type_Order:
    {
        Order* temp = NULL;

        for (quint32 i = 0; i < m_campaign->getNumberOfOrders(); ++i)
        {
            temp = m_campaign->getOrder(i);

            if ((m_deliveryFilter == AllStatus || (temp->isDelivered() && m_deliveryFilter == DeliveredOnly) || (( ! temp->isDelivered()) && m_deliveryFilter == NonDeliveredOnly)) &&
                (temp->getVendor()->getFullName().contains(m_stringFilter, Qt::CaseInsensitive) || temp->getCustomer()->getFullName().contains(m_stringFilter, Qt::CaseInsensitive) || temp->getFormattedId().contains(m_stringFilter, Qt::CaseInsensitive)) &&
                (m_campaign->getCurrentUser() != NULL || (m_campaign->getCurrentVendor() != NULL && m_campaign->getCurrentVendor()->getId() == temp->getVendor()->getId())))
            {
                addObjectToList(temp);
            }
        }
        break;
    }
*/
    case Type_Order:
    {
        Order* temp = NULL;

        for (quint32 i = 0; i < m_campaign->getNumberOfOrders(); ++i)
        {
            temp = m_campaign->getOrder(i);

            if ((m_deliveryFilter == AllStatus || (temp->isDelivered() && m_deliveryFilter == DeliveredOnly) || (( ! temp->isDelivered()) && m_deliveryFilter == NonDeliveredOnly)) &&
                (
temp->getFactureId().contains(m_stringFilter, Qt::CaseInsensitive) ||
temp->getInformations().contains(m_stringFilter, Qt::CaseInsensitive) ||
temp->getDate().toString("yyyy-mm-dd").contains(m_stringFilter, Qt::CaseInsensitive) ||


                        temp->getVendor()->getFullName().contains(m_stringFilter, Qt::CaseInsensitive) || temp->getCustomer()->getFullName().contains(m_stringFilter, Qt::CaseInsensitive) || temp->getFormattedId().contains(m_stringFilter, Qt::CaseInsensitive)) &&
                (m_campaign->getCurrentUser() != NULL || (m_campaign->getCurrentVendor() != NULL && m_campaign->getCurrentVendor()->getId() == temp->getVendor()->getId())))
            {
                addObjectToList(temp);
            }
        }

        break;
    }

    case Type_Order2:
    {
        Order2* temp = NULL;

        for (quint32 i = 0; i < m_campaign->getNumberOfOrders2(); ++i)
        {
            temp = m_campaign->getOrder2(i);

            if ((m_deliveryFilter == AllStatus || (temp->isDelivered() && m_deliveryFilter == DeliveredOnly) || (( ! temp->isDelivered()) && m_deliveryFilter == NonDeliveredOnly)) &&
                (temp->getVendor()->getFullName().contains(m_stringFilter, Qt::CaseInsensitive) || temp->getCustomer()->getFullName().contains(m_stringFilter, Qt::CaseInsensitive) || temp->getFormattedId().contains(m_stringFilter, Qt::CaseInsensitive)) &&
                (m_campaign->getCurrentUser() != NULL || (m_campaign->getCurrentVendor() != NULL && m_campaign->getCurrentVendor()->getId() == temp->getVendor()->getId())))
            {
                addObjectToList(temp);
            }
        }
        break;
    }
    case Type_Payment:
    {
        Customer* temp = NULL;

        for (quint32 i = 0; i < m_campaign->getNumberOfCustomers(); ++i)
        {
            temp = m_campaign->getCustomer(i);

            if ((temp->getFormattedId().contains(m_stringFilter, Qt::CaseInsensitive) || temp->getFullName().contains(m_stringFilter, Qt::CaseInsensitive)) &&
                (m_activityFilter == All || (temp->isActive() && m_activityFilter == ActiveOnly) || (( ! temp->isActive()) && m_activityFilter == InactiveOnly)) )
            {
                addObjectToList(temp);
            }
        }
        break;
    }
    case Type_PaymentMode:
    {
        PaymentMode* temp = NULL;

        for (quint32 i = 0; i < m_campaign->getNumberOfPaymentModes(); ++i)
        {
            temp = m_campaign->getPaymentMode(i);

            if ((temp->getFormattedId().contains(m_stringFilter, Qt::CaseInsensitive) || temp->getFullName().contains(m_stringFilter, Qt::CaseInsensitive)) &&
                (m_activityFilter == All || (temp->isActive() && m_activityFilter == ActiveOnly) || (( ! temp->isActive()) && m_activityFilter == InactiveOnly)) )
            {
                addObjectToList(temp);
            }
        }
        break;
    }
        /*
    case Type_Supplier:
    {
        Supplier* temp = NULL;

        for (quint32 i = 0; i < m_campaign->getNumberOfSuppliers(); ++i)
        {
            temp = m_campaign->getSupplier(i);

            if ((temp->getFormattedId().contains(m_stringFilter, Qt::CaseInsensitive) || temp->getFullName().contains(m_stringFilter, Qt::CaseInsensitive)) &&
                (m_activityFilter == All || (temp->isActive() && m_activityFilter == ActiveOnly) || (( ! temp->isActive()) && m_activityFilter == InactiveOnly)) )
            {
                addObjectToList(temp);
            }
        }
        break;
    }
    */
    case Type_Supplier:
    {
        Supplier* temp = NULL;

        for (quint32 i = 0; i < m_campaign->getNumberOfSuppliers(); ++i)
        {
            temp = m_campaign->getSupplier(i);

            if ((
temp->getInformations().contains(m_stringFilter, Qt::CaseInsensitive) ||
temp->getLocation()->adress.contains(m_stringFilter, Qt::CaseInsensitive) ||
temp->getLocation()->cellphone.contains(m_stringFilter, Qt::CaseInsensitive) ||
temp->getLocation()->city.contains(m_stringFilter, Qt::CaseInsensitive) ||
temp->getLocation()->country.contains(m_stringFilter, Qt::CaseInsensitive) ||
temp->getLocation()->email.contains(m_stringFilter, Qt::CaseInsensitive) ||
temp->getLocation()->fax.contains(m_stringFilter, Qt::CaseInsensitive) ||
temp->getLocation()->phone1.contains(m_stringFilter, Qt::CaseInsensitive) ||
temp->getLocation()->phone2.contains(m_stringFilter, Qt::CaseInsensitive) ||
temp->getLocation()->zipcode.contains(m_stringFilter, Qt::CaseInsensitive) ||

                    temp->getFormattedId().contains(m_stringFilter, Qt::CaseInsensitive) || temp->getFullName().contains(m_stringFilter, Qt::CaseInsensitive)) &&
                (m_activityFilter == All || (temp->isActive() && m_activityFilter == ActiveOnly) || (( ! temp->isActive()) && m_activityFilter == InactiveOnly)) )
            {
                addObjectToList(temp);
            }
        }
        break;
    }


    case Type_Delivery:
    {
        Delivery* temp = NULL;

        for (quint32 i = 0; i < m_campaign->getNumberOfDeliveries(); ++i)
        {
            temp = m_campaign->getDelivery(i);

            if ((m_deliveryFilter == AllStatus || (temp->isDelivered() && m_deliveryFilter == DeliveredOnly) || (( ! temp->isDelivered()) && m_deliveryFilter == NonDeliveredOnly)) &&
                (temp->getSupplier()->getName().contains(m_stringFilter, Qt::CaseInsensitive) || temp->getFormattedId().contains(m_stringFilter, Qt::CaseInsensitive)))
            {
                addObjectToList(temp);
            }
        }
        break;
    }

    case Type_Delivery2:
    {
        Delivery2* temp = NULL;

        for (quint32 i = 0; i < m_campaign->getNumberOfDeliveries2(); ++i)
        {
            temp = m_campaign->getDelivery2(i);

            if ((m_deliveryFilter == AllStatus || (temp->isDelivered() && m_deliveryFilter == DeliveredOnly) || (( ! temp->isDelivered()) && m_deliveryFilter == NonDeliveredOnly)) &&
                (temp->getSupplier()->getName().contains(m_stringFilter, Qt::CaseInsensitive) || temp->getFormattedId().contains(m_stringFilter, Qt::CaseInsensitive)))
            {
                addObjectToList(temp);
            }
        }
        break;
    }
    case Type_Tax:
    {
        Tax* temp = NULL;

        for (quint32 i = 0; i < m_campaign->getNumberOfTaxes(); ++i)
        {
            temp = m_campaign->getTax(i);

            if ((temp->getFormattedId().contains(m_stringFilter, Qt::CaseInsensitive) || temp->getFullName().contains(m_stringFilter, Qt::CaseInsensitive)) &&
                (m_activityFilter == All || (temp->isActive() && m_activityFilter == ActiveOnly) || (( ! temp->isActive()) && m_activityFilter == InactiveOnly)) )
            {
                addObjectToList(temp);
            }
        }
        break;
    }
    case Type_User:
    {
        User* temp = NULL;

        for (quint32 i = 0; i < m_campaign->getNumberOfUsers(); ++i)
        {
            temp = m_campaign->getUser(i);

            if ((temp->getFormattedId().contains(m_stringFilter, Qt::CaseInsensitive) || temp->getFullName().contains(m_stringFilter, Qt::CaseInsensitive)))
            {
                addObjectToList(temp);
            }
        }
        break;
    }
    case Type_StockAdj:
    {
        StockAdjustment* temp = NULL;

        for (quint32 i = 0; i < m_campaign->getNumberOfStockAdjustments(); ++i)
        {
            temp = m_campaign->getStockAdjustment(i);

            if (temp->getFormattedId().contains(m_stringFilter, Qt::CaseInsensitive))
            {
                addObjectToList(temp);
            }
        }
        break;
    }
    case Type_CashAdj:
    {
        CashAdjustment* temp = NULL;

        for (quint32 i = 0; i < m_campaign->getNumberOfCashAdjustments(); ++i)
        {
            temp = m_campaign->getCashAdjustment(i);

            if (temp->getFormattedId().contains(m_stringFilter, Qt::CaseInsensitive))
            {
                addObjectToList(temp);
            }
        }
        break;
    }
    case Type_Expense:
    {
        Expense* temp = NULL;

        for (quint32 i = 0; i < m_campaign->getNumberOfExpenses(); ++i)
        {
            temp = m_campaign->getExpense(i);

            if (temp->getFormattedId().contains(m_stringFilter, Qt::CaseInsensitive))
            {
                addObjectToList(temp);
            }
        }
        break;
    }
    case Type_Donator:
    {
        Donator* temp = NULL;

        for (quint32 i = 0; i < m_campaign->getNumberOfDonators(); ++i)
        {
            temp = m_campaign->getDonator(i);

            if ((temp->getFormattedId().contains(m_stringFilter, Qt::CaseInsensitive) || temp->getFullName().contains(m_stringFilter, Qt::CaseInsensitive)) &&
                (m_activityFilter == All || (temp->isActive() && m_activityFilter == ActiveOnly) || (( ! temp->isActive()) && m_activityFilter == InactiveOnly)) )
            {
                addObjectToList(temp);
            }
        }
        break;
    }
    case Type_Donation:
    {
        Donation* temp = NULL;

        for (quint32 i = 0; i < m_campaign->getNumberOfDonations(); ++i)
        {
            temp = m_campaign->getDonation(i);

            if (temp->getFormattedId().contains(m_stringFilter, Qt::CaseInsensitive))
            {
                addObjectToList(temp);
            }
        }
        break;
    }
    case Type_Service:
    {
        Service* temp = NULL;

        for (quint32 i = 0; i < m_campaign->getNumberOfServices(); ++i)
        {
            temp = m_campaign->getService(i);

            if ((temp->getFormattedId().contains(m_stringFilter, Qt::CaseInsensitive) || temp->getFullName().contains(m_stringFilter, Qt::CaseInsensitive)) &&
                (m_activityFilter == All || (temp->isActive() && m_activityFilter == ActiveOnly) || (( ! temp->isActive()) && m_activityFilter == InactiveOnly)) )
            {
                addObjectToList(temp);
            }
        }
        break;
    }
    default:
        break;
    }

    sortList();
}

void CampaignModel::addObjectToList(const CAMSEG::CampaignObject* p_object)
{
    m_objects << CampaignModelItem(p_object, this);
}

void CampaignModel::setupHeaders()
{
    m_listHeaders.clear();

    switch (m_objectType)
    {
    case Type_Customer:
        m_listHeaders << tr("Cust. Code")
                      << tr("Name")
                      << tr("First Name")
                      << tr("Category")
                      << tr("Phone")
                      << tr("Gross Sales");
        break;
    case Type_Vendor:
        m_listHeaders << tr("Vend. Code")
                      << tr("Name")
                      << tr("First Name")
                      << tr("Category")
                      << tr("Phone")
                      << tr("Gross Sales");

        break;
    case Type_Product:
        m_listHeaders << tr("Product SKU")
                      << tr("Supplier SKU")
                      << tr("Name")
                      << tr("Category")
                      << tr("Supplier")
                      << tr("Cost Price")
                      << tr("Rec. Price Sold");

        break;
    case Type_PLU:
        m_listHeaders << tr("PLU Code")
                      << tr("Name")
                      << tr("Category")
                      << tr("Costs")
                      << tr("Price Sold")
                      << tr("Taxes");

        break;
    case Type_CustomerCat:
        m_listHeaders << tr("ID")
                      << tr("Name")
                      << tr("Customers") ;
        break;
    case Type_VendorCat:
        m_listHeaders << tr("ID")
                      << tr("Name")
                      << tr("Vendors");
        break;
    case Type_ProductCat:
        m_listHeaders << tr("ID")
                      << tr("Name")
                      << tr("Products");
        break;
    case Type_PLUCat:
        m_listHeaders << tr("ID")
                      << tr("Name")
                      << tr("PLUs");
        break;
    case Type_Discount:
        m_listHeaders << tr("Discount Code")
                      << tr("Name")
                      << tr("Application Type")
                      << tr("Status")
                      << tr("Expiration Date");

        break;
    case Type_Unit:
        m_listHeaders << tr("ID")
                      << tr("Name")
                      << tr("Type");
        break;
    case Type_Order:
        m_listHeaders << tr("ID")
                      << tr("Facture")
                      << tr("Vendor")
                      << tr("Customer")
                      << tr("Date")
                      << tr("Status")
                      << tr("Amount")
                      << tr("Amount + Taxes")
                      << tr("Agent");

        break;

    case Type_Order2:
        m_listHeaders << tr("ID")
                      << tr("Vendor")
                      << tr("Customer")
                      << tr("Date")
                      << tr("Status")
                      << tr("Amount")
                      << tr("Amount + Taxes");

        break;
    case Type_Payment:
        m_listHeaders << tr("Cust. Code")
                      << tr("Name")
                      << tr("Gross Sales")
                      << tr("Amount Paid")
                      << tr("Amount Due");
        break;
    case Type_PaymentMode:
        m_listHeaders << tr("ID")
                      << tr("Name")
                      << tr("Amount");
        break;
    case Type_Supplier:
        m_listHeaders << tr("ID")
                      << tr("Name")
                      << tr("City")
                      << tr("Phone")
                      << tr("Deliveries");
        break;
    case Type_Delivery:
        m_listHeaders << tr("ID")
                      << tr("Supplier")
                      << tr("Date")
                      << tr("Status")
                      << tr("Num. of items")
                      << tr("Cost");
        break;

    case Type_Delivery2:
        m_listHeaders << tr("ID")
                      << tr("Supplier")
                      << tr("Date")
                      << tr("Status")
                      << tr("Num. of items")
                      << tr("Cost");
        break;
    case Type_Tax:
        m_listHeaders << tr("ID")
                      << tr("Name")
                      << tr("Rate")
                      << tr("Total Amount");
        break;
    case Type_User:
        m_listHeaders << tr("ID")
                      << tr("Username")
                      << tr("Type");
        break;
    case Type_StockAdj:
        m_listHeaders << tr("ID")
                      << tr("Date")
                      << tr("Product")
                      << tr("Adjustment");
        break;
    case Type_CashAdj:
        m_listHeaders << tr("ID")
                      << tr("Date")
                      << tr("Amount");
        break;
    case Type_Expense:
        m_listHeaders << tr("ID")
                      << tr("Amount")
                      << tr("Type")
                      << tr("Date");
        break;
    case Type_Donator:
        m_listHeaders << tr("ID")
                      << tr("Name")
                      << tr("First Name")
                      << tr("Total Donated");
        break;
    case Type_Donation:
        m_listHeaders << tr("ID")
                      << tr("Amount")
                      << tr("Who")
                      << tr("Mode")
                      << tr("Date");
        break;
    case Type_Service:
        m_listHeaders << tr("Service Code")
                      << tr("Name")
                      << tr("Costs")
                      << tr("Price Sold")
                      << tr("Taxes");
        break;
    default:
        break;
    }
}

void CampaignModel::sortList()
{
    if (m_sortedColumn == -1)
        return;

    //Sort the list
    m_sortOrder == Qt::AscendingOrder ? qSort(m_objects.begin(), m_objects.end()) : qSort(m_objects.begin(), m_objects.end(), qGreater<CampaignModelItem>());

    //Emit signal
    emit dataChanged(QModelIndex(), QModelIndex());
}

inline bool CampaignModelItem::operator <(const CampaignModelItem& p_other) const
{
    bool ok1, ok2;

    QString str1 = value(m_model->m_sortedColumn, Qt::UserRole).toString(),
            str2 = p_other.value(m_model->m_sortedColumn, Qt::UserRole).toString();
    double num1 = str1.toDouble(&ok1),
           num2 = str2.toDouble(&ok2);

    return (ok1 && ok2) ? num1 < num2 : str1 < str2;
}

QVariant CampaignModelItem::value(const int p_column, int role) const
{
    if (m_object == NULL || p_column < 0)
        return QVariant();

    switch (m_model->m_objectType)
    {
    case Type_Customer:
        {
            const Customer* object = dynamic_cast<const Customer*>(m_object);

            switch (p_column)
            {
            case 0:
                return object->getFormattedId();
                break;
            case 1:
                return object->getName();
                break;
            case 2:
                return object->getFirstName();
                break;
            case 3:
                return object->getCategory()->getName();
                break;
            case 4:
                return object->getLocation()->phone1;
                break;
            case 5:
                return role == Qt::DisplayRole ? QVariant(m_model->m_campaign->formatCurrency(object->getTotalAmountWithTaxes())) : QVariant(object->getTotalAmountWithTaxes().toDouble());
                break;
            }
            break;
        }
    case Type_Vendor:
        {
            const Vendor* object = dynamic_cast<const Vendor*>(m_object);

            switch (p_column)
            {
            case 0:
                return object->getFormattedId();
                break;
            case 1:
                return object->getName();
                break;
            case 2:
                return object->getFirstName();
                break;
            case 3:
                return object->getCategory()->getName();
                break;
            case 4:
                return object->getLocation()->phone1;
                break;
            case 5:
                return  role == Qt::DisplayRole ? QVariant(m_model->m_campaign->formatCurrency(object->getTotalGrossSales())) : object->getTotalGrossSales().toDouble();
                break;
            }
            break;
        }
    case Type_Product:
        {
            const Product* object = dynamic_cast<const Product*>(m_object);

            switch (p_column)
            {
            case 0:
                return object->getFormattedId();
                break;
            case 1:
                return object->getSupplierId();
                break;
            case 2:
                return object->getName();
                break;
            case 3:
                return object->getCategory()->getName();
                break;
            case 4:
                return object->getSupplier()->getName();
                break;
            case 5:
                return  role == Qt::DisplayRole ? QVariant(m_model->m_campaign->formatCurrency(object->getBasePrice())) : object->getBasePrice().toDouble();
                break;
            case 6:
                return  role == Qt::DisplayRole ? QVariant(m_model->m_campaign->formatCurrency(object->getPriceSold())) : object->getPriceSold().toDouble();
                break;
            }
            break;
        }
    case Type_PLU:
        {
            const PLU* object = dynamic_cast<const PLU*>(m_object);

            switch (p_column)
            {
            case 0:
                return object->getFormattedId();
                break;
            case 1:
                return object->getName();
                break;
            case 2:
                return object->getCategory()->getName();
                break;
            case 3:
                return  role == Qt::DisplayRole ? QVariant(m_model->m_campaign->formatCurrency(object->getCosts())) : object->getCosts().toDouble();
                break;
            case 4:
                return  role == Qt::DisplayRole ? QVariant(m_model->m_campaign->formatCurrency(object->getPriceSold())) : object->getPriceSold().toDouble();
                break;
            case 5:
                return object->getTaxAbbreviations();
                break;
            }
            break;
        }
    case Type_CustomerCat:
        {
            const CustomerCat* object = dynamic_cast<const CustomerCat*>(m_object);

            switch (p_column)
            {
            case 0:
                return object->getFormattedId();
                break;
            case 1:
                return object->getName();
                break;
            case 2:
                return role == Qt::DisplayRole ? QVariant(QString::number(object->getNumberOfCustomers())) : object->getNumberOfCustomers();
                break;
            }
            break;
        }
    case Type_VendorCat:
        {
            const VendorCat* object = dynamic_cast<const VendorCat*>(m_object);

            switch (p_column)
            {
            case 0:
                return object->getFormattedId();
                break;
            case 1:
                return object->getName();
                break;
            case 2:
                return role == Qt::DisplayRole ? QVariant(QString::number(object->getNumberOfVendors())) : object->getNumberOfVendors();
                break;
            }
            break;
        }
    case Type_ProductCat:
        {
            const ProductCat* object = dynamic_cast<const ProductCat*>(m_object);

            switch (p_column)
            {
            case 0:
                return object->getFormattedId();
                break;
            case 1:
                return object->getName();
                break;
            case 2:
                return role == Qt::DisplayRole ? QVariant(QString::number(object->getNumberOfProducts())) : object->getNumberOfProducts();
                break;
            }
            break;
        }
    case Type_PLUCat:
        {
            const PLUCat* object = dynamic_cast<const PLUCat*>(m_object);

            switch (p_column)
            {
            case 0:
                return object->getFormattedId();
                break;
            case 1:
                return object->getName();
                break;
            case 2:
                return role == Qt::DisplayRole ? QVariant(QString::number(object->getNumberOfPLUs())) : object->getNumberOfPLUs();
                break;
            }
            break;
        }
    case Type_Discount:
        {
            const Discount* object = dynamic_cast<const Discount*>(m_object);

            switch (p_column)
            {
            case 0:
                return object->getFormattedId();
                break;
            case 1:
                return object->getName();
                break;
            case 2:
                return getLiteralDiscountApplicationType(object->getApplicationType());
                break;
            case 3:
                return getLiteralStatus(object->isActive());
                break;
            case 4:
                return object->getExpirationDate().toString(Qt::ISODate);
                break;
            }
            break;
        }
    case Type_Unit:
        {
            const Unit* object = dynamic_cast<const Unit*>(m_object);

            switch (p_column)
            {
            case 0:
                return object->getFormattedId();
                break;
            case 1:
                return object->getName();
                break;
            case 2:
                return getLiteralUnitType(object->getType());
                break;
            }
            break;
        }
    case Type_Order:
        {
            const Order* object = dynamic_cast<const Order*>(m_object);            

            switch (p_column)
            {
            case 0:
                return object->getFormattedId();
                break;
            case 1:
                return object->getFactureId();
                break;
            case 2:
                return object->getVendor()->getFullName();
                break;
            case 3:
                return object->getCustomer()->getFullName();
                break;
            case 4:
                //return object->getDate().toString(Qt::ISODate);
                return object->getDate().toString(Qt::ISODate);
                break;
            case 5:
                return getLiteralDeliveryStatus(object->getStatus());
                break;
            case 6:
                return role == Qt::DisplayRole ? QVariant(m_model->m_campaign->formatCurrency(object->getAmount())) : object->getAmount().toDouble();
                break;
            case 7:
                return role == Qt::DisplayRole ? QVariant(m_model->m_campaign->formatCurrency(object->getAmountWithTaxes())) : object->getAmountWithTaxes().toDouble();
                break;
            case 8:
                return object->getInformations();
                break;

            }
            break;
        }



    case Type_Order2:
        {
            const Order2* object = dynamic_cast<const Order2*>(m_object);

            switch (p_column)
            {
            case 0:
                return object->getFormattedId();
                break;
            case 1:
                return object->getVendor()->getFullName();
                break;
            case 2:
                return object->getCustomer()->getFullName();
                break;
            case 3:
                //return object->getDate().toString(Qt::ISODate);
                return object->getDate().toString(Qt::ISODate);
                break;
            case 4:
                return getLiteralDeliveryStatus(object->getStatus());
                break;
            case 5:
                return role == Qt::DisplayRole ? QVariant(m_model->m_campaign->formatCurrency(object->getAmount())) : object->getAmount().toDouble();
                break;
            case 6:
                return role == Qt::DisplayRole ? QVariant(m_model->m_campaign->formatCurrency(object->getAmountWithTaxes())) : object->getAmountWithTaxes().toDouble();
                break;

            }
            break;
        }
    case Type_Payment:
        {
            const Customer* object = dynamic_cast<const Customer*>(m_object);

            switch (p_column)
            {
            case 0:
                return object->getFormattedId();
                break;
            case 1:
                return object->getFullName();
                break;
            case 2:
                return role == Qt::DisplayRole ? QVariant(m_model->m_campaign->formatCurrency(object->getTotalAmountWithTaxes())) : object->getTotalAmountWithTaxes().toDouble();
                break;
            case 3:
                return role == Qt::DisplayRole ? QVariant(m_model->m_campaign->formatCurrency(object->getAmountPaid())) : object->getAmountPaid().toDouble();
                break;
            case 4:
                return role == Qt::DisplayRole ? QVariant(m_model->m_campaign->formatCurrency(object->getAmountDue())) : object->getAmountDue().toDouble();
                break;
            }
            break;
        }
    case Type_PaymentMode:
        {
            const PaymentMode* object = dynamic_cast<const PaymentMode*>(m_object);

            switch (p_column)
            {
            case 0:
                return object->getFormattedId();
                break;
            case 1:
                return object->getName();
                break;
            case 2:
                return role == Qt::DisplayRole ? QVariant(m_model->m_campaign->formatCurrency(object->getTotalAmount())) : object->getTotalAmount().toDouble();
                break;
            }
            break;
        }
    case Type_Supplier:
        {
            const Supplier* object = dynamic_cast<const Supplier*>(m_object);

            switch (p_column)
            {
            case 0:
                return object->getFormattedId();
                break;
            case 1:
                return object->getName();
                break;
            case 2:
                return object->getLocation()->city;
                break;
            case 3:
                return object->getLocation()->phone1;
                break;
            case 4:
                return role == Qt::DisplayRole ? QVariant(QString::number(object->getNumberOfDeliveries())) : object->getNumberOfDeliveries();
                break;
            }
            break;
        }
    case Type_Delivery:
        {
            const Delivery* object = dynamic_cast<const Delivery*>(m_object);

            switch (p_column)
            {
            case 0:
                return object->getFormattedId();
                break;
            case 1:
                return object->getSupplier()->getName();
                break;
            case 2:
                //return object->getDate().toString(Qt::ISODate);
                return object->getDate().toString(Qt::ISODate);
                break;
            case 3:
                return getLiteralDeliveryStatus(object->getStatus());
                break;
            case 4:
                return role == Qt::DisplayRole ? QVariant(QString::number(object->getNumberOfItems())) : object->getNumberOfItems();
                break;
            case 5:
                return role == Qt::DisplayRole ? QVariant(m_model->m_campaign->formatCurrency(object->getAmount())) : object->getAmount().toDouble();
                break;
            }
            break;
        }

    case Type_Delivery2:
        {
            const Delivery2* object = dynamic_cast<const Delivery2*>(m_object);

            switch (p_column)
            {
            case 0:
                return object->getFormattedId();
                break;
            case 1:
                return object->getSupplier()->getName();
                break;
            case 2:
                //return object->getDate().toString(Qt::ISODate);
                return object->getDate().toString(Qt::ISODate);
                break;
            case 3:
                return getLiteralDeliveryStatus(object->getStatus());
                break;
            case 4:
                return role == Qt::DisplayRole ? QVariant(QString::number(object->getNumberOfItems())) : object->getNumberOfItems();
                break;
            case 5:
                return role == Qt::DisplayRole ? QVariant(m_model->m_campaign->formatCurrency(object->getAmount())) : object->getAmount().toDouble();
                break;
            }
            break;
        }
    case Type_Tax:
        {
            const Tax* object = dynamic_cast<const Tax*>(m_object);

            switch (p_column)
            {
            case 0:
                return object->getFormattedId();
                break;
            case 1:
                return object->getName();
                break;
            case 2:
                return role == Qt::DisplayRole ? QVariant(formatPercentage(object->getRate(), 4)) : object->getRate();
                break;
            case 3:
                return role == Qt::DisplayRole ? QVariant(m_model->m_campaign->formatCurrency(object->getTotalAmount())) : object->getTotalAmount().toDouble();
                break;
            }
            break;
        }
    case Type_User:
        {
            const User* object = dynamic_cast<const User*>(m_object);

            switch (p_column)
            {
            case 0:
                return object->getFormattedId();
                break;
            case 1:
                return object->getLoginInfos()->login;
                break;
            case 2:
                return getLiteralUserType(object->getType());
                break;
            }
            break;
        }
    case Type_StockAdj:
        {
            const StockAdjustment* object = dynamic_cast<const StockAdjustment*>(m_object);

            switch (p_column)
            {
            case 0:
                return object->getFormattedId();
                break;
            case 1:
                return object->getDate().toString(Qt::ISODate);
                break;
            case 2:
                return object->getProduct()->getName();
                break;
            case 3:
                return role == Qt::DisplayRole ? QVariant(object->getProduct()->getUnit()->formatQuantity(object->getAdjustmentQuantity())) : object->getAdjustmentQuantity().toDouble();
                break;
            }
            break;
        }
    case Type_CashAdj:
        {
            const CashAdjustment* object = dynamic_cast<const CashAdjustment*>(m_object);

            switch (p_column)
            {
            case 0:
                return object->getFormattedId();
                break;
            case 1:
                return object->getDate().toString(Qt::ISODate);
                break;
            case 2:
                return role == Qt::DisplayRole ? QVariant(m_model->m_campaign->formatCurrency(object->getAmount())) : object->getAmount().toDouble();
                break;
            }
            break;
        }
    case Type_Expense:
        {
            const Expense* object = dynamic_cast<const Expense*>(m_object);

            switch (p_column)
            {
            case 0:
                return object->getFormattedId();
                break;
            case 1:
                return object->getDate().toString(Qt::ISODate);
                break;
            case 2:
                return role == Qt::DisplayRole ? QVariant(m_model->m_campaign->formatCurrency(object->getAmount())) : object->getAmount().toDouble();
                break;
            }
            break;
        }
    case Type_Donator:
        {
            const Donator* object = dynamic_cast<const Donator*>(m_object);

            switch (p_column)
            {
            case 0:
                return object->getFormattedId();
                break;
            case 1:
                return object->getName();
                break;
            case 2:
                return object->getFirstName();
                break;
            case 3:
                return role == Qt::DisplayRole ? QVariant(m_model->m_campaign->formatCurrency(object->getTotalDonations())) : object->getTotalDonations().toDouble();
                break;
            }
            break;
        }
    case Type_Donation:
        {
            const Donation* object = dynamic_cast<const Donation*>(m_object);

            switch (p_column)
            {
            case 0:
                return object->getFormattedId();
                break;
            case 1:
                return role == Qt::DisplayRole ? QVariant(m_model->m_campaign->formatCurrency(object->getAmount())) : object->getAmount().toDouble();
                break;
            case 2:
                return object->isDonator() ? object->getDonator()->getFullName() : object->getCustomer()->getFullName();
                break;
            case 3:
                return object->getPaymentMode()->getName();
                break;
            case 4:
                return object->getDate().toString(Qt::ISODate);
                break;
            }
            break;
        }
    case Type_Service:
        {
            const Service* object = dynamic_cast<const Service*>(m_object);

            switch (p_column)
            {
            case 0:
                return object->getFormattedId();
                break;
            case 1:
                return object->getName();
                break;
            case 2:
                return role == Qt::DisplayRole ? QVariant(m_model->m_campaign->formatCurrency(object->getCosts())) : object->getCosts().toDouble();
                break;
            case 3:
                return role == Qt::DisplayRole ? QVariant(m_model->m_campaign->formatCurrency(object->getPriceSold())) : object->getPriceSold().toDouble();
                break;
            case 4:
                return object->getTaxAbbreviations();
                break;
            }
            break;
        }
    default:
        break;
    }

    return QVariant();
}

inline quint32 CampaignModelItem::internalID() const
{
    return m_object->getInternalId();
}





