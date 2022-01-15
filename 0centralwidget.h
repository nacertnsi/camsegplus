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

#ifndef CENTRALWIDGET_H
#define CENTRALWIDGET_H

#include <QtGui>
#include <QThread>
#include "ui_centralwidget.h"
#include <libCAMSEG/campaign.h>

class frmManageCustomers;
class frmManageVendors;
class frmManagePLUs;
class frmManageOrders;
class frmManageDeliveries;
class frmManagePayments;
class frmManageSuppliers;
class frmOverview;
class frmManageDiscounts;
class frmManageCWI;
class frmInventory;
class frmReportViewer;
class frmCharts;
class frmManageServices;
class frmManageDonators;
class UIReloader;

class centralView : public QWidget, Ui::centralView
{
    Q_OBJECT

    public:
        centralView(CAMSEG::Campaign* p_campaign);
        ~centralView();

        void showFilesWindow(unsigned int p_row);
        void showOperationsWindow(unsigned int p_row);

        void showReport(const QString & p_path);
        void showChart(const QString & p_path);

        void showReports();
        void showCharts();

        void showDonators();
        void showServices();
        void showCashAdjustments();
        void showStockAdjustments();
        void showVendorPay();
        void showUnits();
        void showTaxes();
        void showDonations();
        void showExpenses();
        void showProducts();
        void showPaymentModes();
        void showPOSLayoutEditor();
        void showUsers();

        void setCWIManagerState(bool p_enabled);
        void updateTabPosition();

        frmManageOrders* getManageOrders();
        frmManageDeliveries* getManageDeliveries();
        frmManagePLUs* getManagePLUs();
        frmInventory* getInventory();
        frmManagePayments* getManagePayments();

    public slots:
        void reloadAllPages();

        void changeFilesPage(QListWidgetItem *current, QListWidgetItem *previous);
        void changeOperationsPage(QListWidgetItem *current, QListWidgetItem *previous);

        void campaignDataModified(const CAMSEG::ObjectType p_type);
        void campaignDataAddedRemoved(const CAMSEG::ObjectType p_type);

    private:
        void loadUI();

        void setupFilesIcons();
        void setupOperationsIcons();

        CAMSEG::Campaign* m_campaign;

        QStackedWidget* stackFiles;
        QStackedWidget* stackOperations;

        QListWidget* listFiles;
        QListWidget* listOperations;

        QListWidgetItem *LmanageCWI;

        frmReportViewer* tabReports;
        frmCharts* tabCharts;

        frmManageCustomers* customers;
        frmManageVendors* vendors;
        frmManagePLUs* plus;
        frmManageSuppliers* suppliers;
        frmManageDiscounts* discounts;
        frmManageCWI* cwimanager;

        frmManageOrders* orders;
        frmManageDeliveries* deliveries;
        frmManagePayments* payments;
        frmOverview* overview;
        frmInventory* inventory;

        frmManageDonators* donatorManager;
        frmManageServices* serviceManager;
};

#else
class centralWidget;

#endif // CENTRALWIDGET_H
