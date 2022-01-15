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

#include "camsegtable.h"
#include "global.h"
#include "camsegobjectmodifier.h"
#include "campaignmodel.h"

#include <libCAMSEG/campaign.h>
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
#include <QTreeWidget>
#include <QComboBox>
#include <QListWidget>
#include <QToolButton>
#include <QStandardItemModel>
#include <QSignalMapper>

using namespace CAMSEG;

CAMSEGTable::CAMSEGTable(ObjectType p_viewType, Campaign* p_campaign, QWidget* parent) :
        QSplitter(parent), m_type(p_viewType), m_campaign(p_campaign)
{
    m_table = new CAMSEGTableView();
    m_table ->resizeColumnsToContents();
    m_catTree = new QTreeWidget();

    m_campaign

    m_model = new CampaignModel(m_type, m_campaign, this);
    m_table->setModel(m_model);

    rightWidget = new QWidget();

    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setSelectionMode(QAbstractItemView::SingleSelection);
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table->verticalHeader()->hide();
    m_table->setSortingEnabled(true);

    m_table->setAlternatingRowColors(true);

    m_catTree->header()->hide();

    setupBaseTable();
    fillCats();

    //Filter Widget
    filterWidget = new QWidget();
    txtNameFilter = new QLineEdit();
    cboStatusFilter = new QComboBox();
    btnClose = new QToolButton();

    txtNameFilter->setMinimumWidth(150);

    if (m_type != Type_Order && m_type != Type_Delivery)
    {
        CAMSEGObjectModifier::fillComboBox(cboStatusFilter, NULL, CAMSEGObjectModifier::ActivityFilterFill, NULL);
    }
    else
    {
        CAMSEGObjectModifier::fillComboBox(cboStatusFilter, NULL, CAMSEGObjectModifier::DeliveryStatusFilterFill, NULL);
    }

    btnClose->setIcon(QIcon(PATH_ICONS + "close.png"));
    btnClose->setAutoRaise(true);

    filterWidget->setHidden(true);

    QHBoxLayout* layoutFilter = new QHBoxLayout(filterWidget);
    layoutFilter->addWidget(new QLabel(tr("Filter :")));
    layoutFilter->addWidget(txtNameFilter);
    layoutFilter->addStretch(2);
    layoutFilter->addWidget(new QLabel(tr("Status :")));
    layoutFilter->addWidget(cboStatusFilter);
    layoutFilter->addStretch(20);
    layoutFilter->addWidget(btnClose);

    //Right Layout
    QVBoxLayout* rightWidgetLayout = new QVBoxLayout(rightWidget);
    rightWidgetLayout->addWidget(m_table, 10);
    rightWidgetLayout->addWidget(filterWidget);

    connect(m_table, SIGNAL(currentItemChanged()), this, SIGNAL(rowChanged()));
    connect(m_table, SIGNAL(doubleClicked(QModelIndex)), this, SIGNAL(rowDoubleClicked(QModelIndex)));

    connect(m_catTree, SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)), this, SLOT(reloadMainTable()));
    connect(txtNameFilter, SIGNAL(textChanged(QString)), this, SLOT(updateNameFilter()));
    connect(cboStatusFilter, SIGNAL(currentIndexChanged(int)), this, SLOT(updateActivityFilter()));

    connect(btnClose, SIGNAL(clicked()), this, SLOT(toggleFilterWidget()));
}

CAMSEGTable::~CAMSEGTable()
{
    saveWidths();
}

int CAMSEGTable::getCurrentInternalId() const
{
    if (m_table->currentIndex().row() >= 0)
    {
        return m_model->internalIdForRow(m_table->currentIndex().row());
    }

    return -1;
}

void CAMSEGTable::setCurrentInternalId(const quint32 p_id)
{
    for (int i = 0; i < m_model->rowCount(); ++i)
    {
        if (m_model->internalIdForRow(i) == p_id)
        {
            m_table->setCurrentIndex(m_model->index(i, 0));
        }
    }
}

void CAMSEGTable::setActivityFilter(const ActivityFilter p_filter)
{
    if (m_type != Type_Order && m_type != Type_Delivery)
    {
        m_model->setActivityFilter(p_filter);
    }
}

void CAMSEGTable::setDeliveryStatusFilter(const DeliveryStatusFilter p_filter)
{
    if (m_type == Type_Order || m_type == Type_Delivery)
    {
        m_model->setDeliveryFilter(p_filter);
    }
}

