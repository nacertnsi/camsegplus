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

#include "centralwidget.h"
#include "frmmanagecustomers.h"
#include "frmmanagevendors.h"
#include "frmmanageplus.h"
#include "frmmanageorders.h"
#include "frmmanagedeliveries.h"
#include "frmmanagepayments.h"
#include "frmmanagesuppliers.h"
#include "frmoverview.h"
#include "frmmanagediscounts.h"
#include "frmmanagedonators.h"
#include "frmmanageexpenses.h"
#include "frmmanageservices.h"
#include "frmcashadjustments.h"
#include "frmmanagepay.h"
#include "frmmanageunits.h"
#include "frmmanagetaxes.h"
#include "frmmanagedonations.h"
#include "frmposlayoutconfig.h"
#include "frmmanageusers.h"
#include "frmmanagecwi.h"
#include "frminventory.h"
#include "frmreportviewer.h"
#include "frmcharts.h"
#include "global.h"

using namespace CAMSEG;

centralView::centralView(Campaign* p_campaign) : m_campaign(p_campaign)
{
    QFont frmFont(this->font());
    frmFont.setPointSize(Font_Size);
    this->setFont(frmFont);

    setupUi(this);
    loadUI();

    //Setup campaign signals
    connect(m_campaign, SIGNAL(objectAdded(CAMSEG::ObjectType)), this, SLOT(campaignDataAddedRemoved(CAMSEG::ObjectType)));
    connect(m_campaign, SIGNAL(objectModified(CAMSEG::ObjectType)), this, SLOT(campaignDataModified(CAMSEG::ObjectType)));
    connect(m_campaign, SIGNAL(objectRemoved(CAMSEG::ObjectType)), this, SLOT(campaignDataAddedRemoved(CAMSEG::ObjectType)));
    connect(m_campaign, SIGNAL(currencyChanged()), this, SLOT(reloadAllPages()));
    connect(m_campaign, SIGNAL(customIDsChanged()), this, SLOT(reloadAllPages()));

   this->showOperationsWindow(0);
}

centralView::~centralView()
{
}

void centralView::loadUI()
{
    //Create the donator manager
    donatorManager = new frmManageDonators(m_campaign, this);
    donatorManager->readSettings();

    //Create the service manager
    serviceManager = new frmManageServices(m_campaign, this);
    serviceManager->readSettings();

    //Other tabs

    tabReports = new frmReportViewer(m_campaign);
    tabCharts = new frmCharts(m_campaign);

    tabWidget->removeTab(0);
    tabWidget->removeTab(0);

    tabWidget->addTab(tabFiles, QIcon(PATH_ICONS + "files.png"), tr("Files"));
    tabWidget->addTab(tabOperations, QIcon(PATH_ICONS + "sales.png"), tr("Operations"));
    tabWidget->addTab(tabReports, QIcon(PATH_ICONS + "invoice.png"), tr("Reports"));
    tabWidget->addTab(tabCharts, QIcon(PATH_ICONS + "chart.png"), tr("Charts"));

    updateTabPosition();

    //----------------------Files Tab----------------------

    //Widgets
    customers = new frmManageCustomers(m_campaign);
    vendors = new frmManageVendors(m_campaign);
    plus = new frmManagePLUs(m_campaign);
    suppliers = new frmManageSuppliers(m_campaign);
    discounts = new frmManageDiscounts(m_campaign);

    stackFiles = new QStackedWidget();
    stackFiles->addWidget(customers);
    stackFiles->addWidget(vendors);
    stackFiles->addWidget(plus);
    stackFiles->addWidget(suppliers);
    stackFiles->addWidget(discounts);
    cwimanager = NULL;

    //List
    listFiles = new QListWidget();
    listFiles->setViewMode(QListView::IconMode);
    listFiles->setIconSize(QSize(70, 70));
    listFiles->setMovement(QListView::Static);
    listFiles->setMaximumWidth(100);
    listFiles->setSpacing(8);
    listFiles->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    listFiles->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    QPalette palette = listFiles->palette();
    palette.setColor(QPalette::Background, palette.color(QPalette::Button));
    listFiles->setPalette(palette);

    setupFilesIcons();
    listFiles->setCurrentRow(0);

    //----------------------Operations Tab----------------------

    //Widgets
    orders = new frmManageOrders(m_campaign);
    deliveries = new frmManageDeliveries(m_campaign);
    payments = new frmManagePayments(m_campaign);
    overview = new frmOverview(m_campaign);
    inventory = new frmInventory(m_campaign);

    stackOperations = new QStackedWidget();
    stackOperations->addWidget(overview);
    stackOperations->addWidget(orders);
    stackOperations->addWidget(deliveries);
    stackOperations->addWidget(payments);
    stackOperations->addWidget(inventory);

    //List
    listOperations = new QListWidget();
    listOperations->setViewMode(QListView::IconMode);
    listOperations->setIconSize(QSize(70, 70));
    listOperations->setMovement(QListView::Static);
    listOperations->setMaximumWidth(100);
    listOperations->setSpacing(8);
    listOperations->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    listOperations->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    setupOperationsIcons();
    listOperations->setCurrentRow(0);

    setCWIManagerState(m_campaign->isCWIEnabled());

    //----------------------Layouts----------------------

    QHBoxLayout* layoutFiles = new QHBoxLayout(tabFiles);
    layoutFiles->addWidget(listFiles);
    layoutFiles->addWidget(stackFiles);

    QHBoxLayout* layoutOperations = new QHBoxLayout(tabOperations);
    layoutOperations->addWidget(listOperations);
    layoutOperations->addWidget(stackOperations);
}

