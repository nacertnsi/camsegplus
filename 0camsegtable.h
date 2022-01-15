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

#ifndef CAMSEGTABLE_H
#define CAMSEGTABLE_H

#include <QStandardItem>
#include <QTableView>
#include <QSplitter>
#include <QHeaderView>
#include <libCAMSEG/camseg.h>

#define NO_VALUE -10000000.887

namespace CAMSEG {
    class Campaign;
    class CustomerCat;
    class VendorCat;
    class ProductCat;
    class PLUCat;
}

class QTableView;
class QTreeWidget;
class QToolButton;
class QListWidget;
class QComboBox;
class QLineEdit;
class QMenu;
class QStandardItemModel;
class QTreeWidgetItem;
class QSignalMapper;
class CampaignModel;

class CAMSEGTableItem : public QStandardItem
{
    public:
        CAMSEGTableItem();
        CAMSEGTableItem(const QString & text, const CAMSEG::Amount p_value = NO_VALUE);

        void setText(const QString &text);
        void setText(const QString &text, const CAMSEG::Amount p_value);
};

class CAMSEGTableView : public QTableView
{
    Q_OBJECT

    public:
        CAMSEGTableView();
        void setupHeaderMenu();

    protected:
        void currentChanged(const QModelIndex & current, const QModelIndex & previous);

signals:
        void currentItemChanged();
};

class CAMSEGTableHeader : public QHeaderView
{
    Q_OBJECT

    public:
        CAMSEGTableHeader(Qt::Orientation p_orientation, CAMSEGTableView* p_table);

        void setupMenu();

    protected:
        void mousePressEvent(QMouseEvent* event);

    private slots:
        void actionClicked(int p_id);

    private:
        CAMSEGTableView *   m_table;
        QMenu*              m_contextMenu;
        QVector<QAction*>   m_actions;
        QSignalMapper*      m_signalMapper;
};

class CAMSEGTable : public QSplitter
{
    Q_OBJECT

    public:
                            CAMSEGTable(CAMSEG::ObjectType p_viewType, CAMSEG::Campaign* p_campaign, QWidget* parent = NULL);
                            ~CAMSEGTable();

        int                 getCurrentInternalId() const;
        void                setCurrentInternalId(const quint32 p_id);

        void                setActivityFilter(const CAMSEG::ActivityFilter p_filter);
        void                setDeliveryStatusFilter(const CAMSEG::DeliveryStatusFilter p_filter);
        void                setNameFilter(const QString & p_filter);

    public slots:
        void                reloadTable();
        void                reloadMainTable();
        void                toggleFilterWidget();

        void                updateActivityFilter();
        void                updateNameFilter();

    signals:
        void                rowChanged();
        void                rowDoubleClicked(QModelIndex p_index);
        void                modifyCurrentItem();
        void                deleteCurrentItem();

    protected:
        void                keyReleaseEvent(QKeyEvent *);

    private:
        void                fillCats();
        void                setupBaseTable();
        void                saveWidths();

        void                fillCustomerCats(QTreeWidgetItem* item, const CAMSEG::CustomerCat* p_baseCat);
        void                fillVendorCats(QTreeWidgetItem* item, const CAMSEG::VendorCat* p_baseCat);
        void                fillProductCats(QTreeWidgetItem* item, const CAMSEG::ProductCat* p_baseCat);
        void                fillPLUCats(QTreeWidgetItem* item, const CAMSEG::PLUCat* p_baseCat);

        CAMSEG::ObjectType  m_type;
        CAMSEG::Campaign*   m_campaign;

        QList<int>          m_listCats;

        QWidget*            rightWidget;
        CAMSEGTableView*    m_table;
        QTreeWidget*        m_catTree;

        QWidget*            filterWidget;
        QLineEdit*          txtNameFilter;
        QComboBox*          cboStatusFilter;
        QToolButton*        btnClose;

        QList<int>          m_widths;
        CampaignModel*      m_model;
};

#else
class CAMSEGTable;

#endif // CAMSEGTABLE_H