void CAMSEGTable::setNameFilter(const QString & p_filter)
{
    m_model->setStringFilter(p_filter);
}

void CAMSEGTable::reloadTable()
{
    int currentRow = m_catTree->currentItem() != NULL ? m_catTree->currentItem()->data(0, Qt::UserRole).toInt() : -1;
    QString currentText = m_catTree->currentItem() != NULL ? m_catTree->currentItem()->text(0) : "";

    disconnect(m_catTree, SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)), this, SLOT(reloadMainTable()));

    fillCats();

    if (currentRow >= 0)
    {
        QList<QTreeWidgetItem*> items = m_catTree->findItems(currentText, Qt::MatchFixedString | Qt::MatchRecursive);

        if (items.count() == 1)
        {
            m_catTree->setCurrentItem(items[0]);
        }
        else
        {
            for (int i = 0; i < items.count(); ++i)
            {
                if (items[i]->data(0, Qt::UserRole).toInt() == currentRow)
                {
                    m_catTree->setCurrentItem(items[i]);
                    break;
                }
            }
        }
    }

    connect(m_catTree, SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)), this, SLOT(reloadMainTable()));

    reloadMainTable();
}

void CAMSEGTable::reloadMainTable()
{
    int idCat = -1;
    if (m_catTree->currentItem() != NULL)
        idCat = m_catTree->currentItem()->data(0, Qt::UserRole).toInt();

    m_model->setCategory(idCat);
}

void CAMSEGTable::toggleFilterWidget()
{
    filterWidget->setHidden( ! filterWidget->isHidden());

    if ( ! filterWidget->isHidden())
    {
        txtNameFilter->setFocus();
    }
    else
    {
        txtNameFilter->setText("");
        cboStatusFilter->setCurrentIndex(0);
    }
}

void CAMSEGTable::updateActivityFilter()
{
    if (m_type != Type_Order && m_type != Type_Delivery)
    {
        ActivityFilter newFilter = All;

        if (cboStatusFilter->currentIndex() == 1)
        {
            newFilter = ActiveOnly;
        }
        else if (cboStatusFilter->currentIndex() == 2)
        {
            newFilter = InactiveOnly;
        }

        setActivityFilter(newFilter);
    }
    else
    {
        DeliveryStatusFilter newFilter = AllStatus;

        if (cboStatusFilter->currentIndex() == 1)
        {
            newFilter = DeliveredOnly;
        }
        else if (cboStatusFilter->currentIndex() == 2)
        {
            newFilter = NonDeliveredOnly;
        }

        setDeliveryStatusFilter(newFilter);
    }
}

void CAMSEGTable::updateNameFilter()
{
    setNameFilter(txtNameFilter->text());
}

void CAMSEGTable::keyReleaseEvent(QKeyEvent * ev)
{
    if (ev->key() == Qt::Key_Delete)
    {
        emit deleteCurrentItem();
    }
    else if (ev->key() == Qt::Key_Return)
    {
        emit modifyCurrentItem();
    }
    else
    {
        ev->ignore();
    }
}