void centralView::setupFilesIcons()
{
    QListWidgetItem *LmanageClients = new QListWidgetItem(listFiles);
    LmanageClients->setIcon(QIcon(PATH_ICONS + "clients.png"));
    LmanageClients->setText(tr("Customers"));
    LmanageClients->setTextAlignment(Qt::AlignHCenter);
    LmanageClients->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

    QListWidgetItem *LmanageVendeurs = new QListWidgetItem(listFiles);
    LmanageVendeurs->setIcon(QIcon(PATH_ICONS + "manage_vendor.png"));
    LmanageVendeurs->setText(tr("Vendors"));
    LmanageVendeurs->setTextAlignment(Qt::AlignHCenter);
    LmanageVendeurs->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

    QListWidgetItem *LmanageProduits = new QListWidgetItem(listFiles);
    LmanageProduits->setIcon(QIcon(PATH_ICONS + "plu.png"));
    LmanageProduits->setText(tr("PLUs"));
    LmanageProduits->setTextAlignment(Qt::AlignHCenter);
    LmanageProduits->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

    QListWidgetItem *LmanageSuppliers = new QListWidgetItem(listFiles);
    LmanageSuppliers->setIcon(QIcon(PATH_ICONS + "supplier.png"));
    LmanageSuppliers->setText(tr("Suppliers"));
    LmanageSuppliers->setTextAlignment(Qt::AlignHCenter);
    LmanageSuppliers->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

    QListWidgetItem *LmanageDiscounts = new QListWidgetItem(listFiles);
    LmanageDiscounts->setIcon(QIcon(PATH_ICONS + "discount.png"));
    LmanageDiscounts->setText(tr("Discounts"));
    LmanageDiscounts->setTextAlignment(Qt::AlignHCenter);
    LmanageDiscounts->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

    LmanageCWI = new QListWidgetItem(listFiles);
    LmanageCWI->setIcon(QIcon(PATH_ICONS + "cwi.png"));
    LmanageCWI->setText(tr("CWI Manager"));
    LmanageCWI->setTextAlignment(Qt::AlignHCenter);
    if (m_campaign->isCWIEnabled())
    {
        LmanageCWI->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    }
    else
    {
        LmanageCWI->setFlags(Qt::ItemIsSelectable);
    }

    connect(listFiles, SIGNAL(currentItemChanged(QListWidgetItem *, QListWidgetItem *)), this, SLOT(changeFilesPage(QListWidgetItem *, QListWidgetItem*)));
}

