#ifndef CAMPAIGNMODEL_H
#define CAMPAIGNMODEL_H

#include <QAbstractTableModel>
#include <libCAMSEG/campaign.h>

class CampaignModel;

class CampaignModelItem
{
public:
        CampaignModelItem() : m_object(NULL), m_model(NULL) {}
        CampaignModelItem(const CAMSEG::CampaignObject* p_object, const CampaignModel* p_model) :
                m_object(p_object), m_model(p_model) {}

        bool operator <(const CampaignModelItem& p_other) const;

        QVariant value(const int p_column, int role = Qt::DisplayRole) const;
        quint32 internalID() const;

    private:
        const CAMSEG::CampaignObject* m_object;
        const CampaignModel* m_model;
};

class CampaignModel : public QAbstractTableModel
{
    Q_OBJECT

    public:
        CampaignModel(const CAMSEG::ObjectType p_objectType, const CAMSEG::Campaign* p_campaign, QObject* parent = NULL);

        int rowCount(const QModelIndex &parent = QModelIndex()) const;
        int columnCount(const QModelIndex &parent = QModelIndex()) const;
        QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
        QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
        void sort(int p_column, Qt::SortOrder p_order = Qt::AscendingOrder);

        void reloadModel();

        quint32 internalIdForRow(const quint32 p_row);

        CAMSEG::ActivityFilter activityFilter() const { return m_activityFilter; }
        CAMSEG::DeliveryStatusFilter deliveryFilter() const { return m_deliveryFilter; }
        QString stringFilter() const { return m_stringFilter; }

    public slots:
        void setCategory(const int p_idCategory);
        void setActivityFilter(const CAMSEG::ActivityFilter p_filter);
        void setDeliveryFilter(const CAMSEG::DeliveryStatusFilter p_filter);
        void setStringFilter(const QString & p_filter);

        void objectAddedModified(CAMSEG::ObjectType p_type);

    private:
        void reloadObjectList();
        void sortList();
        void setupHeaders();
        void addObjectToList(const CAMSEG::CampaignObject* p_object);


        const CAMSEG::Campaign* m_campaign;
        const CAMSEG::ObjectType m_objectType;

        int m_idCategory;
        CAMSEG::ActivityFilter m_activityFilter;
        CAMSEG::DeliveryStatusFilter m_deliveryFilter;
        QString m_stringFilter;

        QList<CampaignModelItem> m_objects;
        QStringList m_listHeaders;

        Qt::SortOrder m_sortOrder;
        int m_sortedColumn;

        friend class CampaignModelItem;
};



#endif // CAMPAIGNMODEL_H