void CAMSEGTable::fillCats()
{
    m_catTree->clear();
    m_listCats.clear();
    QString nameRoot = "";

    switch (m_type)
    {
        case Type_Customer:
            nameRoot = tr("All Customers");
            break;
        case Type_Vendor:
            nameRoot = tr("All Vendors");
            break;
        case Type_Product:
            nameRoot = tr("All Products");
            break;
        case Type_PLU:
            nameRoot = tr("All PLUs");
            break;
        default:
            return;
    }

    m_catTree->clear();

    QTreeWidgetItem* rootItem = new QTreeWidgetItem(m_catTree);
    rootItem->setText(0, nameRoot);
    rootItem->setIcon(0, QIcon(PATH_ICONS + "folder.png"));
    rootItem->setData(0, Qt::UserRole, -1);

    switch (m_type)
    {
        case Type_Customer:
            for (quint32 i = 0; i < m_campaign->getNumberOfCustomerCats(); ++i)
            {
                CustomerCat* cat = m_campaign->getCustomerCat(i);

                if ( ! cat->hasParentCat())
                {
                    QTreeWidgetItem* subItem = new QTreeWidgetItem(rootItem);
                    subItem->setText(0, cat->getName());
                    subItem->setIcon(0, QIcon(PATH_ICONS + "folder.png"));
                    subItem->setData(0, Qt::UserRole, cat->getId());
                    fillCustomerCats(subItem, cat);
                }
            }
            break;
        case Type_Vendor:
            for (quint32 i = 0; i < m_campaign->getNumberOfVendorCats(); ++i)
            {
                VendorCat* cat = m_campaign->getVendorCat(i);

                if ( ! cat->hasParentCat())
                {
                    QTreeWidgetItem* subItem = new QTreeWidgetItem(rootItem);
                    subItem->setText(0, cat->getName());
                    subItem->setIcon(0, QIcon(PATH_ICONS + "folder.png"));
                    subItem->setData(0, Qt::UserRole, cat->getId());
                    fillVendorCats(subItem, cat);
                }
            }
            break;
        case Type_Product:
            for (quint32 i = 0; i < m_campaign->getNumberOfProductCats(); ++i)
            {
                ProductCat* cat = m_campaign->getProductCat(i);

                if ( ! cat->hasParentCat())
                {
                    QTreeWidgetItem* subItem = new QTreeWidgetItem(rootItem);
                    subItem->setText(0, cat->getName());
                    subItem->setIcon(0, QIcon(PATH_ICONS + "folder.png"));
                    subItem->setData(0, Qt::UserRole, cat->getId());
                    fillProductCats(subItem, cat);
                }
            }
            break;
        case Type_PLU:
            for (quint32 i = 0; i < m_campaign->getNumberOfPLUCats(); ++i)
            {
                PLUCat* cat = m_campaign->getPLUCat(i);

                if ( ! cat->hasParentCat())
                {
                    QTreeWidgetItem* subItem = new QTreeWidgetItem(rootItem);
                    subItem->setText(0, cat->getName());
                    subItem->setIcon(0, QIcon(PATH_ICONS + "folder.png"));
                    subItem->setData(0, Qt::UserRole, cat->getId());
                    fillPLUCats(subItem, cat);
                }
            }
            break;
        default:
            return;
    }

    m_catTree->expandAll();
}