void centralView::setupOperationsIcons()
{
    QListWidgetItem *Loverview = new QListWidgetItem(listOperations);
    Loverview->setIcon(QIcon(PATH_ICONS + "sales70.png"));
    Loverview->setText(tr("Overview"));
    Loverview->setTextAlignment(Qt::AlignHCenter);
    Loverview->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

    QListWidgetItem *LmanageOrders = new QListWidgetItem(listOperations);
    LmanageOrders->setIcon(QIcon(PATH_ICONS + "order70.png"));
    LmanageOrders->setText(tr("Orders"));
    LmanageOrders->setTextAlignment(Qt::AlignHCenter);
    LmanageOrders->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

    QListWidgetItem *LmanageDeliveries = new QListWidgetItem(listOperations);
    LmanageDeliveries->setIcon(QIcon(PATH_ICONS + "delivery70.png"));
    LmanageDeliveries->setText(tr("Deliveries"));
    LmanageDeliveries->setTextAlignment(Qt::AlignHCenter);
    LmanageDeliveries->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

    QListWidgetItem *LmanagePayments = new QListWidgetItem(listOperations);
    LmanagePayments->setIcon(QIcon(PATH_ICONS + "money70.png"));
    LmanagePayments->setText(tr("Payments"));
    LmanagePayments->setTextAlignment(Qt::AlignHCenter);
    LmanagePayments->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

    QListWidgetItem *Linventory = new QListWidgetItem(listOperations);
    Linventory->setIcon(QIcon(PATH_ICONS + "inventory70.png"));
    Linventory->setText(tr("Inventory"));
    Linventory->setTextAlignment(Qt::AlignHCenter);
    Linventory->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

    connect(listOperations, SIGNAL(currentItemChanged(QListWidgetItem *, QListWidgetItem *)), this, SLOT(changeOperationsPage(QListWidgetItem *, QListWidgetItem*)));
}

void centralView::showFilesWindow(unsigned int p_row)
{
    tabWidget->setCurrentIndex(0);
    listFiles->setCurrentRow(p_row);
}

void centralView::showOperationsWindow(unsigned int p_row)
{
    tabWidget->setCurrentIndex(1);
    listOperations->setCurrentRow(p_row);
}

void centralView::showReport(const QString & p_path)
{
    tabWidget->setCurrentIndex(2);
    tabReports->viewReport(p_path);
}

void centralView::showChart(const QString & p_path)
{
    tabWidget->setCurrentIndex(3);
    tabCharts->viewChart(p_path);
}

void centralView::showReports()
{
    tabWidget->setCurrentIndex(2);
}

void centralView::showCharts()
{
    tabWidget->setCurrentIndex(3);
}

void centralView::showDonators()
{
    //Check the permissions
    if ( ! checkPermissions(this, m_campaign, User_Normal, Perm_ManageDonators))
        return;

    donatorManager->show();
    donatorManager->raise();
    donatorManager->activateWindow();
}

void centralView::showServices()
{
    //Check the permissions
    if ( ! checkPermissions(this, m_campaign, User_Normal, Perm_ManageServices))
        return;

    serviceManager->show();
    serviceManager->raise();
    serviceManager->activateWindow();
}

void centralView::showCashAdjustments()
{
    //Check the permissions
    if ( ! checkPermissions(this, m_campaign, User_Normal, Perm_ManageCashAdjustments))
        return;

    frmCashAdjustments* adjustments = new frmCashAdjustments(m_campaign, this);
    adjustments->exec();
    delete adjustments;
}