void CAMSEGTable::setupBaseTable()
{
    QSettings settings;
    QList<int> sizes;

    if (settings.value("General/CatChooser").toBool() == false)
        m_catTree->setHidden(true);

    settings.beginGroup("CTableSizes");
    settings.beginGroup(QString::number(objectTypeToInt(m_type)));

    sizes << settings.value("0", 100).toInt();
    sizes << settings.value("1", 300).toInt();

    settings.endGroup();
    settings.endGroup();

    settings.beginGroup("TableColumns");
    settings.beginGroup(QString::number(objectTypeToInt(m_type)));

    switch (m_type)
    {
        case Type_Customer:

            m_widths << settings.value("0", 45).toInt();
            m_widths << settings.value("1", 130).toInt();
            m_widths << settings.value("2", 130).toInt();
            m_widths << settings.value("3", 130).toInt();
            m_widths << settings.value("4", 70).toInt();
            m_widths << settings.value("5", 85).toInt();

            m_table->verticalHeader()->setClickable(true);

            addWidget(m_catTree);
            addWidget(rightWidget);
            setSizes(sizes);

            break;

        case Type_Vendor:
            m_widths << settings.value("0", 45).toInt();
            m_widths << settings.value("1", 130).toInt();
            m_widths << settings.value("2", 130).toInt();
            m_widths << settings.value("3", 130).toInt();
            m_widths << settings.value("4", 70).toInt();
            m_widths << settings.value("5", 85).toInt();

            addWidget(m_catTree);
            addWidget(rightWidget);
            setSizes(sizes);

            break;

        case Type_Product:
            m_widths << settings.value("0", 50).toInt();
            m_widths << settings.value("1", 50).toInt();
            m_widths << settings.value("2", 130).toInt();
            m_widths << settings.value("3", 130).toInt();
            m_widths << settings.value("4", 130).toInt();
            m_widths << settings.value("5", 85).toInt();
            m_widths << settings.value("6", 85).toInt();

            addWidget(m_catTree);
            addWidget(rightWidget);
            setSizes(sizes);

            break;

        case Type_PLU:
            m_widths << settings.value("0", 45).toInt();
            m_widths << settings.value("1", 130).toInt();
            m_widths << settings.value("2", 130).toInt();
            m_widths << settings.value("3", 85).toInt();
            m_widths << settings.value("4", 85).toInt();
            m_widths << settings.value("5", 85).toInt();

            addWidget(m_catTree);
            addWidget(rightWidget);
            setSizes(sizes);

            break;

        case Type_CustomerCat:
            m_widths << settings.value("0", 30).toInt();
            m_widths << settings.value("1", 130).toInt();
            m_widths << settings.value("2", 85).toInt();

            addWidget(rightWidget);

            break;

        case Type_VendorCat:
            m_widths << settings.value("0", 30).toInt();
            m_widths << settings.value("1", 130).toInt();
            m_widths << settings.value("2", 85).toInt();

            addWidget(rightWidget);

            break;

        case Type_ProductCat:
            m_widths << settings.value("0", 30).toInt();
            m_widths << settings.value("1", 130).toInt();
            m_widths << settings.value("2", 85).toInt();

            addWidget(rightWidget);

            break;

        case Type_PLUCat:
            m_widths << settings.value("0", 30).toInt();
            m_widths << settings.value("1", 130).toInt();
            m_widths << settings.value("2", 85).toInt();

            addWidget(rightWidget);

            break;

        case Type_Discount:
            m_widths << settings.value("0", 50).toInt();
            m_widths << settings.value("1", 140).toInt();
            m_widths << settings.value("2", 120).toInt();
            m_widths << settings.value("3", 100).toInt();
            m_widths << settings.value("4", 100).toInt();

            addWidget(rightWidget);

            break;

        case Type_Unit:
            m_widths << settings.value("0", 30).toInt();
            m_widths << settings.value("1", 130).toInt();
            m_widths << settings.value("2", 85).toInt();

            addWidget(rightWidget);

            break;

        case Type_Order:
            m_widths << settings.value("0", 30).toInt();
            m_widths << settings.value("1", 140).toInt();
            m_widths << settings.value("2", 140).toInt();
            m_widths << settings.value("3", 85).toInt();
            m_widths << settings.value("4", 85).toInt();
            m_widths << settings.value("5", 85).toInt();
            m_widths << settings.value("6", 95).toInt();
            m_widths << settings.value("7", 95).toInt(); //***

            addWidget(rightWidget);

            break;

        case Type_Payment:        
            m_widths << settings.value("0", 55).toInt();
            m_widths << settings.value("1", 130).toInt();
            m_widths << settings.value("2", 90).toInt();
            m_widths << settings.value("3", 90).toInt();
            m_widths << settings.value("4", 90).toInt();

            addWidget(rightWidget);
            break;        

        case Type_PaymentMode:
            m_widths << settings.value("0", 30).toInt();
            m_widths << settings.value("1", 130).toInt();
            m_widths << settings.value("2", 85).toInt();

            addWidget(rightWidget);
            break;

        case Type_Supplier:
            m_widths << settings.value("0", 30).toInt();
            m_widths << settings.value("1", 130).toInt();
            m_widths << settings.value("2", 110).toInt();
            m_widths << settings.value("3", 90).toInt();
            m_widths << settings.value("4", 85).toInt();

            addWidget(rightWidget);
            break;

        case Type_Delivery:
            m_widths << settings.value("0", 30).toInt();
            m_widths << settings.value("1", 130).toInt();
            m_widths << settings.value("2", 85).toInt();
            m_widths << settings.value("3", 85).toInt();
            m_widths << settings.value("4", 90).toInt();
            m_widths << settings.value("5", 85).toInt();

            addWidget(rightWidget);
            break;

        case Type_Tax:
            m_widths << settings.value("0", 30).toInt();
            m_widths << settings.value("1", 130).toInt();
            m_widths << settings.value("2", 85).toInt();
            m_widths << settings.value("3", 90).toInt();

            addWidget(rightWidget);
            break;

        case Type_User:
            m_widths << settings.value("0", 30).toInt();
            m_widths << settings.value("1", 130).toInt();
            m_widths << settings.value("2", 85).toInt();

            addWidget(rightWidget);
            break;

        case Type_StockAdj:
            m_widths << settings.value("0", 30).toInt();
            m_widths << settings.value("1", 90).toInt();
            m_widths << settings.value("2", 120).toInt();
            m_widths << settings.value("3", 85).toInt();

            addWidget(rightWidget);
            break;

        case Type_CashAdj:
            m_widths << settings.value("0", 30).toInt();
            m_widths << settings.value("1", 90).toInt();
            m_widths << settings.value("2", 85).toInt();

            addWidget(rightWidget);
            break;

        case Type_Expense:
            m_widths << settings.value("0", 30).toInt();
            m_widths << settings.value("1", 55).toInt();
            m_widths << settings.value("2", 70).toInt();
            m_widths << settings.value("3", 55).toInt();

            addWidget(rightWidget);
            break;

        case Type_Donator:
            m_widths << settings.value("0", 30).toInt();
            m_widths << settings.value("1", 90).toInt();
            m_widths << settings.value("2", 90).toInt();
            m_widths << settings.value("3", 85).toInt();

            addWidget(rightWidget);
            break;

        case Type_Donation:
            m_widths << settings.value("0", 30).toInt();
            m_widths << settings.value("1", 75).toInt();
            m_widths << settings.value("2", 100).toInt();
            m_widths << settings.value("3", 85).toInt();
            m_widths << settings.value("3", 75).toInt();

            addWidget(rightWidget);
            break;

        case Type_Service:
            m_widths << settings.value("0", 50).toInt();
            m_widths << settings.value("1", 130).toInt();
            m_widths << settings.value("2", 85).toInt();
            m_widths << settings.value("3", 85).toInt();
            m_widths << settings.value("4", 85).toInt();

            addWidget(rightWidget);
            break;

        default:
            break;

    }

    settings.endGroup();
    settings.endGroup();

    settings.beginGroup("TableSorting");
    settings.beginGroup(QString::number(objectTypeToInt(m_type)));

    m_table->horizontalHeader()->setSortIndicator(settings.value("Current").toInt(), (Qt::SortOrder) settings.value("Order").toInt());

    settings.endGroup();
    settings.endGroup();

    for (int i = 0; i < m_model->columnCount(); ++i)
    {
        m_table->setColumnWidth(i, m_widths[i]);
    }

    //Visible Columns
    settings.beginGroup("TableColumnsVisible");
    settings.beginGroup(QString::number(objectTypeToInt(m_type)));

    for (int i = 0; i < m_model->columnCount(); ++i)
    {
        m_table->setColumnHidden(i, settings.value(QString::number(i), false).toBool());
    }

    settings.endGroup();
    settings.endGroup();

    m_table->setupHeaderMenu();
}

void CAMSEGTable::saveWidths()
{
    QSettings settings;
    settings.beginGroup("TableColumns");
    settings.beginGroup(QString::number(objectTypeToInt(m_type)));

    for (int i = 0; i < m_model->columnCount(); ++i)
    {
        if (m_table->columnWidth(i) != 0)
            settings.setValue(QString::number(i), m_table->columnWidth(i));
        else
            settings.setValue(QString::number(i), 100);
    }

    settings.endGroup();
    settings.endGroup();


    //Table Sizes
    settings.beginGroup("CTableSizes");
    settings.beginGroup(QString::number(objectTypeToInt(m_type)));

    for (int i = 0; i < sizes().count(); ++i)
    {
        settings.setValue(QString::number(i), sizes().at(i));
    }

    settings.endGroup();
    settings.endGroup();


    //Table Sorting
    settings.beginGroup("TableSorting");
    settings.beginGroup(QString::number(objectTypeToInt(m_type)));

    settings.setValue("Current", m_table->horizontalHeader()->sortIndicatorSection());
    settings.setValue("Order", (int) m_table->horizontalHeader()->sortIndicatorOrder());

    settings.endGroup();
    settings.endGroup();


    //Visible Columns
    settings.beginGroup("TableColumnsVisible");
    settings.beginGroup(QString::number(objectTypeToInt(m_type)));

    for (int i = 0; i < m_model->columnCount(); ++i)
    {
        settings.setValue(QString::number(i), m_table->isColumnHidden(i));
    }

    settings.endGroup();
    settings.endGroup();

    settings.sync();
}

void CAMSEGTable::fillCustomerCats(QTreeWidgetItem* item, const CAMSEG::CustomerCat* p_baseCat)
{
    QList<CustomerCat*> listCats = p_baseCat->getDirectChilds();

    foreach (CustomerCat* cat, listCats)
    {
        QTreeWidgetItem* subItem = new QTreeWidgetItem(item);
        subItem->setText(0, cat->getName());
        subItem->setIcon(0, QIcon(PATH_ICONS + "folder.png"));
        subItem->setData(0, Qt::UserRole, cat->getId());
        fillCustomerCats(subItem, cat);
    }
}