void centralView::showStockAdjustments()
{
    inventory->manageAdjustments();
}

void centralView::showVendorPay()
{
    //Check the permissions
    if ( ! checkPermissions(this, m_campaign, User_Normal, Perm_ManageVendorPay))
        return;

    frmManagePay* managePay = new frmManagePay(m_campaign, this);
    managePay->exec();
    delete managePay;
}

void centralView::showUnits()
{
    //Check the permissions
    if ( ! checkPermissions(this, m_campaign, User_Admin))
        return;

    frmManageUnits* manageUnits = new frmManageUnits(m_campaign, this);
    manageUnits->exec();
    delete manageUnits;
}

void centralView::showTaxes()
{
    //Check the permissions
    if ( ! checkPermissions(this, m_campaign, User_Normal, Perm_ManageTaxes))
        return;

    frmManageTaxes* manageTaxes = new frmManageTaxes(m_campaign, this);
    manageTaxes->exec();
    delete manageTaxes;
}

void centralView::showDonations()
{
    //Check the permissions
    if ( ! checkPermissions(this, m_campaign, User_Normal, Perm_ManageDonations))
        return;

    frmManageDonations* manageDonations = new frmManageDonations(m_campaign, this);
    manageDonations->exec();
    delete manageDonations;
}

void centralView::showExpenses()
{
    //Check the permissions
    if ( ! checkPermissions(this, m_campaign, User_Normal, Perm_ManageExpenses))
        return;

    frmManageExpenses* manageExpenses = new frmManageExpenses(m_campaign, this);
    manageExpenses->exec();
    delete manageExpenses;
}

void centralView::showProducts()
{
    plus->manageProducts();
}

void centralView::showPaymentModes()
{
    payments->manageModes();
}

void centralView::showPOSLayoutEditor()
{
    //Check the permissions
    if ( ! checkPermissions(this, m_campaign, User_Normal, Perm_EditPOSLayout))
        return;

    frmPOSLayoutConfig* posConfig = new frmPOSLayoutConfig(m_campaign, this);
    posConfig->readSettings();
    posConfig->exec();
    delete posConfig;
}

void centralView::showUsers()
{
    //Check the permissions
    if ( ! checkPermissions(this, m_campaign, User_Admin))
        return;

    frmManageUsers* users = new frmManageUsers(m_campaign, this);
    users->exec();
    delete users;
}

void centralView::setCWIManagerState(bool p_enabled)
{
    if (p_enabled)
    {
        if (cwimanager == NULL)
        {
            cwimanager = new frmManageCWI(m_campaign);
            stackFiles->addWidget(cwimanager);
        }
        LmanageCWI->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    }
    else
    {
        if (cwimanager != NULL)
        {
            stackFiles->removeWidget(cwimanager);
            delete cwimanager;
            cwimanager = NULL;
        }
        LmanageCWI->setFlags(Qt::ItemIsSelectable);
    }
}

void centralView::updateTabPosition()
{
    QSettings settings;
    int tabPosition = settings.value("General/TabPosition", 0).toInt();
    QTabWidget::TabPosition position;

    switch (tabPosition)
    {
        case 0:
            position = QTabWidget::North;
            break;
        case 1:
            position = QTabWidget::South;
            break;
        case 2:
            position = QTabWidget::West;
            break;
        case 3:
            position = QTabWidget::East;
            break;
        default:
            position = QTabWidget::North;
            break;
    }

    tabWidget->setTabPosition(position);
}

frmManageOrders* centralView::getManageOrders()
{
    return orders;
}

frmManageDeliveries* centralView::getManageDeliveries()
{
    return deliveries;
}

frmManagePLUs* centralView::getManagePLUs()
{
    return plus;
}

frmInventory* centralView::getInventory()
{
    return inventory;
}

frmManagePayments* centralView::getManagePayments()
{
    return payments;
}