void CAMSEGTable::fillVendorCats(QTreeWidgetItem* item, const CAMSEG::VendorCat* p_baseCat)
{
    QList<VendorCat*> listCats = p_baseCat->getDirectChilds();

    foreach (VendorCat* cat, listCats)
    {
        QTreeWidgetItem* subItem = new QTreeWidgetItem(item);
        subItem->setText(0, cat->getName());
        subItem->setIcon(0, QIcon(PATH_ICONS + "folder.png"));
        subItem->setData(0, Qt::UserRole, cat->getId());
        fillVendorCats(subItem, cat);
    }
}

void CAMSEGTable::fillProductCats(QTreeWidgetItem* item, const CAMSEG::ProductCat* p_baseCat)
{
    QList<ProductCat*> listCats = p_baseCat->getDirectChilds();

    foreach (ProductCat* cat, listCats)
    {
        QTreeWidgetItem* subItem = new QTreeWidgetItem(item);
        subItem->setText(0, cat->getName());
        subItem->setIcon(0, QIcon(PATH_ICONS + "folder.png"));
        subItem->setData(0, Qt::UserRole, cat->getId());
        fillProductCats(subItem, cat);
    }
}

void CAMSEGTable::fillPLUCats(QTreeWidgetItem* item, const CAMSEG::PLUCat* p_baseCat)
{
    QList<PLUCat*> listCats = p_baseCat->getDirectChilds();

    foreach (PLUCat* cat, listCats)
    {
        QTreeWidgetItem* subItem = new QTreeWidgetItem(item);
        subItem->setText(0, cat->getName());
        subItem->setIcon(0, QIcon(PATH_ICONS + "folder.png"));
        subItem->setData(0, Qt::UserRole, cat->getId());
        fillPLUCats(subItem, cat);
    }
}

//CAMSEGTableItem

CAMSEGTableItem::CAMSEGTableItem()
{
    setText("");
}

CAMSEGTableItem::CAMSEGTableItem(const QString & text, const Amount p_value)
{
    setText(text, p_value);
}

void CAMSEGTableItem::setText(const QString &text)
{
    setText(text, NO_VALUE);
}

void CAMSEGTableItem::setText(const QString &text, const Amount p_value)
{
    setData(text, Qt::DisplayRole);

    bool ok;
    double value = text.toDouble(&ok);

    if (p_value != NO_VALUE)
    {
        setData(p_value.toDouble(), Qt::UserRole);
    }
    else if (ok)
    {
        setData(value, Qt::UserRole);
    }
    else
    {
        setData(text, Qt::UserRole);
    }
}

CAMSEGTableView::CAMSEGTableView() : QTableView()
{
    setHorizontalHeader(new CAMSEGTableHeader(Qt::Horizontal, this));
}

void CAMSEGTableView::setupHeaderMenu()
{
    dynamic_cast<CAMSEGTableHeader*>(horizontalHeader())->setupMenu();
}

void CAMSEGTableView::currentChanged(const QModelIndex &current, const QModelIndex &previous)
{
    QTableView::currentChanged(current, previous);

    if (current != previous)
        emit currentItemChanged();
}

//CAMSEGTableHeader

CAMSEGTableHeader::CAMSEGTableHeader(Qt::Orientation p_orientation, CAMSEGTableView* p_table) :
        QHeaderView(p_orientation, p_table), m_table(p_table)
{
    m_contextMenu = new QMenu(tr("Columns"), this);
    m_signalMapper = new QSignalMapper(this);
    setClickable(true);
}

void CAMSEGTableHeader::setupMenu()
{
    for (int i = 0; i < count(); ++i)
    {
        QAction* action = m_contextMenu->addAction(model()->headerData(i, orientation(), Qt::DisplayRole).toString());
        action->setCheckable(true);
        action->setChecked( ! m_table->isColumnHidden(i));

        connect(action, SIGNAL(toggled(bool)), m_signalMapper, SLOT(map()));
        m_signalMapper->setMapping(action, i);

        m_actions.append(action);
    }

    connect(m_signalMapper, SIGNAL(mapped(int)), this, SLOT(actionClicked(int)));
}

void CAMSEGTableHeader::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::RightButton)
    {
        m_contextMenu->exec(QCursor::pos());
    }
    else
    {
        this->QHeaderView::mousePressEvent(event);
    }
}

void CAMSEGTableHeader::actionClicked(int p_id)
{
    m_table->setColumnHidden(p_id,  ! m_actions[p_id]->isChecked());
}