void centralView::reloadAllPages()
{
    customers->reloadUI();
    vendors->reloadUI();
    plus->reloadUI();
    suppliers->reloadUI();
    discounts->reloadUI();

    orders->reloadUI();
    deliveries->reloadUI();
    payments->reloadUI();
    overview->reloadUI();
    inventory->reloadUI();
}

void centralView::campaignDataModified(const ObjectType p_type)
{
    switch (p_type)
    {
    case Type_Product:
        inventory->reloadUI();
        break;

    case Type_CustomerCat:
        customers->reloadUI();
        break;

    case Type_VendorCat:
        vendors->reloadUI();
        break;

    case Type_ProductCat:
        plus->reloadProducts();
        break;

    case Type_Order:
        overview->reloadUI();
        inventory->reloadUI();
        break;

    case Type_Payment:
        overview->reloadUI();
        break;

    case Type_Supplier:
        plus->reloadProducts();
        break;

    case Type_Tax:
        serviceManager->reloadUI();
        plus->reloadProducts();
        break;

    case Type_Delivery:
        overview->reloadUI();
        inventory->reloadUI();
        break;

    case Type_StockAdj:
        inventory->reloadUI();
        overview->reloadUI();
        break;

    case Type_CashAdj:
        overview->reloadUI();
        break;

    case Type_Expense:
        overview->reloadUI();
        break;

    case Type_PLUCat:
        plus->reloadUI();
        break;

    case Type_Unit:
        inventory->reloadUI();
        overview->reloadUI();
        plus->reloadUI();
        break;

    case Type_Service:
        overview->reloadUI();
        break;

    case Type_Donation:
        overview->reloadUI();
        break;

    default:
        break;
    }
}

void centralView::campaignDataAddedRemoved(const CAMSEG::ObjectType p_type)
{
    switch (p_type)
    {
    case Type_Customer:
        customers->reloadUI();
        payments->reloadUI();
        break;

    case Type_Vendor:
        vendors->reloadUI();
        break;

    case Type_Product:
        plus->reloadProducts();
        inventory->reloadUI();
        break;

    case Type_CustomerCat:
        customers->reloadUI();
        break;

    case Type_VendorCat:
        vendors->reloadUI();
        break;

    case Type_ProductCat:
        plus->reloadProducts();
        break;

    case Type_Order:
        orders->reloadUI();
        overview->reloadUI();
        inventory->reloadUI();
        break;

    case Type_Payment:
        payments->reloadUI();
        overview->reloadUI();
        break;

    case Type_Supplier:
        suppliers->reloadUI();
        plus->reloadProducts();
        break;

    case Type_Delivery:
        deliveries->reloadUI();
        overview->reloadUI();
        inventory->reloadUI();
        break;

    case Type_StockAdj:
        inventory->reloadUI();
        overview->reloadUI();
        break;

    case Type_CashAdj:
        overview->reloadUI();
        break;

    case Type_Expense:
        overview->reloadUI();
        break;

    case Type_PLU:
        plus->reloadUI();
        break;

    case Type_PLUCat:
        plus->reloadUI();
        break;

    case Type_Unit:
        plus->reloadUI();
        break;

    case Type_Discount:
        discounts->reloadUI();
        break;

    case Type_Service:
        serviceManager->reloadUI();
        break;

    case Type_Donation:
        overview->reloadUI();
        break;

    case Type_Donator:
        donatorManager->reloadUI();
        break;

    default:
        break;
    }
}

void centralView::changeFilesPage(QListWidgetItem *current, QListWidgetItem *previous)
{
    if ( ! current)
    {
        current = previous;
    }

    stackFiles->setCurrentIndex(listFiles->row(current));
}

void centralView::changeOperationsPage(QListWidgetItem *current, QListWidgetItem *previous)
{
    if ( ! current)
    {
        current = previous;
    }

    stackOperations->setCurrentIndex(listOperations->row(current));
}



